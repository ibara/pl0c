# pl0c Makefile

CC =		cc
CFLAGS =	-g -O2 -DHAVE_STRTONUM

PROG =	pl0c
OBJS =	pl0c.o strtonum.o

NATIVE =	npl0c

all: ${OBJS}
	${CC} ${LDFLAGS} -o ${PROG} ${OBJS}

native: all
	./pl0c pl0c.pl0 | cc -O2 -o ${NATIVE} -x c -

test:
	cd tests && ./test.sh

clean:
	rm -f ${PROG} ${NATIVE} ${OBJS} ${PROG}.core
