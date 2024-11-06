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

#ifndef INTEGRATIONPLUGINMDTGLASTASTER2_H
#define INTEGRATIONPLUGINMDTGLASTASTER2_H

#include "plugintimer.h"
#include <knxipinterfacemanager.h>
#include <integrationknxplugin.h>
#include <loggingcategories.h>

class IntegrationPluginKnxMDTGlastTaster2: public IntegrationKNXPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "io.nymea.knx.IntegrationKNXPlugin" FILE "integrationpluginmdtglastaster2.json")
    Q_INTERFACES(IntegrationKNXPlugin IntegrationPlugin)

public:
    IntegrationPluginKnxMDTGlastTaster2(QObject *parent = nullptr);

    void init() override;

    void discoverThings(ThingDiscoveryInfo *info) override;
    void setupThing(ThingSetupInfo *info) override;

    void postSetupThing(Thing *thing) override;

    void setKNXIPInterfaceManager(const KNXIPInterfaceManager *interfaceManager) override;
private:
    PluginTimer *m_pluginTimer = nullptr;
    const KNXIPInterfaceManager *interfaceManager = nullptr;
    Thing *thing = nullptr;
    const ThingLink *thingLink = nullptr;

private slots:
    void onPluginTimerTimeout();

    void connected();
    void disconnected();
    void frameReceived(const QKnxLinkLayerFrame &frame);

};

#endif // INTEGRATIONPLUGINMDTGLASTASTER2_H
