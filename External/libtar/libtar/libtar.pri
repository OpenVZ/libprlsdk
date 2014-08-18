LIBTARGET = libtar
PROJ_FILE = $$PWD/libtar.pro
!include(../../../Sources/Build/qmake/staticlib.pri): error(missed include)

INCLUDEPATH *= \
	$$PWD/lib \
	$$PWD/compat \
	$$PWD/listhash
win32 {
	INCLUDEPATH *= $$PWD/win32
}

mobilePlatform {
INCLUDEPATH += $$PWD/lib/tar
}

win32: include($$EXT_LEVEL/zlib/zlib.pri)
