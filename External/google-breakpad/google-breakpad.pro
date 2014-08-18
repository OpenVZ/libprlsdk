
TEMPLATE = subdirs

LEVEL = ./../..
include($$LEVEL/Sources/Parallels.pri)
include($$LEVEL/Sources/Build/Options.pri)

CONFIG += ordered

# Crash lib
SUBDIRS += src/src.pro

# Only for Linux for now
linux-*:SUBDIRS += minidump_parser/minidump_parser.pro

QMAKE_CFLAGS -= -Werror
QMAKE_CXXFLAGS -= -Werror
