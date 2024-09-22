/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Copyright (C) 2024                                                     *
 *  Bogdan Ilies <10359749+ene-ilies@users.noreply.github.com>             *
 *                                                                         *
 *  This file is part of nymea.                                            *
 *                                                                         *
 *  This library is free software; you can redistribute it and/or          *
 *  modify it under the terms of the GNU Lesser General Public             *
 *  License as published by the Free Software Foundation; either           *
 *  version 2.1 of the License, or (at your option) any later version.     *
 *                                                                         *
 *  This library is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *  Lesser General Public License for more details.                        *
 *                                                                         *
 *  You should have received a copy of the GNU Lesser General Public       *
 *  License along with this library; If not, see                           *
 *  <http://www.gnu.org/licenses/>.                                        *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "experiencepluginknxipinterface.h"

NYMEA_LOGGING_CATEGORY(dcKNXIPExperience, "KNXIP_Experience")

ExperiencePluginKnxIPInterface::ExperiencePluginKnxIPInterface()
{
    this->thingsMap = new QMap<const ThingId&, const IntegrationKNXPlugin*>();
    this->interfaceManager = new KNXIPInterfaceManager();
    qCDebug(dcKNXIPExperience()) << "KNX/IP Experience Plugin instance created.";
}

void ExperiencePluginKnxIPInterface::init()
{
    qCInfo(dcKNXIPExperience()) << "KNX/IP Experience Plugin initializing.";
    //jsonRpcServer()->registerExperienceHandler(new KNXIPJsonHandler(this), 1, 0);
    watchThings();
    qCInfo(dcKNXIPExperience()) << "KNX/IP Experience Plugin initialized.";
}

void ExperiencePluginKnxIPInterface::watchThings() 
{
    qCDebug(dcKNXIPExperience()) << "Register things watcher.";
    ThingManager *tm = thingManager();
    foreach (Thing *thing, tm->configuredThings()) {
        watchThing(thing);
    }
    connect(tm, &ThingManager::thingAdded, this, &ExperiencePluginKnxIPInterface::watchThing);
    connect(tm, &ThingManager::thingRemoved, this, &ExperiencePluginKnxIPInterface::unwatchThing);
    qCDebug(dcKNXIPExperience()) << "Things watcher registered.";
}

void ExperiencePluginKnxIPInterface::watchThing(Thing *thing)
{
    qCInfo(dcKNXIPExperience()) << "Watching for new thing: " << thing->id().toString() << "|" << thing->name();
    IntegrationKNXPlugin *knxThing = qobject_cast<IntegrationKNXPlugin *>( p );
    if (knxThing != nullptr) {
        qCInfo(dcKNXIPExperience()) << "Watch KNX plugin.";
        this->thingsMap[thing->id()] = knxThing;
        knxThing->setKNXIPInterfaceManager(this->interfaceManager);
        //connect(thing, &Thing::configValueChanged, this, &ExperiencePluginKnxIPInterface::onPluginConfigurationChanged);
    }
}

void ExperiencePluginKnxIPInterface::unwatchThing(const ThingId &thingId)
{
    qCInfo(dcKNXIPExperience()) << "Unwatching deleted thing with id: " << thingId.toString();
    auto itemsRemoved = this->thingsMap.remove(thingId);
    qCDebug(dcKNXIPExperience()) << "Removed: "  << itemsRemoved << "from things managed by this experience.";
}
