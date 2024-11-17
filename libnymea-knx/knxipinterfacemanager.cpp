#include "knxipinterfacemanager.h"
#include <stdexcept>

NYMEA_LOGGING_CATEGORY(dcKNXLib, "KNX_Lib")

KNXIPInterfaceManager::KNXIPInterfaceManager(ThingManager *thingManager)
{
    this->thingManager = thingManager;
    this->interfaces = new QMap<ThingId, const ThingLink *>();
    this->devices = new QMap<ThingId, const ThingLink *>();
    qCDebug(dcKNXLib()) << "KNX/IP Interface manager initialized.";
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
    qCDebug(dcKNXLib()) << "KNX/IP Interface manager destroyed.";
}

const ThingLink* KNXIPInterfaceManager::registerInterface(ThingId thingId) const
{
    auto interfaceLink = new ThingLink();
    this->interfaces->insert(thingId, interfaceLink);
    return interfaceLink;
}

const QMap<ThingId, QString> KNXIPInterfaceManager::availableInterfaces() const
{
    QMap<ThingId, QString> result;
    for (const ThingId thingId : this->interfaces->keys())
    {
        qCDebug(dcKNXLib()) << "Querying interface manager for gateway with id: " << thingId.toString();
        if (this->thingManager != nullptr) {
            auto interface = this->thingManager->findConfiguredThing(thingId);
            if (interface == nullptr)
            {
                qCDebug(dcKNXLib()) << "No thing found for interface thing with id" << thingId.toString();
                continue;
            }
            result.insert(thingId, interface->name());
        }
    }
    qCDebug(dcKNXLib()) << result.size() << " available interfaces.";
    return result;
}

const ThingLink* KNXIPInterfaceManager::link(ThingId interfaceId, ThingId clientId) const
{
    
    if (!this->interfaces->contains(interfaceId))
    {
        qCDebug(dcKNXLib()) << "No thing link found for interface thing with id" << interfaceId.toString();
        return nullptr;
    }
    auto interfaceLink = this->interfaces->value(interfaceId);
    auto clientLink = new ThingLink();
    this->devices->insert(clientId, clientLink);
    connect(interfaceLink, &ThingLink::connectedEvent, clientLink, &ThingLink::connected);
    connect(interfaceLink, &ThingLink::disconnectedEvent, clientLink, &ThingLink::disconnected);
    connect(interfaceLink, &ThingLink::frameReceivedEvent, clientLink, &ThingLink::frameReceived);
    connect(clientLink, &ThingLink::sendFrameEvent, interfaceLink, &ThingLink::sendFrame);
    qCInfo(dcKNXLib()) << "Linked thing with id: " << clientId << " to interface thing with id" << interfaceId.toString();
    return clientLink;
}
