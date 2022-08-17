TEMPLATE	= lib
CONFIG      += staticlib
CONFIG += qt

include(zlib.pri)

HEADERS +=		\
	crc32.h		\
	deflate.h	\
	inffast.h	\
	inffixed.h	\
	inflate.h	\
	inftrees.h	\
	trees.h		\
	zconf.h		\
	zconf.in.h	\
	zlib.h		\
	zutil.h

SOURCES +=		\
	adler32.c	\
	compress.c	\
	crc32.c		\
	deflate.c	\
	gzio.c		\
	infback.c	\
	inffast.c	\
	inflate.c	\
	inftrees.c	\
	minigzip.c	\
	trees.c		\
	uncompr.c	\
	zutil.c
