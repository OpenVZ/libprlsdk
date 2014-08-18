
TARGET = prl_minidump_parser

LEVEL = ./../../..
include($$LEVEL/Sources/Parallels.pri)
include($$PWD/../google-breakpad.pri)

TEMPLATE = app
CONFIG += warn_on console
QT -= core gui network xml

#
# Only for Linux now
#

linux-*:SOURCES += \
           ../src/processor/minidump_stackwalk.cc
