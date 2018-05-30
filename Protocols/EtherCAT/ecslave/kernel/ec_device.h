/******************************************************************************
 *
 *  $Id: ec_device.h,v 11c0b2caa253 2009/02/24 12:51:39 fp $
 *
 *  Copyright (C) 2006-2008  Florian Pose, Ingenieurgemeinschaft IgH
 *
 *  This file is part of the IgH EtherCAT Master.
 *
 *  The IgH EtherCAT Master is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License version 2, as
 *  published by the Free Software Foundation.
 *
 *  The IgH EtherCAT Master is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *  Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with the IgH EtherCAT Master; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  ---
 *
 *  The license mentioned above concerns the source code only. Using the
 *  EtherCAT technology and brand is only permitted in compliance with the
 *  industrial property and similar rights of Beckhoff Automation GmbH.
 *
 *****************************************************************************/

#ifndef __EC_DEVICE_H__
#define __EC_DEVICE_H__

//#include <linux/interrupt.h>
#include "ecdev.h"

struct ec_device
{
    ecat_node_t *ecat_node; /**< EtherCAT master/slave */
    struct net_device *dev; /**< pointer to the assigned net_device */
    struct sk_buff *processed_skb;
    struct list_head events;
    spinlock_t events_sync;
    struct ecat_event rx_time;
    // Frame statistics
    u64 tx_count; /**< Number of frames sent. */
    u64 last_tx_count; /**< Number of frames sent of last statistics cycle. */
    u64 rx_count; /**< Number of frames received. */
    u64 tx_bytes; /**< Number of frames sent. */
    u64 last_tx_bytes; /**< Number of bytes sent of last statistics cycle. */
    u64 tx_errors; /**< Number of transmit errors. */
    u64 last_loss; /**< Tx/Rx difference of last statistics cycle. */

};

/*****************************************************************************/

int ec_device_init(
        ec_device_t *device, /**< EtherCAT device */
        ecat_node_t *ecat_node /**< ethercat node owning the device */
        );

void ec_device_attach(
        ec_device_t *device, /**< EtherCAT device */
        struct net_device *net_dev /**< net_device structure */
        );
/*****************************************************************************/
void ec_device_detach(
        ec_device_t *device /**< EtherCAT device */
        );

/*****************************************************************************/
void ec_device_clear_stats(
        ec_device_t *device /**< EtherCAT device */
        );

/*****************************************************************************/
void ec_device_send(
	ec_device_t *device,
	struct sk_buff *skb
	);

#endif
