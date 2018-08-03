#include <sys/io.h>
#include "plclib.h"
#include "util.h"
#include "plcemu.h"
#include "hardware.h"

void hw_config(const config_t conf)
{
}

int enable_bus() /* Enable bus communication */
{
	int uid;
	uid = getuid(); /* get User id */
	seteuid(0); /* set User Id to root (0) */
	if (geteuid() != 0){
		fprintf(stderr, "FATAL ERROR: UNABLE TO CHANGE TO ROOT\n");
		return -1;
	}
	if (iopl(3)){
/* request bus WR i/o permission */
		fprintf(stderr, "FATAL ERROR: UNABLE TO GET I/O PORT PERMISSION\n");
		perror("iopl() ");
		seteuid(uid);
		return -1;
	}
	seteuid(uid); /* reset User Id */
	outb(0, Base + Wr_offs); //clear outputs port
	printf("io card enabled\n");
	return 1;
}

int disable_bus() /* Disable bus communication */
{
	int uid, i, j, n;
	for (i = 0; i < Dq; i++){	//write zeros
		for (j = 0; j < BYTESIZE; j++){	//zero n bit out
			n = BYTESIZE * i + j;
			dio_write(plc.outputs, n, 0);
		}
	}
	uid = getuid(); /* get User id */
	setuid(0); /* set User Id to root (0) */
	if (getuid() != 0){
		fprintf(stderr, "Unable to change id to root\nExiting\n");
		return -1;
	}
	if (iopl(0)){ /* Normal i/o prevelege level */
		perror("iopl() ");
		setuid(uid);
		return -1;
	}
	setuid(uid); /* reset User Id */
    return 1;
}

int io_fetch()
{
    return 0;
}

int io_flush()
{
    return 0;
}

void dio_read(unsigned int n, BYTE* bit)
{	//write input n to bit
	unsigned int b;
	BYTE i;
	i = inb(Base + Rd_offs + n / BYTESIZE);
	b = (i >> n % BYTESIZE) % 2;
	*bit = (BYTE) b;
}

void dio_write(BYTE * buf, int n, int bit)
{	//write bit to n output
	BYTE q;
	q = buf[n / BYTESIZE];
	q |= bit << n % BYTESIZE;
	outb(q, Base + Wr_offs + n / BYTESIZE);
}

void dio_bitfield(BYTE * write_mask, BYTE * bits)
{	//simultaneusly write output bits defined my mask and read all inputs
    /*FIXME
    int i;
	for (i = 0; i < Dq; i++)
		outb(bits[i] & write_mask[i], Base + Wr_offs + i);
	for (i = 0; i < Di; i++)
        bits[i] = inb(Base + Rd_offs + i);*/
}

void data_read(unsigned int index, uint64_t* value)
{
    return; //unimplemented for user space
}


void data_write(unsigned int index, uint64_t value)
{
    return; //unimplemented for user space
}

