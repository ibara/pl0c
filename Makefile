# pl0c Makefile

CC =		cc
CFLAGS =	-g -O2 -DHAVE_STRTONUM

PROG =	pl0c
OBJS =	pl0c.o strtonum.o

NATIVE =	npl0c

all: ${OBJS}
	${CC} ${LDFLAGS} -o ${PROG} ${OBJS}

native: all
	./${PROG} pl0c.pl0 | ${CC} ${CFLAGS} -o ${NATIVE} -x c -

test:
	cd tests && ./test.sh

clean:
	rm -f ${PROG} ${NATIVE} ${OBJS} ${PROG}.core
