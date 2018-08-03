#include "plcemu.h"
static comedi_t * it;
int enable_bus() /* Enable bus communication */
{
	int uid;
	char filestr[256];

	if (!USE_COMEDI)
	{

		uid = getuid(); /* get User id */
		seteuid(0); /* set User Id to root (0) */
		if (geteuid() != 0)
		{
			printf("FATAL ERROR: UNABLE TO CHANGE TO ROOT\n");
			return -1;
		}
		if (iopl(3)) /* request bus WR i/o permission */
		{
			printf("FATAL ERROR: UNABLE TO GET I/O PORT PERMISSION\n");
			perror("iopl() ");
			seteuid(uid);
			return -1;
		}
		seteuid(uid); /* reset User Id */
		outb(0, BASE + WR_OFFS); //clear outputs port
	}
	else
	{
		memset(filestr, 0, 256);
		sprintf(filestr, "/dev/comedi%d", COMEDI_FILE);
		printf(filestr);
		if ((it = comedi_open(filestr)) == NULL )
			return -1;
	}
	printf("io card enabled\n");
	return 1;
}

int disable_bus() /* Disable bus communication */
{
	int uid;

	if (!USE_COMEDI)
	{

		uid = getuid(); /* get User id */
		setuid(0); /* set User Id to root (0) */
		if (getuid() != 0)
		{
			fprintf(stderr, "Unable to change id to root\nExiting\n");
			return -1;
		}
		if (iopl(0)) /* Normal i/o prevelege level */
		{
			perror("iopl() ");
			setuid(uid);
			return -1;
		}
		setuid(uid); /* reset User Id */
	}
	else
		comedi_close(it);
}
void dio_read(int n, BYTE* bit)
{ //write input n to bit
	unsigned int b;
	if (!USE_COMEDI)
	{
		BYTE i;
		i = inb(BASE + RD_OFFS + n / 8);
		*bit = (i >> n % 8) % 2;
	}
	else
	{
		comedi_dio_read(it, COMEDI_SUBDEV_I, n, &b);
		*bit = (BYTE) b;
	}
}
void dio_write(BYTE * buf, int n, int bit)
{ //write bit to n output
	BYTE q;
	q = buf[n / 8];
	q |= bit << n % 8;
	if (!USE_COMEDI)
		outb(q, BASE + WR_OFFS + n / 8);
	else
		comedi_dio_write(it, COMEDI_SUBDEV_Q, n, bit);
}
void dio_bitfield(BYTE * write_mask, BYTE * bits)
{ //simultaneusly write output bits defined my mask and read all inputs
	int i, j, n, in;
	unsigned int w, b;
	if (!USE_COMEDI)
	{
        for (i = 0; i < BOOL_DQ; i++)
			outb(bits[i] & write_mask[i], BASE + WR_OFFS + i);
		for (i = 0; i < BOOL_DI; i++)
			bits[i] = inb(BASE + RD_OFFS + i);
	}
	else
	{
		w = (unsigned int) (*write_mask);
		b = (unsigned int) (*bits);
		comedi_dio_bitfield(it, COMEDI_SUBDEV_I, w, &b);
	}
}
main()
{
	unsigned char buf[256];
	int i;
	BYTE b;
	enable_bus();
	memset(buf, 0, 256);
	for (;;)
	{
		printf("INPUTS:\n");
		memset(buf, 48, 8 * BOOL_DI);
		buf[8 * BOOL_DI] = 0;
		for (i = 0; i < 8 * BOOL_DI; i++)
		{
			dio_read(i, &b);
			buf[i] = b ? 49 : 48;
		}
		printf("%s\n", buf);
		memset(buf, 0, 256);
		if (fgets(buf, 255, stdin) == NULL )
			buf[0] = 0;
		printf("OUTPUTS:\n");
        for (i = 0; i < 8 * BOOL_DQ; i++)
		{
			b = (buf[i / 8] >> i % 8) % 2;
			dio_write(buf, i, b);
		}
	}
	disable_bus();
}
