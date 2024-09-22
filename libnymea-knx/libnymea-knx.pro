# Note: make external qtknx linking
exists(/opt/qtknx/build/mkspecs/modules/qt_lib_knx.pri) {
    include(/opt/qtknx/build/mkspecs/modules/qt_lib_knx.pri)
}

include(../config.pri)

QT += network knx

TEMPLATE = lib
TARGET = $$qtLibraryTarget(nymea-knx)

include(../config.pri)

NYMEA_KNX_VERSION_STRING = "0.0.1"

CONFIG += link_pkgconfig
PKGCONFIG += nymea

HEADERS += \
    knxipjsonhandler.h \
    knxtunnel.h \
    knxserverdiscovery.h \
    integrationknxplugin.h \
    knxipinterfacemanager.h 

SOURCES += \
    knxipjsonhandler.cpp \
    knxtunnel.cpp \
    knxserverdiscovery.cpp \
    knxipinterfacemanager.cpp

target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

for(header, HEADERS) {
    path = $$[QT_INSTALL_PREFIX]/include/nymea-knx/$${dirname(header)}
    eval(headers_$${path}.files += $${header})
    eval(headers_$${path}.path = $${path})
    eval(INSTALLS *= headers_$${path})
}

CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_NAME = libnymea-knx
QMAKE_PKGCONFIG_DESCRIPTION = nymea knx library
QMAKE_PKGCONFIG_PREFIX = $$[QT_INSTALL_PREFIX]
QMAKE_PKGCONFIG_INCDIR = $$[QT_INSTALL_PREFIX]/include/nymea-knx/
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_VERSION = $$NYMEA_KNX_VERSION_STRING
QMAKE_PKGCONFIG_FILE = nymea-knx
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
