TEMPLATE = lib
TARGET = $$qtLibraryTarget(nymea_experiencepluginknxip)

QT += network

CONFIG += plugin link_pkgconfig
PKGCONFIG += nymea

include(../config.pri)

HEADERS += experiencepluginknxipinterface.h

SOURCES += experiencepluginknxipinterface.cpp

INCLUDEPATH += $$top_srcdir/libnymea-knx
LIBS += -L$$top_builddir/libnymea-knx -lnymea-knx

target.path = $$[QT_INSTALL_LIBS]/nymea/experiences/
INSTALLS += target
