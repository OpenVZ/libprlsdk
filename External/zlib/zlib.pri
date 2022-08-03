win32 {
	LIBTARGET = zlib
	PROJ_FILE = $$PWD/zlib.pro
	!include(../../Sources/Build/qmake/staticlib.pri): error(missed include)

	LIBS += -lKernel32
} else {
	# for unix-es use zlib from system
	LIBS += -lz
}
