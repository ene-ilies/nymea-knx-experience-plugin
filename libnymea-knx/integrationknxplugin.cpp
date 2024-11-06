#include "integrationknxplugin.h"

IntegrationKNXPlugin::IntegrationKNXPlugin(QObject *parent): IntegrationPlugin(parent) {}

IntegrationKNXPlugin::~IntegrationKNXPlugin() {}

void IntegrationKNXPlugin::setKNXIPInterfaceManager(const KNXIPInterfaceManager *interfaceManager) {
    Q_UNUSED(interfaceManager)
}
