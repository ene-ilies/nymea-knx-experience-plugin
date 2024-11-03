include(../config.pri)

isEmpty(PLUGIN_PRI) {
  exists($$[QT_INSTALL_PREFIX]/include/nymea/plugin.pri) {
    include($$[QT_INSTALL_PREFIX]/include/nymea/plugin.pri)
  } else {
    message("plugin.pri not found. Either install libnymea1-dev or use the PLUGIN_PRI argument to point to it.")
    message("For building this project without nymea installed system-wide, you will want to export those variables in addition:")
    message("PKG_CONFIG_PATH=/path/to/build-nymea/libnymea/pkgconfig/")
    message("CPATH=/path/to/nymea/libnymea/")
    message("LIBRARY_PATH=/path/to/build-nymea/libnymea/")
    message("PATH=/path/to/build-nymea/tools/nymea-plugininfocompiler:$PATH")
    message("LD_LIBRARY_PATH=/path/to/build-nymea/libnymea/")
    error("plugin.pri not found. Cannot continue")
  }
} else {
  message("Using $$PLUGIN_PRI")
  include($$PLUGIN_PRI)
}

QT += network knx
CONFIG += plugin link_pkgconfig
PKGCONFIG += nymea

TARGET = $$qtLibraryTarget(nymea_integrationpluginknxipinterface)

INCLUDEPATH += $$top_srcdir/libnymea-knx
LIBS += -L$$top_builddir/libnymea-knx -lnymea-knx

HEADERS += \
    knxtunnel.h \
    knxserverdiscovery.h \
    integrationpluginknxipinterface.h

SOURCES += \
    knxtunnel.cpp \
    knxserverdiscovery.cpp \
    integrationpluginknxipinterface.cpp

target.path = $$[QT_INSTALL_LIBS]/nymea/plugins/
INSTALLS += target
