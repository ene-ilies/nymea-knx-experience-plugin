#ifndef KNXIPINTERFACEMANAGER_H
#define KNXIPINTERFACEMANAGER_H

#include <QLoggingCategory>
#include <QObject>
#include "knxtunnel.h"
#include <loggingcategories.h>

Q_DECLARE_LOGGING_CATEGORY(dcKNXIPExperience)

class KNXIPInterfaceManager: public QObject {
    Q_OBJECT
private:
    KnxTunnel* knxTunnel = nullptr;
public:
    ~KNXIPInterfaceManager();
    void setKNXTunnel(KnxTunnel *knxTunnel);
    KnxTunnel* getKNXTunnel();
};

#endif //KNXIPINTERFACEMANAGER_H