#ifndef modbus_common_H
#define modbus_common_H

#pragma pack(1)

#define MODPROT_ID 0

#define MODCMD_READ_MULTI_COILS        0x01
#define MODCMD_READ_MULTI_DISCRETE_INS 0x02
#define MODCMD_READ_MULTI_HOLDREGS     0x03
#define MODCMD_READ_MULTI_INPUTREGS    0x04
#define MODCMD_WRITE_SINGLE_COIL       0x05
#define MODCMD_WRITE_SINGLE_HOLDREG    0x06
#define MODCMD_WRITE_MULTI_COILS       0x0F
#define MODCMD_WRITE_MULTI_HOLDREGS    0x10

struct mod_pdu
{
   unsigned char  funcCode;
   unsigned short addrOffset;
   unsigned char  data[255];
};

#define MOD_UNUSED           1
#define MOD_RW_COILS         2
#define MOD_RW_HOLDREG       3
#define MOD_R_DISCRETE_IN    4
#define MOD_R_INPUTREG       5
#define MOD_R_FLOAT_HOLDREG  6

#pragma pack(8)

#endif
