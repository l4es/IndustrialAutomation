/******************************************************************************
 *
 *  $Id: globals.h,v 7421bca39b6d 2010/04/30 08:53:56 ch1010858 $
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

/** \file
 * Global definitions and macros.
 */

/*****************************************************************************/

#ifndef __EC_MASTER_GLOBALS_H__
#define __EC_MASTER_GLOBALS_H__

//#include "globals.h"
//#include "ecrt.h"

/******************************************************************************
 * EtherCAT master
 *****************************************************************************/

/** Datagram timeout in microseconds. */
#define EC_IO_TIMEOUT 500

/** SDO injection timeout in microseconds. */
#define EC_SDO_INJECTION_TIMEOUT 10000

/** Time to send a byte in nanoseconds.
 *
 * t_ns = 1 / (100 MBit/s / 8 bit/byte) = 80 ns/byte
 */
#define EC_BYTE_TRANSMISSION_TIME_NS 80

/** Number of state machine retries on datagram timeout. */
#define EC_FSM_RETRIES 3

/** Seconds to wait before fetching SDO dictionary
    after slave entered PREOP state. */
#define EC_WAIT_SDO_DICT 3

/** Minimum size of a buffer used with ec_state_string(). */
#define EC_STATE_STRING_SIZE 32

/** Maximum SII size in words, to avoid infinite reading. */
#define EC_MAX_SII_SIZE 4096

/** Number of statistic rate intervals to maintain. */
#define EC_RATE_COUNT 3

/******************************************************************************
 * EtherCAT protocol
 *****************************************************************************/

/** Size of an EtherCAT frame header. */
#define EC_FRAME_HEADER_SIZE 2

/** Size of an EtherCAT datagram header. */
#define EC_DATAGRAM_HEADER_SIZE 10

/** Size of an EtherCAT datagram footer. */
#define EC_DATAGRAM_FOOTER_SIZE 2

/** Size of the EtherCAT address field. */
#define EC_ADDR_LEN 4

/** Resulting maximum data size of a single datagram in a frame. */
#define EC_MAX_DATA_SIZE (ETH_DATA_LEN - EC_FRAME_HEADER_SIZE \
                          - EC_DATAGRAM_HEADER_SIZE - EC_DATAGRAM_FOOTER_SIZE)

/** Mailbox header size.  */
#define EC_MBOX_HEADER_SIZE 6

/** Word offset of first SII category. */
#define EC_FIRST_SII_CATEGORY_OFFSET 0x40

/** Size of a sync manager configuration page. */
#define EC_SYNC_PAGE_SIZE 8

/** Maximum number of FMMUs per slave. */
#define EC_MAX_FMMUS 16

/** Size of an FMMU configuration page. */
#define EC_FMMU_PAGE_SIZE 16

/** Number of DC sync signals. */
#define EC_SYNC_SIGNAL_COUNT 2

/** Size of the datagram description string.
 *
 * This is also used as the maximum lenth of EoE device names.
 **/
#define EC_DATAGRAM_NAME_SIZE 20

/** Slave state mask.
 *
 * Apply this mask to a slave state byte to get the slave state without
 * the error flag.
 */
#define EC_SLAVE_STATE_MASK 0x0F

/** 
 * maximum slaves mc allows
*/
#define EC_MAX_SLAVES	32
/** State of an EtherCAT slave.
 */
typedef enum {
    EC_SLAVE_STATE_UNKNOWN = 0x00,
    /**< unknown state */
    EC_SLAVE_STATE_INIT = 0x01,
    /**< INIT state (no mailbox communication, no IO) */
    EC_SLAVE_STATE_PREOP = 0x02,
    /**< PREOP state (mailbox communication, no IO) */
    EC_SLAVE_STATE_BOOT = 0x03,
    /**< Bootstrap state (mailbox communication, firmware update) */
    EC_SLAVE_STATE_SAFEOP = 0x04,
    /**< SAFEOP (mailbox communication and input update) */
    EC_SLAVE_STATE_OP = 0x08,
    /**< OP (mailbox communication and input/output update) */
    EC_SLAVE_STATE_ACK_ERR = 0x10
    /**< Acknowledge/Error bit (no actual state) */
} ec_slave_state_t;

enum {
    EC_SDO_ENTRY_ACCESS_PREOP, /**< Access rights in PREOP. */
    EC_SDO_ENTRY_ACCESS_SAFEOP, /**< Access rights in SAFEOP. */
    EC_SDO_ENTRY_ACCESS_OP, /**< Access rights in OP. */
    EC_SDO_ENTRY_ACCESS_COUNT /**< Number of states. */
};

/*****************************************************************************/

/** Convenience macro for printing EtherCAT-specific information to syslog.
 *
 * This will print the message in \a fmt with a prefixed "EtherCAT: ".
 *
 * \param fmt format string (like in printf())
 * \param args arguments (optional)
 */
#define ECAT_INFO(fmt, args...) \
    printk(KERN_INFO "EtherCAT: " fmt, ##args)

/** Convenience macro for printing EtherCAT-specific errors to syslog.
 *
 * This will print the message in \a fmt with a prefixed "EtherCAT ERROR: ".
 *
 * \param fmt format string (like in printf())
 * \param args arguments (optional)
 */
#define ECAT_ERR(fmt, args...) \
    printk(KERN_ERR "EtherCAT ERROR: " fmt, ##args)

/** Convenience macro for printing EtherCAT-specific warnings to syslog.
 *
 * This will print the message in \a fmt with a prefixed "EtherCAT WARNING: ".
 *
 * \param fmt format string (like in printf())
 * \param args arguments (optional)
 */
#define ECAT_WARN(fmt, args...) \
    printk(KERN_WARNING "EtherCAT WARNING: " fmt, ##args)

/** Convenience macro for printing EtherCAT debug messages to syslog.
 *
 * This will print the message in \a fmt with a prefixed "EtherCAT DEBUG: ".
 *
 * \param fmt format string (like in printf())
 * \param args arguments (optional)
 */
#define ECAT_DBG(fmt, args...) \
    printk(KERN_DEBUG "EtherCAT DEBUG: " fmt, ##args)

/*****************************************************************************/

/** Absolute value.
 */
#define EC_ABS(X) ((X) >= 0 ? (X) : -(X))

/*****************************************************************************/
#endif
