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
#include "knxframe.h"

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
    if (info->thingClassId() == dimmerThingClassId) {
        QMap<ThingId, QString> availableInterfaces = this->interfaceManager->availableInterfaces();
        qCDebug(dcMDTGlastaster2()) << "Found " << availableInterfaces.size() << " gateways.";
        for (auto thingId : availableInterfaces.keys()) {
            auto name = availableInterfaces.value(thingId);
            qCDebug(dcMDTGlastaster2()) << "Discovered gateway with id: " << thingId.toString() << " and name: " << name;
            ThingDescriptor descriptor(dimmerThingClassId, info->displayMessage(), name);
            ParamList params;
            params.append(Param(dimmerThingGatewayParamTypeId, thingId));
            params.append(Param(dimmerThingDimmerGroupParamTypeId));
            params.append(Param(dimmerThingSwitchGroupParamTypeId));
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

    if (thing->thingClassId() == dimmerThingClassId) {
        this->thing = thing;
        qCDebug(dcMDTGlastaster2()) << "Getting param";
        ThingId gatewayThingId(this->thing->paramValue(dimmerThingGatewayParamTypeId).value<QString>());
        if (this->interfaceManager != nullptr) {
            qCDebug(dcMDTGlastaster2()) << "Linking to gateway.";
            this->thingLink = this->interfaceManager->link(gatewayThingId, this->thing->id());
            if (this->thingLink != nullptr) {
                qCDebug(dcMDTGlastaster2()) << "Register to gateway events.";
                connect(this->thingLink, &ThingLink::connectedEvent, this, &IntegrationPluginKnxMDTGlastTaster2::connected);
                connect(this->thingLink, &ThingLink::disconnectedEvent, this, &IntegrationPluginKnxMDTGlastTaster2::disconnected);
                connect(this->thingLink, &ThingLink::frameReceivedEvent, this, &IntegrationPluginKnxMDTGlastTaster2::frameReceived);
            }
        }
    }
}

void IntegrationPluginKnxMDTGlastTaster2::executeAction(ThingActionInfo *info)
{
    Thing *thing = info->thing();
    Action action = info->action();
    qCDebug(dcMDTGlastaster2()) << "Executing action for device" << thing->name() << action.actionTypeId().toString() << action.params();

    // Glastaster dimmer
    if (thing->thingClassId() == dimmerThingClassId) {
        // brightness
        if (action.actionTypeId() == dimmerBrightnessActionTypeId) {
            auto groupAddress = QKnxAddress(QKnxAddress::Type::Group, thing->paramValue(dimmerThingDimmerGroupParamTypeId).value<QString>());
            auto value = action.paramValue(dimmerBrightnessActionBrightnessParamTypeId).value<quint8>();
            auto frame = KnxFrame::createPercentageRequest(groupAddress, value);
            if (this->thingLink == nullptr) {
                qCDebug(dcMDTGlastaster2()) << "Thing link is null.";
                info->finish(Thing::ThingErrorItemNotExecutable);
                return;
            }
            this->thingLink->sendFrame(frame);
            info->finish(Thing::ThingErrorNoError);
            return;
        }

        // switch
        if (action.actionTypeId() == dimmerPowerActionTypeId) {
            auto groupAddress = QKnxAddress(QKnxAddress::Type::Group, thing->paramValue(dimmerThingSwitchGroupParamTypeId).value<QString>());
            auto value = action.paramValue(dimmerPowerActionPowerParamTypeId).value<bool>();
            auto state = value? QKnxSwitch::State::On : QKnxSwitch::State::Off;
            auto frame = KnxFrame::createSwitchRequest(groupAddress, state);
            if (this->thingLink == nullptr) {
                qCDebug(dcMDTGlastaster2()) << "Thing link is null.";
                info->finish(Thing::ThingErrorItemNotExecutable);
                return;
            }
            this->thingLink->sendFrame(frame);
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    }

   return info->finish(Thing::ThingErrorNoError);
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
        this->thing->setStateValue(dimmerConnectedStateTypeId, true);
    }
}

void IntegrationPluginKnxMDTGlastTaster2::disconnected()
{
    qCDebug(dcMDTGlastaster2()) << "Assigned gateway disconnected.";
    if (this->thing == nullptr) {
        qCDebug(dcMDTGlastaster2()) << "Thing not properly configured.";
    } else {
        qCDebug(dcMDTGlastaster2()) << "Set state on thing. Disconnected";
        this->thing->setStateValue(dimmerConnectedStateTypeId, false);
    }
}

void IntegrationPluginKnxMDTGlastTaster2::frameReceived(const QKnxLinkLayerFrame &frame)
{
    Q_UNUSED(frame);
    qCDebug(dcMDTGlastaster2()) << "new frame received";   
}
