TEMPLATE = app

TARGET = alttar
CONFIG += console

LEVEL=../../..

include($$LEVEL/Sources/Parallels.pri)

INCLUDEPATH	+= 	$$LEVEL \
			$$LEVEL/External/zlib \
			$$LEVEL/External/libtar/libtar/lib \
			$$LEVEL/External/libtar/libtar/compat \
			$$LEVEL/External/libtar/libtar/listhash

			
win32{
	INCLUDEPATH	+= 	$$LEVEL/External/libtar/libtar/win32
	HEADERS += 	getopt.h

	SOURCES += 	getopt.c
}

SOURCES += 	\
		libtar.c

LIBS 	+= 	-lzlib -llibtar
