#ifndef KNXIPINTERFACEMANAGER_H
#define KNXIPINTERFACEMANAGER_H

#include <QLoggingCategory>
#include <QObject>
#include <QMap>
#include <loggingcategories.h>
#include <integrations/thingmanager.h>
#include <integrations/integrationplugin.h>
#include "thinglink.h"

Q_DECLARE_LOGGING_CATEGORY(dcKNXLib)

class KNXIPInterfaceManager: public QObject {
    Q_OBJECT
public:
    KNXIPInterfaceManager(ThingManager* thingManager);
    ~KNXIPInterfaceManager();
    const ThingLink* registerInterface(ThingId interfaceId) const;
    const QMap<ThingId, QString> availableInterfaces() const;
    const ThingLink* link(ThingId interfaceId, ThingId clientId) const;
private:
    ThingManager* thingManager = nullptr;
    QMap<ThingId, const ThingLink*> *interfaces;
    QMap<ThingId, const ThingLink*> *devices;
};

#endif //KNXIPINTERFACEMANAGER_H
