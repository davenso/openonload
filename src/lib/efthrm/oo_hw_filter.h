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

#ifndef __ONLOAD_HW_FILTER_H__
#define __ONLOAD_HW_FILTER_H__


struct tcp_helper_resource_s;


/* Can be used for hwport_mask parameter when filter should be installed
 * for all interfaces in a stack.
 */
#define OO_HW_PORT_ALL ((unsigned) -1)

/* Use when no vlan should be specified for this filter.
 */
#define OO_HW_VLAN_UNSPEC ((ci_uint16) -1)

/* The default vlan.
 */
#define OO_HW_VLAN_DEFAULT (0)

#define OO_HW_SRC_FLAG_LOOPBACK (1)


/* Initialise filter object. */
extern void oo_hw_filter_init(struct oo_hw_filter* oofilter);

/* Remove all filters and disassociate with stack. */
extern void oo_hw_filter_clear(struct oo_hw_filter* oofilter);

/* Remove specified filters.  Association with stack remains. */
extern void oo_hw_filter_clear_hwports(struct oo_hw_filter* oofilter,
                                       unsigned hwport_mask);

/* Add filters on specified hwports, if needed.  Must already be associated
 * with a stack.
 *
 * NB. This call does not clear filters for interfaces not indicated in
 * hwport_mask.  You need to call oo_hw_filter_clear_hwports() as well if
 * you want to do that.
 *
 * Attempts to add a filter to all requested interfaces, even if an error
 * occurs part way through.  Returns error code from first failure
 * encountered, or 0 if all were okay.  On error, use
 * oo_hw_filter_hwports() to determine which interfaces have filters in
 * case of error.
 *
 * A filter specifying vlan_id is used for filters on ports in both hwport_mask
 * and set_vlan_mask.
 */
extern int oo_hw_filter_add_hwports(struct oo_hw_filter* oofilter,
                                    int protocol,
                                    unsigned saddr, int sport,
                                    unsigned daddr, int dport,
                                    ci_uint16 vlan_id, unsigned set_vlan_mask,
                                    unsigned hwport_mask, unsigned src_flags);

/* Clear existing filter, if any.  The insert new filters and associate
 * filter object with given stack.
 *
 * If we fail to insert any filters the filter is cleared.
 */
extern int oo_hw_filter_set(struct oo_hw_filter* oofilter,
                            struct tcp_helper_resource_s* trs, int protocol,
                            unsigned saddr, int sport,
                            unsigned daddr, int dport,
                            ci_uint16 vlan_id, unsigned set_vlan_mask,
                            unsigned hwport_mask, unsigned src_flags);

/* Redirect filter to direct packets to a different stack.  This is similar
 * to doing clear then set, except that it is guaranteed that (for
 * interfaces common to old and new stacks) no packets will slip through
 * the filter during the redirection.
 */
extern int oo_hw_filter_update(struct oo_hw_filter* oofilter,
                               struct tcp_helper_resource_s* new_stack,
                               int protocol,
                               unsigned saddr, int sport,
                               unsigned daddr, int dport,
                               ci_uint16 vlan_id, unsigned set_vlan_mask,
                               unsigned hwport_mask, unsigned src_flags);


/* Transfer filters on ports in hwport_mask from oofilter_old to oofilter_new.
 * Both oofilter_old and oofilter_new must point to the same stack - this
 * function simply transfers the filters, it does not update them in any way.
 */
extern void oo_hw_filter_transfer(struct oo_hw_filter* oofilter_old,
                              struct oo_hw_filter* oofilter_new,
                              unsigned hwport_mask);


/* Return the set of hwports that this filter is installed on.
 *
 * Result is zero if filter is not set, whether or not it is associated
 * with a stack.
 */
extern unsigned oo_hw_filter_hwports(struct oo_hw_filter* oofilter);


extern int oo_hw_filter_set_thc(struct oo_hw_filter* oofilter,
                                struct tcp_helper_cluster_s* thc, int protocol,
                                unsigned daddr, int dport,
                                unsigned hwport_mask);

#endif  /* __ONLOAD_HW_FILTER_H__ */
