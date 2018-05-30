#include "xgeneral.h"

static uint8_t* ecat_regs1 = 0;
static uint8_t* ecat_regs2 = 0;
static uint8_t* ecat_regs3 = 0;
static uint8_t* ecat_regs4 = 0;
static uint8_t* ecat_regs5 = 0 ;
static uint8_t* ecat_regs6 = 0;
static uint8_t* ecat_regs7 = 0;
static uint8_t* ecat_regs8 = 0;
static uint8_t* ecat_regs9 = 0;
static uint8_t* ecat_regs10 = 0;
static uint8_t* ecat_regs11 = 0;
static uint8_t* ecat_regs12 = 0;


int ecat_pool_init()
{
	ecat_regs1 = malloc(200);
	if (!ecat_regs1)
		return -1;
	ecat_regs2 = malloc(200);
	if (!ecat_regs2)
		return -2;
	ecat_regs3 = malloc(200);
	if (!ecat_regs3)
		return -3;
	ecat_regs4 = malloc(200);
	if (!ecat_regs4)
		return -4;
	ecat_regs5 = malloc(200);
	if (!ecat_regs4)
		return -4;
	ecat_regs6 = 0;
	if (!ecat_regs4)
		return -4;
	ecat_regs7 = 0;
	if (!ecat_regs4)
		return -4;
	ecat_regs8 = 0;
	if (!ecat_regs4)
		return -4;
	ecat_regs9 = 0;
	if (!ecat_regs4)
		return -4;
	ecat_regs10 = 0;
	if (!ecat_regs4)
		return -4;
	ecat_regs11 = 0;
	if (!ecat_regs4)
		return -4;
	ecat_regs12 = 0;
	if (!ecat_regs4)
		return -4;
}

/*
 * since obvious bug here is data shared between two arrays,
 * it is given that a user may not pass more than 1 byte
*/
uint8_t* ecat_reg(uint16_t reg)
{
	int addr = reg % 200;

	if (reg  < 200)
		return &ecat_regs1[addr];
	if (reg  < 400)
		return &ecat_regs2[addr];
	if (reg  < 600)
		return &ecat_regs3[addr];
	if (reg  < 800)
		return &ecat_regs4[addr];
	if (reg  < 1000)
		return &ecat_regs5[addr];
	if (reg  < 1200)
		return &ecat_regs6[addr];
	if (reg  < 1400)
		return &ecat_regs7[addr];
	if (reg  < 1600)
		return &ecat_regs8[addr];
	if (reg  < 2000)
		return &ecat_regs9[addr];
	if (reg  < 2200)
		return &ecat_regs10[addr];
	if (reg  < 2400)
		return &ecat_regs11[addr];
	return &ecat_regs12[addr];
}

void ecat_set_reg(uint32_t addr, uint8_t val)
{
	uint8_t *reg = ecat_reg(addr);
	*reg = val;
}

void copy_to_reg(int dest_reg, uint8_t* src, uint8_t len)
{
	int i = 0;

	for (;i < len; i++){
		ecat_set_reg( dest_reg + i , src[i] );
	}
}

void copy_from_reg(uint8_t *dest, int src_reg, uint8_t len)
{
	int i = 0;

	for (;i < len; i++){
		dest[i] = *ecat_reg(src_reg + i);
	}
}

