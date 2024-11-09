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

#include "integrationpluginmdtglastaster2.h"
#include "plugininfo.h"

#include <QObject>

IntegrationPluginKnxMDTGlastTaster2::IntegrationPluginKnxMDTGlastTaster2(QObject *parent):IntegrationKNXPlugin(parent)
{
    qCDebug(dcMDTGlastaster2()) << "MDT Glastaster II instantiated.";
}

void IntegrationPluginKnxMDTGlastTaster2::setKNXIPInterfaceManager(const KNXIPInterfaceManager *interfaceManager) {
    qCDebug(dcMDTGlastaster2()) << "Set KNX/IP interface manager.";
    this->interfaceManager = interfaceManager;
}

void IntegrationPluginKnxMDTGlastTaster2::init()
{
    qCDebug(dcMDTGlastaster2()) << "MDT Glastaster II initializing.";
    qCDebug(dcMDTGlastaster2()) << "MDT Glastaster II initialized.";
}

void IntegrationPluginKnxMDTGlastTaster2::discoverThings(ThingDiscoveryInfo *info)
{
    qCDebug(dcMDTGlastaster2()) << "Discovering things...";
    if (info->thingClassId() == Glastaster2ThingClassId) {
        QMap<ThingId, QString> availableInterfaces = this->interfaceManager->availableInterfaces();
        qCDebug(dcMDTGlastaster2()) << "Found " << availableInterfaces.size() << " gateways.";
        for (auto thingId : availableInterfaces.keys()) {
            auto name = availableInterfaces.value(thingId);
            qCDebug(dcMDTGlastaster2()) << "Discovered gateway with id: " << thingId.toString() << " and name: " << name;
            ThingDescriptor descriptor(Glastaster2ThingClassId, name, "KNXIP Gateway");
            ParamList params;
            params.append(Param(Glastaster2ThingGatewayParamTypeId, thingId));
            descriptor.setParams(params);
            info->addThingDescriptor(descriptor);
        }
        info->finish(Thing::ThingErrorNoError);
    }
}

void IntegrationPluginKnxMDTGlastTaster2::setupThing(ThingSetupInfo *info)
{
    qCDebug(dcMDTGlastaster2()) << "Setup device" << info->thing()->name() << info->thing()->params();

    if (!m_pluginTimer) {
        m_pluginTimer = hardwareManager()->pluginTimerManager()->registerTimer(300);
        connect(m_pluginTimer, &PluginTimer::timeout, this, &IntegrationPluginKnxMDTGlastTaster2::onPluginTimerTimeout);
    }

    info->finish(Thing::ThingErrorNoError);
}

void IntegrationPluginKnxMDTGlastTaster2::postSetupThing(Thing *thing)
{
    qCDebug(dcMDTGlastaster2()) << "Post setup device" << thing->name() << thing->params();

    if (thing->thingClassId() == Glastaster2ThingClassId) {
        this->thing = thing;
        qCDebug(dcMDTGlastaster2()) << "Getting param";
        ThingId gatewayThingId(this->thing->paramValue(Glastaster2ThingGatewayParamTypeId).value<QString>());
        if (this->interfaceManager != nullptr) {
            qCDebug(dcMDTGlastaster2()) << "Linking to gateway.";
            this->thingLink = this->interfaceManager->link(gatewayThingId, this->thing->id());
            if (this->thingLink != nullptr) {
                qCDebug(dcMDTGlastaster2()) << "Linking to gateway.";
                connect(this->thingLink, &ThingLink::connectedEvent, this, &IntegrationPluginKnxMDTGlastTaster2::connected);
                connect(this->thingLink, &ThingLink::disconnectedEvent, this, &IntegrationPluginKnxMDTGlastTaster2::disconnected);
                connect(this->thingLink, &ThingLink::frameReceivedEvent, this, &IntegrationPluginKnxMDTGlastTaster2::frameReceived);
            }
        }
    }
}

void IntegrationPluginKnxMDTGlastTaster2::onPluginTimerTimeout()
{
    qCDebug(dcMDTGlastaster2()) << "Refresh sensor data from KNX devices";
}

void IntegrationPluginKnxMDTGlastTaster2::connected()
{
    qCDebug(dcMDTGlastaster2()) << "Assigned gateway connected.";
    if (this->thing == nullptr) {
        qCDebug(dcMDTGlastaster2()) << "Thing not properly configured.";
    } else {
        qCDebug(dcMDTGlastaster2()) << "Set state on thing. Connected";
        this->thing->setStateValue(Glastaster2ConnectedStateTypeId, true);
    }
}

void IntegrationPluginKnxMDTGlastTaster2::disconnected()
{
    qCDebug(dcMDTGlastaster2()) << "Assigned gateway disconnected.";
    if (this->thing == nullptr) {
        qCDebug(dcMDTGlastaster2()) << "Thing not properly configured.";
    } else {
        qCDebug(dcMDTGlastaster2()) << "Set state on thing. Disconnected";
        this->thing->setStateValue(Glastaster2ConnectedStateTypeId, false);
    }
}

void IntegrationPluginKnxMDTGlastTaster2::frameReceived(const QKnxLinkLayerFrame &frame)
{
    Q_UNUSED(frame);
    qCDebug(dcMDTGlastaster2()) << "new frame received";   
}
