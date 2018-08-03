# MakeFile to test GPIOs access of an AT91SAM
# Marc Le Douarain, august 2015

CROSS := arm-linux-gnueabi-
#GCC_VER := -4.7

CC := ${CROSS}gcc${GCC_VER}

all: test_at91sam_gpio

OBJS := test_at91sam_gpio.o wiringSam.o

test_at91sam_gpio: ${OBJS}
	${CC} -o $@ ${OBJS}

clean:
	-rm -f core ${OBJS} test_at91sam_gpio

test_at91sam_gpio.o: test_at91sam_gpio.c
	${CC} -c  $< -o $@
wiringSam.o: wiringSam.c
	${CC} -c  $< -o $@

dist: clean
	(cd ..;rm -f wiringSam.tar.gz;tar -cvzf wiringSam.tar.gz wiringSam/*)
