#include <stdlib.h>
#include <varargs.h>
#include <limits.h>
#include "eic.h"
#include "termios.h"


static val_t eic_cfsetospeed(void)
{
	val_t v;

	v.ival = cfsetospeed(arg(0,getargs(),ptr_t).p,
		arg(1,getargs(),unsigned));

	return v;
}

static val_t eic_cfsetispeed(void)
{
	val_t v;

	v.ival = cfsetispeed(arg(0,getargs(),ptr_t).p,
		arg(1,getargs(),unsigned));

	return v;
}

static val_t eic_tcflow(void)
{
	val_t v;

	v.ival = tcflow(arg(0,getargs(),int),
		arg(1,getargs(),int));

	return v;
}

static val_t eic_tcsendbreak(void)
{
	val_t v;

	v.ival = tcsendbreak(arg(0,getargs(),int),
		arg(1,getargs(),int));

	return v;
}

static val_t eic_tcflush(void)
{
	val_t v;

	v.ival = tcflush(arg(0,getargs(),int),
		arg(1,getargs(),int));

	return v;
}

static val_t eic_cfgetospeed(void)
{
	val_t v;

	v.uival = cfgetospeed(arg(0,getargs(),ptr_t).p);

	return v;
}

static val_t eic_cfgetispeed(void)
{
	val_t v;

	v.uival = cfgetispeed(arg(0,getargs(),ptr_t).p);

	return v;
}

static val_t eic_tcdrain(void)
{
	val_t v;

	v.ival = tcdrain(arg(0,getargs(),int));

	return v;
}

static val_t eic_tcsetattr(void)
{
	val_t v;

	v.ival = tcsetattr(arg(0,getargs(),int),
		arg(1,getargs(),int),
		arg(2,getargs(),ptr_t).p);

	return v;
}


static val_t eic_tcgetattr(void)
{
	val_t v;

	v.ival = tcgetattr(arg(0,getargs(),int),
		arg(1,getargs(),ptr_t).p);

	return v;
}

/**********************************/

void module_termios()
{
	EiC_add_builtinfunc("cfsetospeed",eic_cfsetospeed);
	EiC_add_builtinfunc("cfsetispeed",eic_cfsetispeed);
	EiC_add_builtinfunc("tcflow",eic_tcflow);
	EiC_add_builtinfunc("tcsendbreak",eic_tcsendbreak);

	EiC_add_builtinfunc("tcsetattr",eic_tcsetattr);
	EiC_add_builtinfunc("tcgetattr",eic_tcgetattr);


	EiC_add_builtinfunc("tcflush",eic_tcflush);
	EiC_add_builtinfunc("cfgetospeed",eic_cfgetospeed);
	EiC_add_builtinfunc("cfgetispeed",eic_cfgetispeed);
	EiC_add_builtinfunc("tcdrain",eic_tcdrain);
}

/**********************************/

