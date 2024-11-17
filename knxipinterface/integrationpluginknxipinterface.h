/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
*
* GNU Lesser General Public License Usage
* This project may be redistributed and/or modified under the
* terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; version 3. This project is distributed in the hope that
* it will be useful, but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this project. If not, see <https://www.gnu.org/licenses/>.
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef INTEGRATIONPLUGINKNXIPINTERFACE_H
#define INTEGRATIONPLUGINKNXIPINTERFACE_H

#include <knxipinterfacemanager.h>
#include <integrationknxplugin.h>
#include <loggingcategories.h>
#include "plugintimer.h"
#include "knxtunnel.h"
#include "knxserverdiscovery.h"
#include "thinglink.h"

class IntegrationPluginKnxIPInterface: public IntegrationKNXPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "io.nymea.knx.IntegrationKNXPlugin" FILE "integrationpluginknxipinterface.json")
    Q_INTERFACES(IntegrationKNXPlugin IntegrationPlugin)

public:
    IntegrationPluginKnxIPInterface(QObject *parent = nullptr);

    void init() override;
    void startMonitoringAutoThings() override;

    void discoverThings(ThingDiscoveryInfo *info) override;
    void setupThing(ThingSetupInfo *info) override;

    void postSetupThing(Thing *thing) override;
    void thingRemoved(Thing *thing) override;

    void executeAction(ThingActionInfo *info) override;

    void setKNXIPInterfaceManager(const KNXIPInterfaceManager *interfaceManager) override;
private:
    PluginTimer *m_pluginTimer = nullptr;

    KnxServerDiscovery *m_discovery = nullptr;
    const KNXIPInterfaceManager *interfaceManager = nullptr;
    KnxTunnel *tunnel = nullptr;
    Thing *knxInterface = nullptr;
    const ThingLink *thingLink = nullptr;

    void autoCreateKnownDevices(Thing *parentThing);

    void createGenericDevices(Thing *parentThing);
    void destroyGenericDevices(Thing *parentThing);

private slots:
    void onPluginTimerTimeout();
    void onPluginConfigurationChanged(const ParamTypeId &paramTypeId, const QVariant &value);

    void onTunnelConnectedChanged();
    void onTunnelFrameReceived(const QKnxLinkLayerFrame &frame);
    void onThingLinkSendFrame(const QKnxLinkLayerFrame &frame);
};

#endif // INTEGRATIONPLUGINKNXIPINTERFACE_H
