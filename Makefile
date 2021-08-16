# pl0c Makefile

CC =		cc
CFLAGS =	-g -O2 -DHAVE_STRTONUM

PROG =	pl0c
OBJS =	pl0c.o strtonum.o

all: ${OBJS}
	${CC} ${LDFLAGS} -o ${PROG} ${OBJS}

test:
	cd tests && ./test.sh

clean:
	rm -f ${PROG} ${OBJS} ${PROG}.core
