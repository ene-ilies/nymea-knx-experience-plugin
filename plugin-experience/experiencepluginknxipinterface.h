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

#ifndef EXPERIENCEPLUGINKNXIPINTERFACE_H
#define EXPERIENCEPLUGINKNXIPINTERFACE_H

#include <QLoggingCategory>
#include <QMap>
#include <experiences/experienceplugin.h>
#include "integrations/thingmanager.h"
#include "knxipjsonhandler.h"
#include "knxipinterfacemanager.h"
#include <jsonrpc/jsonrpcserver.h>
#include <loggingcategories.h>

Q_DECLARE_LOGGING_CATEGORY(dcKNXIPExperience)

class ExperiencePluginKnxIPInterface: public ExperiencePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "io.nymea.ExperiencePlugin")
    Q_INTERFACES(ExperiencePlugin)

public:
    ExperiencePluginKnxIPInterface();
    void init() override;

private:
    QMap<const ThingId&, IntegrationKNXPlugin*> *thingsMap = nullptr;
    KNXIPInterfaceManager* interfaceManager = nullptr;

    void watchThings();
    void watchThing(Thing *thing);
    void unwatchThing(const ThingId &thingId);

/*public slots:
    void setGatewayAddress(const QAddress &gatewayAddress);*/

};

#endif // EXPERIENCEPLUGINKNXIPINTERFACE_H
