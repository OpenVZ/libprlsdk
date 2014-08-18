LIBTARGET = google-breakpad
PROJ_FILE = $$PWD/src/src.pro
QTCONFIG = core
!include(../../Sources/Build/qmake/staticlib.pri): error(include error)

macx: include($$LIBS_LEVEL/Std/Std.pri)

INCLUDEPATH *= \
	$$PWD/src \
	$$PWD/src/parallels \
	$$PWD/src/google-breakpad/common

linux-*: INCLUDEPATH *= $$PWD/src/client/linux/handler
macx: INCLUDEPATH *= $$PWD/src/client/mac/handler
win32: INCLUDEPATH *= $$PWD/src/client/windows/handler
