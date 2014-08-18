TEMPLATE = lib
CONFIG += staticlib

include(libtar.pri)

HEADERS += 	lib/libtar.h \
		lib/internal.h \
		listhash/libtar_listhash.h \
		compat/compat.h \
		compat/config.h \

SOURCES += \
		lib/wrapper.c \
		lib/util.c \
		lib/output.c \
		lib/handle.c \
		lib/extract.c \
		lib/encode.c \
		lib/decode.c \
		lib/block.c \
		lib/append.c \
		listhash/libtar_list.c \
		listhash/libtar_hash.c \
		compat/basename.c \
		compat/dirname.c \
		compat/fnmatch.c \
		compat/glob.c \
		compat/strlcpy.c \
		compat/strmode.c \
		compat/strsep.c

win32 {
	HEADERS += 	\
			win32/dirent.h \
			win32/grp.h \
			win32/pwd.h \
			win32/tar.h \
			win32/utime.h \
			win32/sys/param.h \
			win32/win32/types.h

	SOURCES += 	win32/dirent.c
}
