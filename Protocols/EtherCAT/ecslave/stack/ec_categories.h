#ifndef  __CATEGORIES__
#define __CATEGORIES__

#define EC_FIRST_SII_CATEGORY_OFFSET 0x040

#define CAT_TYPE_NOP	0x00

#define CAT_TYPE_STRINGS 0x000A

#define CAT_TYPE_DATA_TYPE 0x014

#define CAT_TYPE_GENERAL  0x001E
#define CAT_TYPE_FMMU	  0x0028
#define CAT_TYPE_SYNCM	  0x0029
#define CAT_TYPE_TXPDO	  0x0032
#define CAT_TYPE_RXPDO	  0x0033
#define CAT_TYPE_DC	  0x003C
#define CAT_TYPE_END	  0xFFFF


typedef struct {
	uint16_t pdi_control; // 0x000
	uint16_t pdi_configuration; // 0x001
	uint16_t sync_impulse_len; // 0x002
	uint16_t pdi_configuration2;  // 0x003
	uint16_t alias; // 0x004  /**< The slaves alias if not equal to 0. */
	uint8_t  reverved1[4]; // 0x005
	uint16_t checksum; //0x007
	uint32_t vendor_id; //0x008 	/**< Vendor-ID stored on the slave. */
	uint32_t product_code; // 0x00a	   /**< Product-Code stored on the slave. */
	uint32_t revision_number; // 0x00c /**< Revision-Number stored on the slave. */
	uint32_t serial_number; // 0x00e  /**< Serial-Number stored on the slave. */
	uint8_t  reserved2[8]; ///0x013
	uint16_t boot_rx_mailbox_offset; // 0x0014
	uint16_t boot_rx_mailbox_size; // 0x0015
	uint16_t boot_tx_mailbox_offset; // 0x0016
	uint16_t boot_tx_mailbox_size; // 0x0017
	uint16_t std_rx_mailbox_offset; //0x0018 /* sync mananer starts here */
	uint16_t std_rx_mailbox_size; // 0x0019
	uint16_t std_tx_mailbox_offset; // 0x01a
	uint16_t std_tx_mailbox_size; // 0x01b
	uint16_t mailbox_protocols; // 0x01c
	uint8_t  reserved3[66]; // 0x2b
	uint16_t eprom_size_kbits; // 0x03e
	uint16_t version; //0x03f
} ec_sii_t;

#define GROUP_IDX		0
#define IMAGE_IDX		1
#define ORDER_IDX		2
#define NAME_IDX		3
#define RXPDO_CAT_NAME_IDX	4
#define TXPDO_CAT_NAME_IDX	5
#define TX_PDO1_NAME_IDX	6
#define TX_PDO2_NAME_IDX	7
#define RX_PDO1_NAME_IDX	8
#define RX_PDO2_NAME_IDX	9

#define GROUP_STRING "Raz Ben Jehuda"
#define IMAGE_STRING "LIBIX IMAGE"
#define ORDER_STRING "LIBIX ORDER"
#define NAME_STRING "OPEN SOURCE DRIVE"
#define RXPDO_CAT_STRING "LIBIX RX PDO"
#define TXPDO_CAT_STRING "LIBIX TX PDO"
#define TX_PDO1_NAME "TXPDO1 LIBIX"
#define TX_PDO2_NAME "TXPDO2 LIBIX"
#define RX_PDO1_NAME "RXPDO1 LIBIX"
#define RX_PDO2_NAME "RXPDO2 LIBIX"

#define SDO_ENABLED 		0x001
#define	SDO_INFO		0x002
#define	PDO_ASSIGN		0x004
#define	PDO_CONF		0x008
#define	STARTUP_UPLOAD		0x010
#define	SDO_COMPLETE_ACCESS	0x020

#define STRING0 GROUP_STRING
#define STRING1 IMAGE_STRING
#define STRING2 ORDER_STRING
#define STRING3 NAME_STRING
#define STRING4	RXPDO_CAT_STRING
#define STRING5 TXPDO_CAT_STRING
#define STRING6	TX_PDO1_NAME
#define STRING7 TX_PDO2_NAME
#define STRING8 RX_PDO1_NAME
#define STRING9 RX_PDO2_NAME
#define NR_STRINGS  10

#define __SIZEOF__(a) ( sizeof(a) + 1)

#define EC_PORT_MII	0x01
#define PORT0_SHIFT	0
#define NR_PDOS		2

#define STRINGS_SIZE ( __SIZEOF__( STRING9 ) + \
		__SIZEOF__( STRING8 ) + __SIZEOF__ (STRING7) + __SIZEOF__ (STRING6)  \
		+ __SIZEOF__(  STRING5) + __SIZEOF__( STRING4) + __SIZEOF__( STRING3) \
		+ __SIZEOF__( STRING2) + __SIZEOF__( STRING1) + __SIZEOF__( STRING0) \
		+ ((NR_STRINGS +1) * sizeof(uint8_t)))


// table 25
typedef struct {
	uint16_t index;
	uint8_t subindex;
	uint8_t name_idx;	/* idx to strings */
	uint8_t data_type;
	uint8_t bit_len;
	uint16_t flags;
} pdo_entry;

// table 24
typedef struct {
	uint16_t pdo_index;
	uint8_t entries;
	int8_t syncm;
	uint8_t synchronization;
	uint8_t name_idx;
	uint16_t flags;
	pdo_entry pdo[NR_PDOS];
} category_pdo;

// table 23
typedef struct {
	uint16_t phys_start_address;
	uint16_t length;
	uint8_t ctrl_reg;
	uint8_t status_reg;
	uint8_t enable_syncm;
	uint8_t syncm_type;
} category_syncm;

typedef struct __attribute__packed__ {
	uint8_t fmmu0;
	uint8_t fmmu1;
} category_fmmu;

typedef struct {
	uint8_t groupd_idx;	/*index to strings  */
	uint8_t img_idx;	/* index to strings */
	uint8_t order_idx;	/* index to strings */
	uint8_t name_idx;	/* index to strings */
	uint8_t reserved;
	uint8_t coe_details;
	uint8_t foe_detials;
	uint8_t eoe_detials;
	uint8_t soe_detials;
	uint8_t ds402channels;
	uint8_t sysman_class;
	uint8_t flags;
	uint16_t current_on_bus;
	uint16_t pad_byte1;
	uint16_t physical_port __attribute__packed__;
	uint8_t pad_byte2[14];
} category_general;

typedef struct  __attribute__packed__  __category_strings__{
	uint8_t nr_strings;

	uint8_t str0_len;
	char str0[ __SIZEOF__ (STRING0) ];

	uint8_t str1_len;
	char str1[ __SIZEOF__ (STRING1)];

	uint8_t str2_len;
	char str2[__SIZEOF__ (STRING2)];

	uint8_t str3_len;
	char str3[__SIZEOF__ (STRING3)];

	uint8_t str4_len;
	char str4[__SIZEOF__ (STRING4)];

	uint8_t str5_len;
	char str5[__SIZEOF__ (STRING5)];

	uint8_t str6_len;
	char str6[__SIZEOF__ (STRING6)];

	uint8_t str7_len;
	char str7[__SIZEOF__ (STRING7)];

	uint8_t str8_len;
	char str8[__SIZEOF__ (STRING8)];

	uint8_t str9_len;
	char str9[__SIZEOF__ (STRING9)];

	uint8_t pad[(STRINGS_SIZE % 2) +2];
} category_strings;

typedef struct {
	uint16_t type:15;
	uint16_t vendor_specific:1;
	uint16_t size;
} category_header;

typedef struct __sii_categories__{
	ec_sii_t sii __attribute__packed__;

	category_header strings_hdr __attribute__packed__;
	category_strings strings;

	category_header general_hdr __attribute__packed__;
	category_general general __attribute__packed__;

	category_header fmmu_hdr __attribute__packed__;
	category_fmmu fmmu;

	category_header syncm_hdr0 __attribute__packed__;
	category_syncm syncm0  __attribute__packed__;

	category_header syncm_hdr1 __attribute__packed__;
	category_syncm syncm1  __attribute__packed__;

	category_header txpdo_hdr __attribute__packed__;
	category_pdo txpdo  __attribute__packed__;

	category_header rxpdo_hdr __attribute__packed__;
	category_pdo rxpdo  __attribute__packed__;

	category_header endhdr __attribute__packed__;

} sii_categories;

#endif
