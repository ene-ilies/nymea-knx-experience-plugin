#include "knxipinterfacemanager.h"
#include <stdexcept>

NYMEA_LOGGING_CATEGORY(dcKNXIPExperience, "KNXIP_Experience")

KNXIPInterfaceManager::KNXIPInterfaceManager(ThingManager *thingManager)
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
    for (auto thingLink : this->devices->values())
    {
        delete thingLink;
    }
    delete devices;
    qCDebug(dcKNXIPExperience()) << "KNX/IP Interface manager destroyed.";
}

const ThingLink* KNXIPInterfaceManager::registerInterface(ThingId thingId)
{
    auto interfaceLink = new ThingLink();
    this->interfaces->insert(thingId, interfaceLink);
    return interfaceLink;
}

const QMap<ThingId, QString> KNXIPInterfaceManager::availableInterfaces()
{
    QMap<ThingId, QString> result;
    for (const ThingId thingId : this->interfaces->keys())
    {
        auto interface = this->thingManager->findConfiguredThing(thingId);
        if (interface == nullptr)
        {
            qCDebug(dcKNXIPExperience()) << "No thing found for interface thing with id" << thingId.toString();
            continue;
        }
        result.insert(thingId, interface.name());
    }
    return result;
}

const ThingLink* KNXIPInterfaceManager::link(ThingId interfaceId, ThingId clientId)
{
    auto clientLink = new ThingLink();
    auto interfaceLink = this->interfaces->value(interfaceId);
    if (interfaceLink == nullptr)
    {
        qCDebug(dcKNXIPExperience()) << "No thing link found for interface thing with id" << interfaceId.toString();
        throw std::invalid_argument("Unknown interface id:" + interfaceId.toString().toStdString());
    }
    this->devices->insert(clientId, clientLink);
    connect(interfaceLink, &ThingLink::frameReceived, clientLink, &ThingLink::frameReceivedHandler);
    connect(clientLink, &ThingLink::frameReceived, interfaceLink, &ThingLink::frameReceivedHandler);
    qCInfo(dcKNXIPExperience()) << "Linked thing with id: " << clientId << " to interface thing with id" << interfaceId.toString();
    return clientLink;
}
