#include "knxipinterfacemanager.h"

NYMEA_LOGGING_CATEGORY(dcKNXIPExperience, "KNXIP_Interface_Manager")

KNXIPInterfaceManager::~KNXIPInterfaceManager() {
    qCDebug(dcKNXIPExperience()) << "KNX/IP Interface manager destroyed.";
    delete this->knxTunnel;
}

void KNXIPInterfaceManager::setKNXTunnel(KnxTunnel *knxTunnel) {
    qCDebug(dcKNXIPExperience()) << "Setting KNX/IP tunnel.";
    this->knxTunnel = knxTunnel;
}

KnxTunnel* KNXIPInterfaceManager::getKNXTunnel() {
    return this->knxTunnel;
}
