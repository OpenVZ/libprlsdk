/*
 * PrlHandleVm.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019 Virtuozzo International GmbH. All rights reserved.
 *
 * This file is part of Virtuozzo SDK. Virtuozzo SDK is free
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
 * Our contact details: Virtuozzo International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.virtuozzo.com/.
 */


#ifndef __VIRTUOZZO_HANDLE_VM_H__
#define __VIRTUOZZO_HANDLE_VM_H__

#include <QList>
#include <map>
#include <QPair>
#include <QMutex>
#include <QReadWriteLock>
#include <QHash>

#include <prlcommon/IOService/Common/ExecChannel.h>
#include <prlxmlmodel/VmConfig/CVmConfiguration.h>

#include "BuiltinEventSource.h"
#include "PrlHandleBase.h"
#include "PrlHandleHandlesList.h"
#include "PrlHandleIOJob.h"
#include "PrlHandleServerDisp.h"
#include "PrlHandleAccessRights.h"
#include "HandleInterface/IHandleVm.h"

#include "PrlHandleIORemoteCommand.h"
#include "PrlHandleServerStat.h"
#include "PrlHandleServerVm.h"

#define SYNCHRO_PARENT_VM_CONFIG QMutexLocker _lock(m_pVm->GetSynchroObject());

using namespace IOService;

class CResult;
class CVmEvent;

namespace IODisplay
{
struct Unit
{
	Unit( const SmartPtr<IOService::ExecChannel>& channel );

	bool IsConnected () const;

private:
	SmartPtr<IOService::ExecChannel> m_ioExecChannel;
};

struct Connection
{
	Connection ( const Unit& display, const PrlHandleIOJob* ioJobPtr );

	PRL_RESULT Attach ( PrlHandleJob **const ioJobPtr );

	bool IsAttached ();

	void Detach ();

	void SetState ( IOService::Channel::State state );

private:
	QMutex m_mutex;

	std::vector<Unit> m_attachments;
	PrlHandleSmartPtr<PrlHandleIOJob> m_ioJob;
};

} // namespace IODisplay

/**
 * Virtual machine handle.
 */
class PrlHandleVm : public QObject, public PrlHandleBase,
					public IHandleVm
{
	Q_OBJECT
public:
	/**
	 * Class constructor.
	 */
	PrlHandleVm ( const PrlHandleServerPtr &pServer );

	/**
	 * Class destructor.
	 */
	~PrlHandleVm ();

	/**
	 * Returns EventSource object.
	 */
	BuiltinEventSource *eventSource() { return &m_eventSource; }

	/**
	 * Connects to Vm desktop.
	 */
	PRL_RESULT VmConnect ( PrlHandleJob **const pJob );

	/**
	 * Disconnects from Vm desktop gracefully.
	 */
	PRL_RESULT VmDisconnect ();

	/**
	 * Force disconnect from Vm desktop.
	 */
	PRL_RESULT VmDisconnectForcibly ();

	IOService::Channel* GetIOChannel ();
	SmartPtr<IOService::ExecChannel> GetChannel ();

	/**
	 * Returns pointer to internal VM config data synchronization object
	 */
	inline QRecursiveMutex *GetSynchroObject() {return (&m_HandleMutex);}

	/**
	 * Sets VM server
	 */
	void SetServer(const PrlHandleServerPtr &pServer);

	/**
	 * Returns server
	 */
	inline const PrlHandleServerPtr GetServer() const {return (m_pServer);}
	inline const PrlHandleServerDispPtr GetServerDisp() const {return (m_pServerDisp);}

	/**
	 * Called in separate thread on arrival PVE::CVmEventType from VM-side
	 */
	void ProcessVmEvent( PrlHandleBasePtr pEvent );

	/**
	 * Returns registered IO jobs
	 */
	QList< PrlHandleSmartPtr<PrlHandleIOJob> > GetRegisteredIOJobs ();

	/**
	 * Creates event object bint to current VM.
	 * @param buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate event object
	 * * PRL_ERR_SUCCESS 			- operation completed successfully
	 */
	PRL_RESULT CreateEvent(PRL_HANDLE_PTR phEvent);

	/**
	 * Returns VM uuid
	 */
	QString GetUuid();

private slots:

    void IOStateChanged ( IOService::Channel::State );

    void IOPackageReceived ( const SmartPtr<IOPackage> );
	void IOResponsePackageReceived( IOSendJob::Handle hSendJob, const SmartPtr<IOPackage> p );

protected:

	PrlHandleServerPtr m_pServer;
	PrlHandleServerDispPtr m_pServerDisp;
	SmartPtr<IOService::ExecChannel> m_ioExecChannel;

private:

	BuiltinEventSource m_eventSource;

	QReadWriteLock m_ioJobsListRWLock;
	QList< PrlHandleSmartPtr<PrlHandleIOJob> > m_ioJobsList;

public:

	inline const PrlHandleServerVmPtr GetServerVm() const {return (m_pServerVm);}

	/**
	 * Returns VM configuration
	 */
	inline CVmConfiguration &GetVmConfig() {return (m_VmConfig);}

	/**
	 * Read object representation from string
	 */
	PRL_RESULT fromString(PRL_CONST_STR vm_config);
    
	/**
	 * Returns XML as string
	 */
	QString toString();

	/**
	 * Update Vm Configuration
	 */
	PRL_RESULT UpdateVmConfig(const QString &sNewConfig);

	/**
	 * Sets new VM UUID
	 * @param [in] new VM UUID string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetUuid(PRL_CONST_STR sNewVmUuid);

	/// Called when start operation is completed (either with success or failure)
	void StartComplete( CResult* pResult );

	/**
	 * Generates answer event object on some VM question.
	 * @param buffer for storing result
	 * @param answer code
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate event object
	 * * PRL_ERR_SUCCESS 			- operation completed successfully
	 */
	PRL_RESULT CreateAnswerEvent(PRL_HANDLE_PTR phEvent, PRL_RESULT nAnswer);

private:

    void NotifyControlValidity();
    
protected:

	PrlHandleServerStatPtr m_pServerStat;
	PrlHandleServerVmPtr m_pServerVm;
	bool m_bStartInProgress;    // true if VM start operation is in progress
	CVmConfiguration m_VmConfig;

	mutable QMutex m_conMutex;
	SmartPtr<IODisplay::Connection> m_ioConnection;
};


#endif // __VIRTUOZZO_HANDLE_VM_H__
