
UTEST = ut-app

SRCS = app-stubs.c \
ut-app.c \
config.c

IFLAGS+=-I. -I.. -Imock/ -I../../../src/  -I../../../src/vm   

# These are the top-level common vars
include ../Makefile.inc

%.o: %.c
	${CC} ${ALL_IFLAGS} ${ALL_CFLAGS} -c $< -o $@

.PHONY :  all clean run

all: ${SRCS} ${UTEST} 

${UTEST}: ${OBJS}

	${CC} -o ${UTEST} ${OBJS} ${ALL_LDFLAGS} ${ALL_LIBS} 


# Clean up the objects and binary in this directory and its children


clean:
	${RM} ${OBJS} ${UTEST} ${CLEAN_OTHERS} *.xml *.bb *.bbg *.gcda *.gcov *.gcno *.sum *.htm

run:	
	./${UTEST} 	


report:	
	../../coverage config.gcda


