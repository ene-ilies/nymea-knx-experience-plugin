#ifndef INTEGRATIONKNXPLUGIN_H
#define INTEGRATIONKNXPLUGIN_H

#include <QObject>
#include "knxipinterfacemanager.h"
#include "integrations/integrationplugin.h"

class LIBNYMEA_EXPORT IntegrationKNXPlugin: public IntegrationPlugin {
    Q_OBJECT
protected:
    virtual void setKNXIPInterfaceManager(const KNXIPInterfaceManager *interfaceManager) = 0;
};

Q_DECLARE_INTERFACE(IntegrationKNXPlugin, "io.nymea.knx.IntegrationKNXPlugin")
Q_DECLARE_METATYPE(IntegrationKNXPlugin*)

#endif // INTEGRATIONKNXPLUGIN_H