# $OpenBSD$

COMMENT =	simple self-hosting PL/0 compiler
DISTNAME =	pl0c-1.0.1
CATEGORIES =	lang

# ISC
PERMIT_PACKAGE =	Yes

MASTER_SITES =	https://github.com/ibara/pl0c/releases/download/${DISTNAME}/

WANTLIB += c

MAKE_FLAGS =	CC="${CC}" CFLAGS="${CFLAGS} -DHAVE_STRTONUM"

.include <bsd.port.mk>
