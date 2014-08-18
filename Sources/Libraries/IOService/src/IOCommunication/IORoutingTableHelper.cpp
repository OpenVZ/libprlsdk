/*
 * IORoutingTableHelper.cpp
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


#include "IORoutingTableHelper.h"

using namespace IOService;

/*****************************************************************************/

QMutex IORoutingTableHelper::m_mutex;
QHash<PRL_SECURITY_LEVEL, IORoutingTable*>
   IORoutingTableHelper::m_serverTables;
QHash<PRL_SECURITY_LEVEL, IORoutingTable*>
   IORoutingTableHelper::m_clientTables;

/*****************************************************************************/

const IORoutingTable& IORoutingTableHelper::GetServerRoutingTable (
    PRL_SECURITY_LEVEL security )
{
    QMutexLocker locker( &m_mutex );
    if ( m_serverTables.contains(security) )
        return *m_serverTables[security];

    IORoutingTable* table = 0;

    switch ( security ) {
    case PSL_LOW_SECURITY: {
        table = new IORoutingTable( IORoutingTable::PlainRoute,
                                    IORoutingTable::OptionalRoute,
                                    // Additional route
                                    QList<IORoutingTable::RouteName>() <<
                                        IORoutingTable::SSLRoute );
        break;
    }
    case PSL_NORMAL_SECURITY: {
        table = new IORoutingTable( IORoutingTable::PlainRoute,
                                    IORoutingTable::OptionalRoute );
        bool res = false;
        // IO routes
        res = table->addRoute( PET_IO_CLI_KEYBOARD_SCANCODE,
                               IORoutingTable::SSLRoute,
                               IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        res = table->addRoute( PET_IO_CLI_TOOLS_CLIPBOARD_DATA,
                               IORoutingTable::SSLRoute,
                               IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        res = table->addRoute( PET_IO_CLI_AUTHENTICATE_SESSION,
                               IORoutingTable::SSLRoute,
                               IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        res = table->addRoute( PET_IO_CLI_ATTACH_TO_VM,
                               IORoutingTable::SSLRoute,
                               IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        // Management routes
        res= table->addRoute( PVE::DspClientToVmCommandRangeStart,
                              PVE::DspClientToVmCommandRangeEnd,
                              IORoutingTable::SSLRoute,
                              IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        res= table->addRoute( PVE::DspClientToDispatcherCommandRangeStart,
                              PVE::DspClientToDispatcherCommandRangeEnd,
                              IORoutingTable::SSLRoute,
                              IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        res= table->addRoute( PVE::DspVmToClientCommandRangeStart,
                              PVE::DspVmToClientCommandRangeEnd,
                              IORoutingTable::SSLRoute,
                              IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        res= table->addRoute( PVE::DspCtlCommandRangeStart,
                              PVE::DspCtlCommandRangeEnd,
                              IORoutingTable::SSLRoute,
                              IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        res= table->addRoute( PVE::DspWsCommandRangeStart,
                              PVE::DspWsCommandRangeEnd,
                              IORoutingTable::SSLRoute,
                              IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        res= table->addRoute( PVE::DispToDispRangeStart,
                              PVE::DispToDispRangeEnd,
                              IORoutingTable::SSLRoute,
                              IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
		(void)res;
        break;
    }
    case PSL_HIGH_SECURITY: {
        table = new IORoutingTable( IORoutingTable::SSLRoute,
                                    IORoutingTable::RequiredRoute );
        break;
    }
    default : {
        // Unreachable line
        Q_ASSERT(0);
    }}

    Q_ASSERT(table);
    m_serverTables[security] = table;

    return *table;
}

const IORoutingTable& IORoutingTableHelper::GetClientRoutingTable (
    PRL_SECURITY_LEVEL security )
{
    QMutexLocker locker( &m_mutex );
    if ( m_clientTables.contains(security) )
        return *m_clientTables[security];

    IORoutingTable* table = 0;

    switch ( security ) {
    case PSL_LOW_SECURITY: {
        table = new IORoutingTable( IORoutingTable::PlainRoute,
                                    IORoutingTable::RequiredRoute );
        break;
    }
    case PSL_NORMAL_SECURITY: {
        table = new IORoutingTable( IORoutingTable::PlainRoute,
                                    IORoutingTable::RequiredRoute );
        bool res = false;
        // IO routes
        res = table->addRoute( PET_IO_CLI_KEYBOARD_SCANCODE,
                               IORoutingTable::SSLRoute,
                               IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        res = table->addRoute( PET_IO_CLI_TOOLS_CLIPBOARD_DATA,
                               IORoutingTable::SSLRoute,
                               IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        res = table->addRoute( PET_IO_CLI_AUTHENTICATE_SESSION,
                               IORoutingTable::SSLRoute,
                               IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        res = table->addRoute( PET_IO_CLI_ATTACH_TO_VM,
                               IORoutingTable::SSLRoute,
                               IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        // Management routes
        res= table->addRoute( PVE::DspClientToVmCommandRangeStart,
                              PVE::DspClientToVmCommandRangeEnd,
                              IORoutingTable::SSLRoute,
                              IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        res= table->addRoute( PVE::DspClientToDispatcherCommandRangeStart,
                              PVE::DspClientToDispatcherCommandRangeEnd,
                              IORoutingTable::SSLRoute,
                              IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        res= table->addRoute( PVE::DspVmToClientCommandRangeStart,
                              PVE::DspVmToClientCommandRangeEnd,
                              IORoutingTable::SSLRoute,
                              IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        res= table->addRoute( PVE::DspCtlCommandRangeStart,
                              PVE::DspCtlCommandRangeEnd,
                              IORoutingTable::SSLRoute,
                              IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        res= table->addRoute( PVE::DspWsCommandRangeStart,
                              PVE::DspWsCommandRangeEnd,
                              IORoutingTable::SSLRoute,
                              IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
        res= table->addRoute( PVE::DispToDispRangeStart,
                              PVE::DispToDispRangeEnd,
                              IORoutingTable::SSLRoute,
                              IORoutingTable::RequiredRoute );
        Q_ASSERT(res);
		(void)res;
        break;
    }
    case PSL_HIGH_SECURITY: {
        table = new IORoutingTable( IORoutingTable::SSLRoute,
                                    IORoutingTable::RequiredRoute );
        break;
    }
    default : {
        // Unreachable line
        Q_ASSERT(0);
    }}

    Q_ASSERT(table);
    m_clientTables[security] = table;

    return *table;
}

/*****************************************************************************/
