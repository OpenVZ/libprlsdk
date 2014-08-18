/*
 * IOSendJobInterface.h
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


#ifndef IOSENDJOBINTERFACE_H
#define IOSENDJOBINTERFACE_H

#include "IOConnection.h"
#include "IOSendJob.h"

namespace IOService {

/**
 * Common interface for send jobs.
 */
class  IOSendJobInterface
{
public:
    /** Default constructor */
    inline IOSendJobInterface () {}

    /**
     * Urgently wake up send waitings.
     * @param h handle of some send job
     *
     * @return IOSendJob::Success - wake up has been completed.
     * @return IOSendJob::InvalidJob - job handle is invalid.
     */
    inline IOSendJob::Result urgentSendWakeUp (
        const IOSendJob::Handle& h )
    {
        if ( ! h.m_job.isValid() )
            return IOSendJob::InvalidJob;

        SmartPtr<IOSendJob> job = h.m_job;
        job->wakeSendWaitings( IOSendJob::UrgentlyWaked );
        return IOSendJob::Success;
    }

    /**
     * Waits for send.
     * @param h handle of some send job
     * @param timeout wait timeout.
     *        If timeout is UINT_MAX (the default),
     *        then the wait will never timeout
     *
     * @return IOSendJob::Success - wait has been completed.
     * @return IOSendJob::Fail - client or server is not started.
     * @return IOSendJob::InvalidJob - job handle is invalid.
     * @return IOSendJob::Timeout - timeout has been occured.
     */
    inline IOSendJob::Result waitForSend ( const IOSendJob::Handle& h,
                                           quint32 timeout = UINT_MAX )
    {
        if ( ! h.m_job.isValid() )
            return IOSendJob::InvalidJob;

        bool urgentlyWaked = false;
        SmartPtr<IOSendJob> job = h.m_job;
        bool res = job->waitForSend( timeout, urgentlyWaked );
        if ( res ) {
            if ( urgentlyWaked )
                return IOSendJob::UrgentlyWaked;
            else
                return IOSendJob::Success;
        }

        return IOSendJob::Timeout;
    }

    /**
     * Urgently wake up response waitings.
     * @param h handle of some send job
     *
     * @return IOSendJob::Success - wake up has been completed.
     * @return IOSendJob::InvalidJob - job handle is invalid.
     */
    inline IOSendJob::Result urgentResponseWakeUp (
        const IOSendJob::Handle& h )
    {
        if ( ! h.m_job.isValid() )
            return IOSendJob::InvalidJob;

        SmartPtr<IOSendJob> job = h.m_job;
        job->wakeResponseWaitings( IOSendJob::UrgentlyWaked,
                                   IOSender::InvalidHandle,
                                   SmartPtr<IOPackage>() );
        return IOSendJob::Success;
    }

    /**
     * Waits for response.
     * If somebody connected to 'onResponsePackageReceived' signal with
     * 'Qt::DirectConnection' param, it is guaranteed, that this connected
     * slot will be called firstly and only then all waiters will be waked up.
     *
     * @param h handle of some send job
     * @param timeout wait timeout.
     *        If timeout is UINT_MAX (the default),
     *        then the wait will never timeout
     *
     * @return IOSendJob::Success - wait has been completed.
     * @return IOSendJob::Fail - client or server is not started.
     * @return IOSendJob::InvalidJob - job handle is invalid.
     * @return IOSendJob::Timeout - timeout has been occured.
     */
    inline IOSendJob::Result waitForResponse (
        const IOSendJob::Handle& h,
        quint32 timeout = UINT_MAX )
    {
        if ( ! h.m_job.isValid() )
            return IOSendJob::InvalidJob;

        bool urgentlyWaked = false;
        SmartPtr<IOSendJob> job = h.m_job;
        bool res = job->waitForResponse( timeout, urgentlyWaked );
        if ( res ) {
            if ( urgentlyWaked )
                return IOSendJob::UrgentlyWaked;
            else
                return IOSendJob::Success;
        }

        return IOSendJob::Timeout;
    }

    /**
     * Returns send result by job handle.
     * @param handle of some send job
     *
     * @return IOSendJob::Success - send has been completed successfully
     * @return IOSendJob::Fail - send has been failed
     * @return IOSendJob::InvalidJob - job handle is wrong
     * @return IOSendJob::InvalidPackage - package is ivalid
     * @return IOSendJob::SendPended - send is pended. use #waitForSend
     * @return IOSendJob::SendQueueIsFull - queue is full.
     *                                      try to send next time
     */
    inline IOSendJob::Result getSendResult (
        const IOSendJob::Handle& h ) const
    {
        if ( ! h.m_job.isValid() )
            return IOSendJob::InvalidJob;

        return h.m_job->getSendResult();
    }

    /**
     * Pick up all gathered responses by job handle.
     * @note  E.g. somebody has sent three responses on some request,
     *       '#takeResponse' will pick up response with three packages
     *       in list, then somebody sent fourth response, next call
     *       of '#takeResponse' will return only list with one fourth
     *       package, i.e. '#takeResponse' returns and cleans all
     *       gathered packages.
     *
     * @param handle of some send job
     *
     * @return response on some event.
     *         Response structure contains response result:
     *
     *      IOSendJob::Success - response has been received successfully
     *      IOSendJob::Fail - send has been failed
     *      IOSendJob::InvalidJob - job handle is wrong
     *      IOSendJob::InvalidPackage - package is invalid
     *      IOSendJob::NoResponse - response has not been received yet
     *
     * @see #clearResponse
     */
    inline IOSendJob::Response takeResponse ( IOSendJob::Handle& h ) const
    {
        if ( ! h.m_job.isValid() )
            return IOSendJob::Response();

        return h.m_job->takeResponse();
    }

    /**
     * Clears all gathered response packages.
     * This method should be called every time in 'onResponsePackageReceived',
     * if we do not use '#takeResponse' call. This will prevent us from
     * response packages accumulation.
     */
    inline bool clearResponse ( IOSendJob::Handle& h ) const
    {
        if ( ! h.m_job.isValid() )
            return false;

        return h.m_job->clearResponse();
    }

    /**
     * Returns number of threads, which are waiting for send results
     */
    inline quint32 getSendWaitingsNumber ( const IOSendJob::Handle& h ) const
    {
        if ( ! h.m_job.isValid() ) {
            return 0;
        }
        return h.m_job->getSendWaitingsNumber();
    }

    /**
     * Returns number of threads, which are waiting for response results
     */
    inline quint32 getResponseWaitingsNumber (
        const IOSendJob::Handle& h ) const
    {
        if ( ! h.m_job.isValid() ) {
            return 0;
        }
        return h.m_job->getResponseWaitingsNumber();
    }

    /**
     * Returns job uuid.
     * If job is not valid, uuid will be null
     */
    static inline const Uuid& getJobUuid ( const IOSendJob::Handle& h )
    {
        if ( ! h.m_job.isValid() ) {
            static Uuid emptyUuid;
            return emptyUuid;
        }

        return h.m_job->getPackageUuid();
    }

private:
    // Disable copy constructor and alignment operator
    Q_DISABLE_COPY(IOSendJobInterface)
};

} //namespace IOService

#endif //IOSENDJOBINTERFACE_H
