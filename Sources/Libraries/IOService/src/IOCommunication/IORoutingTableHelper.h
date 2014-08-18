/*
 * IORoutingTableHelper.h
 *
 * Copyright (C) 1999-2014 Parallels IP Holdings GmbH
 *
 * This file is part of Parallels SDK. Parallels SDK is free
 * software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/> or write to Free Software Foundation,
 * 51 Franklin Street, Fifth Floor Boston, MA 02110, USA.
 *
 * Our contact details: Parallels IP Holdings GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#ifndef IOROUTINGTABLEHELPER_H
#define IOROUTINGTABLEHELPER_H

#include <QMutex>

#include "IORoutingTable.h"
#include "Interfaces/ParallelsNamespace.h"

namespace IOService {

class IORoutingTableHelper
{
public:
    static const char* SSLName;
    static const char* PlainName;

    static const IORoutingTable& GetServerRoutingTable ( PRL_SECURITY_LEVEL );
    static const IORoutingTable& GetClientRoutingTable ( PRL_SECURITY_LEVEL );

private:
    static QMutex m_mutex;
    static QHash<PRL_SECURITY_LEVEL, IORoutingTable*> m_serverTables;
    static QHash<PRL_SECURITY_LEVEL, IORoutingTable*> m_clientTables;
};

} //namespace IOService

#endif //IOROUTINGTABLEHELPER_H
