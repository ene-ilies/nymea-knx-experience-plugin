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
    this->interfaceManager = new KNXIPInterfaceManager(thingManager());
    this->knxPlugins = new QSet<PluginId>();
    qCDebug(dcKNXIPExperience()) << "KNX/IP Experience Plugin instance created.";
}

ExperiencePluginKnxIPInterface::~ExperiencePluginKnxIPInterface()
{
    delete this->interfaceManager;
    delete this->knxPlugins;
    qCDebug(dcKNXIPExperience()) << "KNX/IP Experience Plugin instance destroyed.";
}

void ExperiencePluginKnxIPInterface::init()
{
    qCInfo(dcKNXIPExperience()) << "KNX/IP Experience Plugin initializing.";
    watchPlugins();
    watchThings();
    qCInfo(dcKNXIPExperience()) << "KNX/IP Experience Plugin initialized.";
}

void ExperiencePluginKnxIPInterface::watchPlugins()
{
    qCDebug(dcKNXIPExperience()) << "Register plugins watcher.";
    ThingManager *tm = thingManager();
    for (IntegrationPlugin *plugin : tm->plugins()) {
        IntegrationKNXPlugin *knxPlugin = qobject_cast<IntegrationKNXPlugin *>(plugin);
        if (knxPlugin != nullptr) {
            qCInfo(dcKNXIPExperience()) << "Watch KNX plugin.";
            this->knxPlugins->insert(knxPlugin->pluginId());
            knxPlugin->setKNXIPInterfaceManager(this->interfaceManager);
        }
    }
    qCDebug(dcKNXIPExperience()) << "Plugins watcher registered.";
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
    if (!this->knxPlugins->contains(thing->pluginId())) {
        qCDebug(dcKNXIPExperience()) << "Thing is not a KNX thing. Skip watching it.";
        return;
    }
    qCInfo(dcKNXIPExperience()) << "Watching for new KNX thing: " << thing->id() << "|" << thing->name();
    if (thing->thingClass().interfaces().contains("gateway")) {
        qCInfo(dcKNXIPExperience()) << "New KNX Gateway added.";
    }
}

void ExperiencePluginKnxIPInterface::unwatchThing(const ThingId &thingId)
{
    qCDebug(dcKNXIPExperience()) << "Unwatching deleted thing with id: " << thingId;
}
