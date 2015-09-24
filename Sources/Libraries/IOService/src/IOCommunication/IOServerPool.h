/*
 * IOServerPool.h
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


#pragma once

#include <memory>
#include "IOServerInterface.h"

namespace IOService {

/**
 * This is a pool of servers instances.
 * Pool can inherit only client side of server interface,
 * so some server methods must be used by direct server
 * instance access. Also pool translates all signals from any
 * server instance and try to operate with all their clients.
 */
class IOServerPool : public IOServerInterface_Client
{
    Q_OBJECT
public:
    /** Contructor */
    IOServerPool ();
    /** Destructor */
    virtual ~IOServerPool ();

    /**
     * Add server to internal pool if server is stopped.
     * If so, true will be returned, false otherwise.
     */
    bool addServer ( SmartPtr<IOServerInterface> );

    /**
     * Remove server from internal pool if server is stopped.
     * If so, true will be returned, false otherwise.
     */
    bool removeServer ( SmartPtr<IOServerInterface>);

    /**
     * Returns server pool
     */
    QList< SmartPtr<IOServerInterface> > getServers () const;

    /**
     * Returns server by handle
     */
    SmartPtr<IOServerInterface> getServerByClientHandle ( const IOSender::Handle& h ) const;

public:
    virtual IOSender::Type clientSenderType (
                                          const IOSender::Handle& ) const;

	virtual IOSender::SecurityMode clientSecurityMode(const IOSender::Handle& ) const;

    virtual IOSender::State clientState ( const IOSender::Handle& ) const;

    virtual QString clientHostName ( const IOSender::Handle& ) const;

    virtual bool clientProtocolVersion (
                                 const IOSender::Handle&,
                                 IOCommunication::ProtocolVersion& ) const;

    virtual bool detachClient ( const IOSender::Handle& cliHandle,
                                int specificArg,
                                const SmartPtr<IOPackage>& additionalPkg =
                                    IOPackage::Null,
                                bool detachBothSides = false );

    virtual bool disconnectClient ( const IOSender::Handle& );

    virtual IOSendJob::Handle sendPackage ( const IOSender::Handle&,
                                            const SmartPtr<IOPackage>& );

    virtual IOSendJob::Handle sendDetachedClient (
                                  const IOSender::Handle&,
                                  const IOCommunication::DetachedClient&,
                                  const SmartPtr<IOPackage>& request =
                                      IOPackage::Null );

private slots:
    void slot_onClientStateChanged ( IOServerInterface*,
                                     IOSender::Handle,
                                     IOSender::State );

private:
    class Imp;
    const std::auto_ptr<Imp> m_imp;
};

} // namespace IOService
