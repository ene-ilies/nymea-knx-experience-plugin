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

#include <loggingcategories.h>
#include "knxipjsonhandler.h"

Q_DECLARE_LOGGING_CATEGORY(dcKNXIPExperience)

KNXIPJsonHandler::KNXIPJsonHandler(QObject *parent): JsonHandler(parent)
{
    qCDebug(dcKNXIPExperience()) << "KNX/IP JSON handler instance created.";
}

QString KNXIPJsonHandler::name() const
{
    return "KNX/IP Interface";
}
