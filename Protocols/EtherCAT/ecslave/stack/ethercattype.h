/*
 * Simple Open EtherCAT Master Library 
 *
 * File    : ethercattype.h
 * Version : 1.2.5
 * Date    : 09-04-2011
 * Copyright (C) 2005-2011 Speciaal Machinefabriek Ketels v.o.f.
 * Copyright (C) 2005-2011 Arthur Ketels
 * Copyright (C) 2008-2009 TU/e Technische Universiteit Eindhoven 
 *
 * SOEM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 as published by the Free
 * Software Foundation.
 *
 * SOEM is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 *
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 *
 * The EtherCAT Technology, the trade name and logo “EtherCAT” are the intellectual
 * property of, and protected by Beckhoff Automation GmbH. You can use SOEM for
 * the sole purpose of creating, using and/or selling or otherwise distributing
 * an EtherCAT network master provided that an EtherCAT Master License is obtained
 * from Beckhoff Automation GmbH.
 *
 * In case you did not receive a copy of the EtherCAT Master License along with
 * SOEM write to Beckhoff Automation GmbH, Eiserstraße 5, D-33415 Verl, Germany
 * (www.beckhoff.com).
 */

/** \file
 * \brief	
 * General typedefs and defines for EtherCAT.
 *
 * Defines that could need optimalisation for specific applications
 * are the EC_TIMEOUTxxx. Assumptions for the standard settings are a
 * standard linux PC or laptop and a wired connection to maximal 100 slaves.
 * For use with wireless connections or lots of slaves the timouts need
 * increasing. For fast systems running Xenomai and RT-net or alike the
 * timeouts need to be shorter.	
 */

#ifndef _EC_TYPE_H
#define _EC_TYPE_H


/** return value general error */
#define EC_ERROR			-3
/** return value no frame returned */
#define EC_NOFRAME			-1
/** return value unknown frame received */
#define EC_OTHERFRAME		-2
/** maximum EtherCAT frame length in bytes */
#define EC_MAXECATFRAME		1518
/** maximum EtherCAT LRW frame length in bytes */
/* MTU - Ethernet header - length - datagram header - WCK - FCS */
#define EC_MAXLRWDATA		EC_MAXECATFRAME - 14 - 2 - 10 - 2 - 4
/* #define EC_MAXLRWDATA		45 */
/** size of DC datagram used in first LRW frame */
#define EC_FIRSTDCDATAGRAM	20
/** standard frame buffer size in bytes */
#define EC_BUFSIZE			EC_MAXECATFRAME
/** datagram type EtherCAT */
#define EC_ECATTYPE			0x1000
/** number of frame buffers per channel (tx, rx1 rx2) */
#define EC_MAXBUF			16
/** timeout value in us for tx frame to return to rx */
#define EC_TIMEOUTRET		500
/** timeout value in us for return "safe" variant (f.e. wireless) */
#define EC_TIMEOUTSAFE		20000
/** timeout value in us for EEPROM access */
#define EC_TIMEOUTEEP		20000
/** timeout value in us for tx mailbox cycle */
#define EC_TIMEOUTTXM		20000
/** timeout value in us for rx mailbox cycle */
#define EC_TIMEOUTRXM		700000
/** timeout value in us for check statechange */
#define EC_TIMEOUTSTATE		2000000
/** size of EEPROM bitmap cache */
#define EC_MAXEEPBITMAP		128
/** size of EEPROM cache buffer */
#define EC_MAXEEPBUF		EC_MAXEEPBITMAP << 5
/** default number of retries if wkc <= 0 */
#define EC_DEFAULTRETRIES	3

/** definition for frame buffers */

/** ethernet header definition */
typedef struct   __attribute__((__packed__))
{
	/** destination MAC */
	uint16_t  da0,da1,da2;
	/** source MAC */
	uint16_t  sa0,sa1,sa2;
	/** ethernet type */
	uint16_t  etype;
} ec_etherheadert;


typedef struct   __attribute__((__packed__)) {
	uint16_t elength;
}ec_frame_header;

/** EtherCAT datagram header definition */
typedef struct __attribute__((__packed__))
{
	/** EtherCAT command, see ec_cmdtype */
	uint8_t   command;
	/** index, used in SOEM for Tx to Rx recombination */
	uint8_t   index;
	/** ADP */
	uint16_t adp;
	/** ADO */
	uint16_t  ado;
	/** length of data portion in datagram */
	uint16_t  dlength;
	/** interrupt, currently unused */
	uint16_t  irpt;
} ec_dgram;

/** Possible EtherCAT slave states */
typedef enum
{
    /** Init state*/
    EC_STATE_INIT       = 0x01,
    /** Pre-operational. */
    EC_STATE_PRE_OP     = 0x02,
    /** Boot state*/
    EC_STATE_BOOT	= 0x03,
    /** Safe-operational. */
    EC_STATE_SAFE_OP    = 0x04,
    /** Operational */
    EC_STATE_OPERATIONAL= 0x08,
    /** Error or ACK error */
    EC_STATE_ACK	= 0x10,
    EC_STATE_ERROR	= 0x10
} ec_state;

/** Possible buffer states */
typedef enum
{
	/** Empty */
    EC_BUF_EMPTY		= 0x00,
	/** Allocated, but not filled */
    EC_BUF_ALLOC		= 0x01,
	/** Transmitted */
    EC_BUF_TX			= 0x02,
	/** Received, but not consumed */
    EC_BUF_RCVD         = 0x03,
	/** Cycle completed */
    EC_BUF_COMPLETE		= 0x04
} ec_bufstate;

/** Ethercat data types */
typedef enum
{
    ECT_BOOLEAN         = 0x0001,
    ECT_INTEGER8        = 0x0002,
    ECT_INTEGER16       = 0x0003,
    ECT_INTEGER32       = 0x0004,
    ECT_UNSIGNED8       = 0x0005,
    ECT_UNSIGNED16      = 0x0006,
    ECT_UNSIGNED32      = 0x0007,
    ECT_REAL32          = 0x0008,
    ECT_VISIBLE_STRING  = 0x0009,
    ECT_OCTET_STRING    = 0x000A,
    ECT_UNICODE_STRING  = 0x000B,
    ECT_TIME_OF_DAY     = 0x000C,
    ECT_TIME_DIFFERENCE = 0x000D,
    ECT_DOMAIN          = 0x000F,
    ECT_INTEGER24       = 0x0010,
    ECT_REAL64          = 0x0011,
    ECT_INTEGER64       = 0x0015,
    ECT_UNSIGNED24      = 0x0016,
    ECT_UNSIGNED64      = 0x001B,
    ECT_BIT1            = 0x0030,
    ECT_BIT2            = 0x0031,
    ECT_BIT3            = 0x0032,
    ECT_BIT4            = 0x0033,
    ECT_BIT5            = 0x0034,
    ECT_BIT6            = 0x0035,
    ECT_BIT7            = 0x0036,
    ECT_BIT8            = 0x0037
} ec_datatype;

/** Ethercat command types */
typedef enum 
{
    /** No operation */
    EC_CMD_NOP          = 0x00,
    /** Auto Increment Read */
    EC_CMD_APRD,
    /** Auto Increment Write */
    EC_CMD_APWR,
    /** Auto Increment Read Write */
    EC_CMD_APRW,
    /** Configured Address Read */
    EC_CMD_FPRD,
    /** Configured Address Write */
    EC_CMD_FPWR,
    /** Configured Address Read Write */
    EC_CMD_FPRW,
    /** Broadcast Read */
    EC_CMD_BRD,
    /** Broaddcast Write */
    EC_CMD_BWR,
    /** Broadcast Read Write */
    EC_CMD_BRW,
    /** Logical Memory Read */
    EC_CMD_LRD,
    /** Logical Memory Write */
    EC_CMD_LWR,
    /** Logical Memory Read Write */
    EC_CMD_LRW,
    /** Auto Increment Read Mulitple Write */
    EC_CMD_ARMW,
    /** Configured Read Mulitple Write */
    EC_CMD_FRMW
    /** Reserved */
} ec_cmdtype;

/** Ethercat EEprom command types */
typedef enum 
{
    /** No operation */
    EC_ECMD_NOP         = 0x0000,
    /** Read */
    EC_ECMD_READ		= 0x0100,
    /** Write */
    EC_ECMD_WRITE		= 0x0201,
    /** Reload */
    EC_ECMD_RELOAD		= 0x0300
} ec_ecmdtype;

/** EEprom state machine read size */
#define EC_ESTAT_R64    0x0040
/** EEprom state machine busy flag */
#define EC_ESTAT_BUSY   0x8000
/** EEprom state machine error flag mask */
#define EC_ESTAT_EMASK	0x7800
/** EEprom state machine error acknowledge */
#define EC_ESTAT_NACK	0x2000

/* Ethercat SSI (Slave Information Interface) */

/** Start address SII sections in Eeprom */
#define ECT_SII_START   0x0040

/** Item offsets in SII general section */
enum
{
    ECT_SII_MANUF       = 0x0008,
    ECT_SII_ID          = 0x000a,
    ECT_SII_REV         = 0x000c,
	ECT_SII_BOOTRXMBX	= 0x0014,
	ECT_SII_BOOTTXMBX	= 0x0016,
	ECT_SII_MBXSIZE     = 0x0019,
    ECT_SII_TXMBXADR    = 0x001a,
    ECT_SII_RXMBXADR    = 0x0018,
    ECT_SII_MBXPROTO	= 0x001c
		
};

/** Mailbox types definitions */
enum
{
	/** Error mailbox type */
	ECT_MBXT_ERR		= 0x00,
	/** ADS over EtherCAT mailbox type */
	ECT_MBXT_AOE,
	/** Ethernet over EtherCAT mailbox type */
	ECT_MBXT_EOE,
	/** CANopen over EtherCAT mailbox type */
	ECT_MBXT_COE,
	/** File over EtherCAT mailbox type */
	ECT_MBXT_FOE,
	/** Servo over EtherCAT mailbox type */
	ECT_MBXT_SOE,
	/** Vendor over EtherCAT mailbox type */
	ECT_MBXT_VOE		= 0x0f
};

/** CoE mailbox types */
enum
{
	ECT_COES_EMERGENCY	= 0x01,
	ECT_COES_SDOREQ,
	ECT_COES_SDORES,
	ECT_COES_TXPDO,
	ECT_COES_RXPDO,
	ECT_COES_TXPDO_RR,
	ECT_COES_RXPDO_RR,
	ECT_COES_SDOINFO
};

/** CoE SDO commands */
enum
{
	ECT_SDO_DOWN_INIT = 0x21,
	ECT_SDO_DOWN_INIT_CA = 0x31,
	ECT_SDO_UP_REQ = 0x40,
	ECT_SDO_UP_REQ_CA = 0x50,
	ECT_SDO_SEG_UP_REQ = 0x60,
	ECT_SDO_ABORT = 0x80
};

/** CoE Object Description commands */
enum
{
	ECT_GET_ODLIST_REQ = 0x01,
	ECT_GET_ODLIST_RES = 0x02,
	ECT_GET_OD_REQ = 0x03,
	ECT_GET_OD_RES = 0x04,
	ECT_GET_OE_REQ = 0x05,
	ECT_GET_OE_RES = 0x06,
	ECT_SDOINFO_ERROR = 0x07
};

/** FoE opcodes */
enum
{
	ECT_FOE_READ	= 0x01,
	ECT_FOE_WRITE,
	ECT_FOE_DATA,
	ECT_FOE_ACK,
	ECT_FOE_ERROR,
	ECT_FOE_BUSY
};

/** SoE opcodes */
enum
{
	ECT_SOE_READREQ	= 0x01,
	ECT_SOE_READRES,
	ECT_SOE_WRITEREQ,
	ECT_SOE_WRITERES,
	ECT_SOE_NOTIFICATION,
	ECT_SOE_EMERGENCY
};

/** Ethercat registers */
enum 
{
    ECT_REG_TYPE        = 0x0000,  /* base type 1byte fsm_slave_scan.c line 296 */
    ECT_BASE_REVISION   = 0x0001,  /* base revision. 1byte */
    ECT_BASE_BUILD1		= 0x0002,  /* base build .2bytes   */
    ECT_BASE_BUILD2		= 0x0003,  /* base fmmu count 1byte         */
    ECT_BASE_FMMUS		= 0x0004,  /* base sync count 1byte */
    ECT_BASE_SYNCM		= 0x0005,  /* octet 1      0x000F one port. mii */
    ECT_REG_PORTDES		= 0x0007,
    ECT_REG_ESCSUP      = 0x0008,
    ECT_REG_STADR       = 0x0010,
    ECT_REG_ALIAS		= 0x0012,
    ECT_REG_DLCTL       = 0x0100,
    ECT_REG_DLPORT      = 0x0101,
    ECT_REG_DLALIAS     = 0x0103,
    ECT_REG_DLSTAT      = 0x0110,
    ECT_REG_ALCTL       = 0x0120,
    ECT_REG_ALSTAT      = 0x0130,
    ECT_REG_ALSTATCODE	= 0x0134,
    ECT_REG_PDICTL      = 0x0140,
    ECT_REG_IRQMASK     = 0x0200,
    ECT_REG_RXERR       = 0x0300,
    ECT_REG_EEPCFG	= 0x0500,
    ECT_REG_EEPCTL      = 0x0502,
    ECT_REG_EEPSTAT     = 0x0502,
    ECT_REG_EEPADR      = 0x0504,
    ECT_REG_EEPDAT      = 0x0508,
    ECT_REG_FMMU0       = 0x0600,
    ECT_REG_FMMU1       = ECT_REG_FMMU0 + 0x10,
    ECT_REG_FMMU2       = ECT_REG_FMMU1 + 0x10,
    ECT_REG_FMMU3       = ECT_REG_FMMU2 + 0x10,
    ECT_REG_SM0         = 0x0800,
    ECT_REG_SM1         = ECT_REG_SM0 + 0x08,
    ECT_REG_SM2         = ECT_REG_SM1 + 0x08,
    ECT_REG_SM3         = ECT_REG_SM2 + 0x08,
    ECT_REG_SM0STAT     = ECT_REG_SM0 + 0x05,
    ECT_REG_SM1STAT     = ECT_REG_SM1 + 0x05,
    ECT_REG_SM1ACT      = ECT_REG_SM1 + 0x06,
    ECT_REG_SM1CONTR    = ECT_REG_SM1 + 0x07,
    ECT_REG_RX_TIME_PORT0 = 0x0900, /* receive time of packet on port 0 */
    ECT_REG_RX_TIME_PORT1 = 0x0904, /* receive time of packet on port 1*/
    ECT_REG_RX_TIME_PORT2 = 0x0908,
    ECT_REG_RX_TIME_PORT3 = 0x090C,
    ECT_REG_DCSYSTIME   = 0x0910, /* the system time */
    ECT_REG_DCSOF       = 0x0918, 
    ECT_REG_DCSYSOFFSET = 0x0920, /* offset from 910 */
    ECT_REG_DCSYSDELAY  = 0x0928,
    ECT_REG_DCSYSDRIFT   = 0x092C, /* drift register */
    ECT_REG_DCSPEEDCNT  = 0x0930,
    ECT_REG_DCTIMEFILT  = 0x0934,
    ECT_REG_DCCUC       = 0x0980, /* cycle unit control -> */
    ECT_REG_DCSYNCACT   = 0x0981, /* ActiveActive */
    ECT_REG_SYNC0START  = 0x0990, /* sync0 start time */
    ECT_REG_SYNC0CYCLE  = 0x09A0, /* sync0 cycle time */
    ECT_REG_DCCYCLE1    = 0x09A4
};


#define MBOX_SIZE	30
#define NR_SYNCM	2
#define	SYNCM_SIZE	128


static inline int16_t __syncm_start(int i)
{
        return ECT_REG_DCCYCLE1 + i*SYNCM_SIZE;
}       

static inline int16_t __sdo_high(void)
{
        return __syncm_start(NR_SYNCM-1) + SYNCM_SIZE;
}

static inline int16_t __mbox_start(void)
{
	return __syncm_start(0);
}

/** standard SDO Sync Manager Communication Type */
#define ECT_SDO_SMCOMMTYPE		0x1c00
/** standard SDO PDO assignment */
#define ECT_SDO_PDOASSIGN		0x1c10
/** standard SDO RxPDO assignment */
#define ECT_SDO_RXPDOASSIGN     0x1c12
/** standard SDO TxPDO assignment */
#define ECT_SDO_TXPDOASSIGN     0x1c13

/** Ethercat packet type */
#define ETH_P_ECAT              0x88A4

/** Error types */
typedef enum
{
    EC_ERR_TYPE_SDO_ERROR       = 0,
    EC_ERR_TYPE_EMERGENCY       = 1,
    EC_ERR_TYPE_PACKET_ERROR    = 3,
    EC_ERR_TYPE_SDOINFO_ERROR   = 4,
	EC_ERR_TYPE_FOE_ERROR	    = 5,         
	EC_ERR_TYPE_FOE_BUF2SMALL   = 6,         
	EC_ERR_TYPE_FOE_PACKETNUMBER= 7,
	EC_ERR_TYPE_SOE_ERROR		= 8
} ec_err_type;


enum {
    EC_MBOX_AOE = 0x01, /**< ADS over EtherCAT */
    EC_MBOX_EOE = 0x02, /**< Ethernet over EtherCAT */
    EC_MBOX_COE = 0x04, /**< CANopen over EtherCAT */
    EC_MBOX_FOE = 0x08, /**< File-Access over EtherCAT */
    EC_MBOX_SOE = 0x10, /**< Servo-Profile over EtherCAT */
    EC_MBOX_VOE = 0x20  /**< Vendor specific */
};  


#ifdef __MAKE_DEBUG__
	#define ec_printf	printf
#else
#ifndef ARDUINO
	void ec_printf(const char *str, ...);
#endif
#endif

#endif /* _EC_TYPE_H */
