#include "knxipinterfacemanager.h"
#include <stdexcept>

NYMEA_LOGGING_CATEGORY(dcKNXIPExperience, "KNXIP_Experience")

KNXIPInterfaceManager::KNXIPInterfaceManager(const ThingManager *thingManager)
{
    this->thingManager = thingManager;
    this->interfaces = new QMap<ThingId, const ThingLink *>();
    this->devices = new QMap<ThingId, const ThingLink *>();
    qCDebug(dcKNXIPExperience()) << "KNX/IP Interface manager initialized.";
}

KNXIPInterfaceManager::~KNXIPInterfaceManager()
{
    for (auto thingLink : this->interfaces->values())
    {
        delete thingLink;
    }
    delete interfaces;
    for (auto thingLink : this->devices.values())
    {
        delete thingLink;
    }
    delete devices;
    delete thingManager;
    qCDebug(dcKNXIPExperience()) << "KNX/IP Interface manager destroyed.";
}

void KNXIPInterfaceManager::registerInterface(ThingId thingId)
{
    this->interfaces->insert(thingId, new ThingLink());
}

const QMap<ThingId, QString> availableInterfaces()
{
    QMap<ThingId, QString> result();
    for (auto thingId : this->interfaces->keys())
    {
        auto interface = this->thingManager->findConfiguredThing(thingId);
        if (interface == nullptr)
        {
            qCDebug(dcKNXIPExperience()) << "No thing found for interface thing with id" << thingId;
            continue;
        }
        result.insert(thingId, interface.name());
    }
    return result;
}

const ThingLink *KNXIPInterfaceManager::link(ThingId interfaceId, ThingId clientId)
{
    auto clientLink = new ThingLink();
    auto interfaceLink = this->interfaces->value(interfaceId);
    if (interfaceLink == nullptr)
    {
        qCDebug(dcKNXIPExperience()) << "No thing link found for interface thing with id" << interfaceId;
        throw new std::invalid_argument("Unknown interface id:" + thingId);
    }
    this->devices->insert(clientId, clientLink);
    connect(interfaceLink, &ThingLink::frameReceived, clientLink, &ThingLink::frameReceivedHandler);
    connect(clientLink, &ThingLink::frameReceived, interfaceLink, &ThingLink::frameReceivedHandler);
    qCInfo(dcKNXIPExperience()) << "Linked thing with id: " << clientId << " to interface thing with id" << interfaceId;
}
