#ifndef KNXIPINTERFACEMANAGER_H
#define KNXIPINTERFACEMANAGER_H

#include <QLoggingCategory>
#include <QObject>
#include <QMap>
#include <loggingcategories.h>
#include "integrations/integrationplugin.h"
#include "thinglink.h"

class ThingManager;

class KNXIPInterfaceManager: public QObject {
    Q_OBJECT
public:
    KNXIPInterfaceManager(ThingManager* thingManager);
    ~KNXIPInterfaceManager();
    const ThingLink* registerInterface(ThingId interfaceId);
    const QMap<ThingId, QString> availableInterfaces();
    const ThingLink* link(ThingId interfaceId, ThingId clientId);
private:
    ThingManager* thingManager = nullptr;
    QMap<ThingId, const ThingLink*> *interfaces;
    QMap<ThingId, const ThingLink*> *devices;
};

#endif //KNXIPINTERFACEMANAGER_H
