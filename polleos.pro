TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    polleos.cpp

DISTFILES += \
    polleos.abi

HEADERS += \
    polleos.hpp \
    polleos.gen.hpp \
    polleos_raw.hpp

INCLUDEPATH += \
    /home/tadas/Programming/sources/eos/build/Debug/sdk/include

