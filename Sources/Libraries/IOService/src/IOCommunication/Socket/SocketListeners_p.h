/*
 * SocketListeners_p.h
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


#ifndef SOCKETLISTENERSP_H
#define SOCKETLISTENERSP_H

#include "../IOProtocol.h"
#include "../IOSendJob.h"

namespace IOService {

class SocketWriteThread;
class SocketClientPrivate;

// Main write thread callbacks
class SocketWriteListenerInterface
{
public:
    virtual void onBeforeWrite ( SocketWriteThread*,
                                 const SmartPtr<IOPackage>& ) = 0;
    virtual void onAfterWrite ( SocketWriteThread*,
                                IOSendJob::Result,
                                const SmartPtr<IOPackage>& ) = 0;
    virtual void onWriteIsFinishedWithErrors ( SocketWriteThread* ) = 0;

protected:
    // Virtual destructor to avoid compiler warnings
    virtual ~SocketWriteListenerInterface () {}
};

/** Main callbacks for receive/send packages of IO client */
class ReceiveSendClientListenerInterface
{
public:
    //
    // Common callbacks
    //

    virtual void onBeforeSend ( SocketClientPrivate*,
                                const IOSender::Handle&,
                                const SmartPtr<IOPackage>& )
    {}

    virtual void onAfterSend ( SocketClientPrivate*,
                               const IOSender::Handle&,
                               IOSendJob::Result,
                               const SmartPtr<IOPackage>& )
    {}

    virtual void onDetachedClientReceived (
                                   SocketClientPrivate*,
                                   const IOSender::Handle&,
                                   const SmartPtr<IOPackage>&,
                                   const IOCommunication::DetachedClient& )
    {}

    virtual void onPackageReceived ( SocketClientPrivate*,
                                     const IOSender::Handle&,
                                     const SmartPtr<IOPackage>& )
    {}

    virtual void onResponsePackageReceived ( SocketClientPrivate*,
                                             const IOSender::Handle&,
                                             const IOSendJob::Handle&,
                                             const SmartPtr<IOPackage>& )
    {}

    //
    // Server context callbacks
    //
    virtual void srv_onDetachClient (
                                  SocketClientPrivate*,
                                  const IOSender::Handle&,
                                  const IOCommunication::DetachedClient& )
    {}

protected:
    // Virtual destructor to avoid compiler warnings
    virtual ~ReceiveSendClientListenerInterface () {}
};

/** Main callbacks for state change of IO client */
class StateClientListenerInterface
{
public:
    //
    // Common callbacks
    //
    virtual void onStateChanged ( SocketClientPrivate*,
                                  const IOSender::Handle&,
                                  IOSender::State,
                                  IOSender::State )
    {}

    //
    // Server callbacks
    //
    virtual bool srv_validateAfterHandshake ( SocketClientPrivate*,
                                              const IOSender::Handle& )
    {
        return true;
    }

protected:
    // Virtual destructor to avoid compiler warnings
    virtual ~StateClientListenerInterface () {}
};

} //namespace IOService

#endif //SOCKETLISTENERSP_H
