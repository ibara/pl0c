# pl0c Makefile

CC =		cc
CFLAGS =	-g -O2 -DHAVE_STRTONUM

PREFIX =	/usr/local
MANDIR =	/usr/local/man

PROG =	pl0c
OBJS =	pl0c.o

BOOTSTRAP =	bootstrap
STAGE2 =	stage2
STAGE3 =	stage3

all: final

bootstrap: ${OBJS}
	${CC} ${LDFLAGS} -o ${BOOTSTRAP} ${OBJS}

stage2: bootstrap
	./${BOOTSTRAP} pl0c.pl0 | ${CC} ${CFLAGS} -o ${STAGE2} -x c -

stage3: stage2
	./${STAGE2} < pl0c.pl0 | ${CC} ${CFLAGS} -o ${STAGE3} -x c -

stage4: stage3
	./${STAGE3} < pl0c.pl0 | ${CC} ${CFLAGS} -o ${PROG} -x c -

final: stage4
	/usr/bin/cmp -s ${STAGE3} ${PROG}

install:
	install -d ${PREFIX}/bin
	install -d ${MANDIR}/man1
	install -c -m 755 ${PROG} ${PREFIX}/bin
	install -c -m 444 ${PROG}.1 ${MANDIR}/man1

test:
	cd tests && ./test.sh

clean:
	rm -f ${PROG} ${BOOTSTRAP} ${STAGE2} ${STAGE3} ${OBJS} *.core
