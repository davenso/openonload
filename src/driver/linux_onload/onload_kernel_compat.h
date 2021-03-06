/*
** Copyright 2005-2015  Solarflare Communications Inc.
**                      7505 Irvine Center Drive, Irvine, CA 92618, USA
** Copyright 2002-2005  Level 5 Networks Inc.
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of version 2 of the GNU General Public License as
** published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
*/

#ifndef __ONLOAD_KERNEL_COMPAT_H__
#define __ONLOAD_KERNEL_COMPAT_H__

#include <driver/linux_net/kernel_compat.h>
#include <driver/linux_affinity/autocompat.h>
#include <linux/file.h>

#ifndef current_fsuid
#define current_fsuid() current->fsuid
#endif
#ifndef current_fsgid
#define current_fsgid() current->fsgid
#endif

#ifdef EFX_HAVE_KMEM_CACHE_S
#define kmem_cache kmem_cache_s
#endif


/* >=2.6.24 has sig_kernel_* macros in the header;
 * 2.6.18 has them in .c */
#ifndef sig_kernel_only

#ifdef SIGEMT
#define M_SIGEMT	M(SIGEMT)
#else
#define M_SIGEMT	0
#endif

#if SIGRTMIN > BITS_PER_LONG
#define M(sig) (1ULL << ((sig)-1))
#else
#define M(sig) (1UL << ((sig)-1))
#endif
#define T(sig, mask) (M(sig) & (mask))

#define SIG_KERNEL_ONLY_MASK (\
	M(SIGKILL)   |  M(SIGSTOP)                                   )

#define SIG_KERNEL_STOP_MASK (\
	M(SIGSTOP)   |  M(SIGTSTP)   |  M(SIGTTIN)   |  M(SIGTTOU)   )

#define SIG_KERNEL_COREDUMP_MASK (\
        M(SIGQUIT)   |  M(SIGILL)    |  M(SIGTRAP)   |  M(SIGABRT)   | \
        M(SIGFPE)    |  M(SIGSEGV)   |  M(SIGBUS)    |  M(SIGSYS)    | \
        M(SIGXCPU)   |  M(SIGXFSZ)   |  M_SIGEMT                     )

#define SIG_KERNEL_IGNORE_MASK (\
        M(SIGCONT)   |  M(SIGCHLD)   |  M(SIGWINCH)  |  M(SIGURG)    )

#define sig_kernel_only(sig) \
		(((sig) < SIGRTMIN)  && T(sig, SIG_KERNEL_ONLY_MASK))
#define sig_kernel_coredump(sig) \
		(((sig) < SIGRTMIN)  && T(sig, SIG_KERNEL_COREDUMP_MASK))
#define sig_kernel_ignore(sig) \
		(((sig) < SIGRTMIN)  && T(sig, SIG_KERNEL_IGNORE_MASK))
#define sig_kernel_stop(sig) \
		(((sig) < SIGRTMIN)  && T(sig, SIG_KERNEL_STOP_MASK))

#endif

#ifndef __NFDBITS
# define __NFDBITS BITS_PER_LONG
#endif


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
# define f_vfsmnt f_path.mnt
#endif


#ifndef EFRM_HAVE_NETDEV_NOTIFIER_INFO
#define netdev_notifier_info_to_dev(info) (info)
#endif

#ifndef EFRM_HAVE_REINIT_COMPLETION
#define reinit_completion(c) INIT_COMPLETION(*c)
#endif

#ifndef EFRM_HAVE_GET_UNUSED_FD_FLAGS
#ifdef O_CLOEXEC
static inline int
efrm_get_unused_fd_flags(unsigned flags)
{
  int fd = get_unused_fd();
  struct files_struct *files = current->files;
  struct fdtable *fdt;

  if( fd < 0 )
    return fd;

  spin_lock(&files->file_lock);
  fdt = files_fdtable(files);
  if( flags & O_CLOEXEC)
    efx_set_close_on_exec(fd, fdt);
  else
    efx_clear_close_on_exec(fd, fdt);
  spin_unlock(&files->file_lock);

  return fd;
}
#undef get_unused_fd_flags
#define get_unused_fd_flags(flags) efrm_get_unused_fd_flags(flags)
#else /* ! O_CLOEXEC */
#define get_unused_fd_flags(flags) get_unused_fd()
#endif
#endif

#endif /* __ONLOAD_KERNEL_COMPAT_H__ */
