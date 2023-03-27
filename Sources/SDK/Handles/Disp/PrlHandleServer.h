/*
 * PrlHandleServer.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2023 Virtuozzo International GmbH. All rights reserved.
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


#ifndef __VIRTUOZZO_HANDLE_SERVER_H__
#define __VIRTUOZZO_HANDLE_SERVER_H__

#include "PrlEventsHandler.h"
#include "PveControl.h"
#include "PrlQuestionsList.h"
#include "PrlHandleHandlesList.h"

#include <prlcommon/PrlCommonUtilsBase/CFeaturesMatrix.h>
#include "PrlHandleGuestOsesMatrix.h"

class PrlHandleBackup;

/**
 * Base handle object - all objects representation on the client
 * side should follow it's interface by the means of inheritance.
 */
class PrlHandleServer : public PrlHandleBase
{

public:

	/**
	 * Class default constructor.
	 */
	PrlHandleServer();

	/**
	 * Class destructor.
	 */
	~PrlHandleServer();

	bool isStarted() const;

	/**
	 * Initialize object from XML string
	 */
	virtual PRL_RESULT fromString(PRL_CONST_STR sXml);

	/**
	 * Returns object as XML string
	 */
	virtual QString toString();

	/**
	 * Returns true or false according to current connection:
	 * true if local, false otherwise.
	 */
	bool IsConnectionLocal () const;

	/**
	 * Returns true if connection is proxified.
	 */
	bool IsProxyConnection () const;

	/**
	 * Returns session UUID
	 */
	QString GetSessionUuid () const;

	/**
	 * Returns remote host name
	 */
	QString GetRemoteHost () const;

	/**
	 * Returns management connection port number
	 */
	PRL_UINT32 GetManagePort() const;

	/**
	 * Returns connection security level
	 */
	PRL_SECURITY_LEVEL GetSecurityLevel() const;

	/**
	 * Return proxy peer handle
	 */
	IOSender::Handle GetProxyPeerHandle () const;

	/**
	* Sets restriction event.
	*/
	void SetRestrictionEvent(QString qsRestrictEvt);

	/**
	 * Returns EventSource object.
	 */
	BuiltinEventSource *eventSource() { return &m_eventSource; }

	/**
	 * Registers notification for specified object
	 */
	void Notify( PrlHandleBasePtr pEvent );

	/**
	 * This is for compatibility with legacy code and should be removed in the future (!).
	 *
	 * @return PVE control object pointer for external usage
	 */
	CPveControl* GetPveControl() const;

	/**
	 * Returns pointer to VM handle instance that matching to specified VM uuid
	 *
	 * @param VM uuid
	 */
	PrlHandleVmPtr GetVmHandleByUuid(const QString &vm_uuid);

	/**
	 * Try to receive already instantiated VM object by specified VM config
	 *
	 * @param VM configuration
	 * @return pointer to VM handle instance
	 */
	PrlHandleVmPtr GetVmByConfig(const QString &vm_config);

	/**
	 * Let to directly finalize all transport active IO operations
	 */
	void StopTransport();

	/**
	 * Stops notification
	 */
	void StopNotificationThread();

	/**
	 * Applies server information (server UUID and host OS version) from login response
	 * @param pointer to result object
	 */
	void ProcessLoginResponse(CResult *pResult);

	/**
	 * Adds job object to response awaiting objects list
	 * @param pointer to adding job object
	 */
	void AddJobToResponseAwaitingList(const PrlHandleServerJobPtr &pJob);
	/**
	 * Removes job object from response awaiting objects list
	 * @param pointer to removing job object
	 */
	void RemoveJobFromResponseAwaitingList(const PrlHandleServerJobPtr &pJob);

public://Global nethods
	/**
	 * Finalizes all transpot IO operations
	 */
	static void FinalizeAllTransportConnections();

	/**
	 * Clean all registered stopped threads
	 */
	static void CleanRegisteredThreads();

	/**
	 * Register thread for deferred delete
	 */
	static void RegisterThreadToDelete(SmartPtr<CNotificationThread> &);

	/**
	 * Register thread for deferred delete from common finalization mech
	 */
	static void RegisterThreadToDeleteFinMech(SmartPtr<CNotificationThread> &);

public://VM operations calls

	/**
	* Fulfills Internal VM command
	* @param VM uuid
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdVmInternal(PRL_CONST_STR sVmUuid, PRL_CONST_STR cmd,
						const QStringList &lstArgs);
	/**
	 * Run the Compressor process
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmRunCompressor(PRL_CONST_STR sVmUuid);
	/**
	 * Cancel the Compressor process
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmCancelCompressor(PRL_CONST_STR sVmUuid);

	/**
	 * Update Nvram for VM
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmUpdateNvram(PRL_CONST_STR sVmUuid);
	/**
	* Commit Tools section VM config at runtime
	* @param VM uuid
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdVmUpdateToolsSection(PRL_CONST_STR sVmUuid, PRL_CONST_STR sToolsSection);

	/**
	 * Cancels specified job
	 * @param job uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdUserCancelOperation(PRL_CONST_STR sJobUuid);

	PrlHandleJobPtr DspCmdVmStorageSetValue(PRL_CONST_STR sVmUuid, PRL_CONST_STR sKey, PRL_CONST_STR sValue
	  , PRL_UINT32 nFlags);

    /**
     * Send problem report by the means of a server
     */
    PrlHandleJobPtr DspCmdSendProblemReport(PRL_CONST_STR sVmUuid,
        PRL_HANDLE hProblemReport, PRL_UINT32 nFlags);

	/**
	 * Get CPU pools list
	 */
	PrlHandleJobPtr GetCPUPoolsList();

	/**
	 * Join server to default cpu pool
	 */
	PrlHandleJobPtr JoinCPUPool();

	/**
	 * Join server to default cpu pool
	 */
	PrlHandleJobPtr LeaveCPUPool();

	/**
	 * Move server to a different cpu pool
	 */
	PrlHandleJobPtr MoveToCPUPool(PRL_CONST_STR sCpuPool);

	/**
	 * Recalculates cpu pool features
	 */
	PrlHandleJobPtr RecalculateCPUPool(PRL_CONST_STR sCpuPool);

	/**
	 * Starts backup activity
	 */
	PrlHandleJobPtr BeginBackup(PrlHandleVm& vm_, PRL_UINT32 flags_);

	/**
	 * Ends backup activity
	 */
	PrlHandleJobPtr EndBackup(const PrlHandleBackup& backup_, PRL_UINT32 flags_);

protected:
	/// Flag to determine which connection is used: local or remote
	bool m_isConnectionLocal;

	/// Flat to determine proxy connection
	bool m_isProxyConnection;

	/// Stub object of client/dispatcher interoperations
	CPveControl* m_pPveControl;

	/// Events processing object
	CEventsHandler* m_pEventsHandler;

	/// Events handlers list
	BuiltinEventSource m_eventSource;

	/// Remote host name
	QString m_sHostname;

	/// Management port number
	PRL_UINT32 m_nManagePort;

	/// Connection security level
	PRL_SECURITY_LEVEL m_nSecurityLevel;

	/// Object for synchronizing remote host name and IO port values access
	mutable QMutex m_MembersMutex;

	/**
	 * Map holding all VM handle objects by their uuid's.
	 */
	typedef std::map< QString, PRL_HANDLE > HandleVmUuidMap;
	HandleVmUuidMap m_HandleVmUuidMap;

	/** Session UUID string */
	QString m_sSessionUuid;

	/** Server UUID string */
	QString m_sServerUuid;

	/** Server host OS version */
	QString m_sHostOsVersion;

	/** Server product version */
	QString m_sProductVersion;

	/** Proxy peer handle */
	IOSender::Handle m_sProxyPeerHandle;

	/** Response awaiting objects list */
	QList<PrlHandleServerJobPtr> m_ResponseAwaitingList;

	/** Questions list */
	PrlQuestionsList m_QuestionsList;

	/** Noninteractive session mode */
	PRL_BOOL m_bNonInteractiveSession;

	/** Confirmation session mode */
	PRL_BOOL m_bConfirmationModeEnabled;

	/** Server Execute Mode **/
	PRL_APPLICATION_MODE m_nServerAppExecuteMode;

	/** Service start time in msecs since Epoch 01-01-1970 (wall clock time) */
	PRL_UINT64 m_nStartTime;

	/** Service start time in msecs (monotonic clock time) */
	PRL_UINT64 m_nStartTimeMonotonic;

	/** Pointer to supported guest OSes matrix */
	SmartPtr<COsesMatrix> m_pSupportedOsesMatrix;

	CVmEvent	m_evtRestriction;

	/** Supported by remote side features matrix */
	CFeaturesMatrix m_FeaturesMatrix;

	/**
	 * Temporary solution for preventing Login/Logoff issues.
	 * Kust deletes internal PveControl and creates new one.
	 */
	void InitializeConnection();

	/**
	 * Init connection settings
	 */
	void InitConnectionSettings(const QString& hostName,
	                            bool ioLocal,
	                            bool isProxyConnection,
	                            PRL_UINT32 port_cmd,
	                            PRL_SECURITY_LEVEL security_level );

    PrlHandleJobPtr create_job(const QString& job_uuid, PRL_JOB_OPERATION_CODE job_code) ;

};

#endif // __VIRTUOZZO_HANDLE_SERVER_H__
