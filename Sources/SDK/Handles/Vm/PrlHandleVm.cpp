/*
 * PrlHandleVm.cpp
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
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
 * Our contact details: Parallels International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


//#define FORCE_LOGGING_ON
//#define FORCE_LOGGING_LEVEL	DBG_DEBUG

#include <QBuffer>
#include <QByteArray>
#include <QDataStream>
#include <assert.h>
#include <memory>
#include <algorithm>
#include <QHostInfo>
#include <map>

#include <prlcommon/Logging/Logging.h>
#include <prlcommon/PrlUuid/Uuid.h>
#include <prlcommon/PrlCommonUtilsBase/countof.h>
#include <prlcommon/PrlCommonUtilsBase/ParallelsDirs.h>
#include <prlcommon/Std/PrlAssert.h>

#include <prlcommon/Messaging/CResult.h>
#include <prlcommon/Messaging/CVmEvent.h>
#include <prlcommon/Messaging/CVmEventParameter.h>

#include "PrlHandleVm.h"
#include "PrlHandleIOEvent.h"
#include "PrlHandleIOEventPackage.h"
#include "PrlHandleEvent.h"
#include "PrlControlValidity.h"
#include "PrlHandleVmEvent.h"
#include "PrlFileDescriptorsMech.h"

#include "PrlHandleVmCfg.h"
#include "PrlHandleVmDevice.h"

#ifdef ENABLE_MALLOC_DEBUG
    // By adding this interface we enable allocations tracing in the module
    #include "Interfaces/Debug.h"
#else
    // We're not allowed to throw exceptions from the library -
    // so we need to prevent operator ::new from doing this
    #include <new>
    using std::nothrow;
    #define new new(nothrow)
#endif

namespace IODisplay
{
Unit::Unit( const SmartPtr<IOService::ExecChannel>& channel ):
	m_ioExecChannel( channel )
{
}

bool Unit::IsConnected () const
{
	return m_ioExecChannel.isValid() && m_ioExecChannel->isValid();
}

Connection::Connection ( const Unit& display, const PrlHandleIOJob* ioJobPtr ):
	m_ioJob ( PRL_OBJECT_BY_HANDLE<PrlHandleIOJob>( ioJobPtr->GetHandle() ) )
{
	m_attachments.push_back(display);
}

PRL_RESULT Connection::Attach ( PrlHandleJob **const ioJobPtr )
{
	QMutexLocker l( &m_mutex );

	if ( m_attachments.empty() )
		return PRL_ERR_UNINITIALIZED;

	Unit a = m_attachments.front();
	m_attachments.push_back(a);

	if ( m_ioJob.isValid() ) {
		m_ioJob->AddRef();
		*ioJobPtr = m_ioJob.getHandle();

		return PRL_ERR_SUCCESS;
	}

	return PRL_ERR_VM_ALREADY_CONNECTED;
}

bool Connection::IsAttached ()
{
	QMutexLocker l( &m_mutex );

	if ( m_attachments.empty() )
		return false;

	return m_ioJob.isValid() || m_attachments.front().IsConnected();
}

void Connection::Detach ()
{
	QMutexLocker l( &m_mutex );

	if ( !m_attachments.empty() )
		m_attachments.pop_back();
}

void Connection::SetState ( IOService::Channel::State state )
{
	QMutexLocker l( &m_mutex );

	if ( m_ioJob.isValid() ) {
		m_ioJob->StateChanged(state);
		m_ioJob = PrlHandleSmartPtr<PrlHandleIOJob>();
	}

	if ( state != IOService::Channel::Started )
		m_attachments.clear();
}

} // namespace IODisplay

PrlHandleVm::PrlHandleVm ( const PrlHandleServerPtr& server ) :
	PrlHandleBase( PHT_VIRTUAL_MACHINE ),
	m_pServer( server ),
	m_pServerDisp( (PrlHandleServerDisp* )server.getHandle() ),
	m_pServerStat( (PrlHandleServerStat* )server.getHandle() ),
	m_pServerVm( (PrlHandleServerVm* )server.getHandle() ),
	m_bStartInProgress(false)
{
	m_VmConfig.getVmIdentification()->setVmUuid(Uuid::createUuid().toString());
	m_pServerVm->RegisterVm(m_VmConfig.getVmIdentification()->getVmUuid(), GetHandle());
}

PrlHandleVm::~PrlHandleVm ()
{
	VmDisconnectForcibly();
	m_pServerVm->UnregisterVm(m_VmConfig.getVmIdentification()->getVmUuid(), GetHandle());
}


PRL_RESULT PrlHandleVm::VmConnect ( PrlHandleJob **const pJob )
{
	{
		QMutexLocker g(&m_conMutex);
		if (m_ioConnection.isValid())
			return m_ioConnection->Attach(pJob);
	}
	// Inconsistent state: Connection is down by some reason.
	VmDisconnectForcibly();

	QString vmId = m_VmConfig.getVmIdentification()->getVmUuid();

	quint32 port = m_pServer->GetManagePort();
	if ( port == 0 ) {
#ifndef _WIN_
		if ( ! m_pServer->IsConnectionLocal() )
			port = PrlGetDefaultListenPort();
#else
		else
			port = PrlGetDefaultListenPort();
#endif
	}

	SmartPtr<IOService::ExecChannel> x;
	// Local connection
	if ( m_pServer->IsConnectionLocal() )
#ifndef _WIN_
		x = SmartPtr<IOService::ExecChannel>(new ExecChannel(
				m_pServer->GetSessionUuid(),
				vmId,
				true,
				ParallelsDirs::getDispatcherLocalSocketPath(),
				port,
				m_pServer->GetSecurityLevel(),
				true));
#else
		x = SmartPtr<IOService::ExecChannel>(new ExecChannel(
				m_pServer->GetSessionUuid(),
				vmId,
				true,
				IOService::LoopbackAddr,
				port,
				m_pServer->GetSecurityLevel()));
#endif
	// Remote connections (generic or proxy)
	else
	{
		x = SmartPtr<IOService::ExecChannel>(new ExecChannel(
					m_pServer->GetSessionUuid(),
					vmId,
					false,
					m_pServer->GetRemoteHost(),
					port,
					m_pServer->GetSecurityLevel()));
	}

	if ( ! x.isValid() )
		return PRL_ERR_OUT_OF_MEMORY;

	// Catch state changes
	QObject::connect(
				  x.getImpl(),
				  SIGNAL(onStateChange(IOService::Channel::State)),
				  SLOT(IOStateChanged(IOService::Channel::State)),
				  Qt::DirectConnection );

	// Catch all packages
	QObject::connect(
				  x.getImpl(),
				  SIGNAL(onPackageReceived(const SmartPtr<IOPackage>)),
				  SLOT(IOPackageReceived(const SmartPtr<IOPackage>)),
				  Qt::DirectConnection );

	// Catch all received packages
	QObject::connect(
				  x.getImpl(),
				  SIGNAL(onResponsePackageReceived(IOSendJob::Handle, const SmartPtr<IOPackage>)),
				  SLOT(IOResponsePackageReceived(IOSendJob::Handle, const SmartPtr<IOPackage>)),
				  Qt::DirectConnection );

	QMutexLocker g(&m_HandleMutex);
	if (m_ioExecChannel.isValid())
		return PRL_ERR_INVALID_HANDLE;

	x->enable();
	m_ioExecChannel = x;
	PrlHandleIOJob *const ioJob = new PrlHandleIOJob(
		PrlHandleIOJob::IOConnectJob,
		SmartPtr<IOService::ExecChannel>(m_ioExecChannel) );
	ioJob->SetVmHandle(GetHandle());

	m_ioConnection = SmartPtr<IODisplay::Connection>(
		new IODisplay::Connection( IODisplay::Unit(m_ioExecChannel), ioJob ) );

	*pJob = ioJob;

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVm::VmDisconnect ()
{
	SmartPtr<IOService::ExecChannel> x;
	{
		QMutexLocker g(&m_conMutex);
		if (!m_ioConnection.isValid())
			return PRL_ERR_SUCCESS;
		m_ioConnection->Detach();

		if (m_ioConnection->IsAttached())
			return PRL_ERR_SUCCESS;

		x = m_ioExecChannel;
		m_ioConnection.reset();
		m_ioExecChannel.reset();
		m_bStartInProgress = false;
	}
	if (x.isValid())
		x->disable();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVm::VmDisconnectForcibly ()
{
	SmartPtr<IOService::ExecChannel> x;
	{
		QMutexLocker g(&m_conMutex);
		x = m_ioExecChannel;
		m_ioConnection.reset();
		m_ioExecChannel.reset();
		m_bStartInProgress = false;
	}
	if (x.isValid())
		x->disable();

	return PRL_ERR_SUCCESS;
}

SmartPtr<IOService::ExecChannel> PrlHandleVm::GetChannel()
{
	return SmartPtr<IOService::ExecChannel>(m_ioExecChannel.getImpl());
}

IOService::Channel* PrlHandleVm::GetIOChannel ()
{
	return m_ioExecChannel.getImpl();
}

void PrlHandleVm::IOStateChanged (  IOService::Channel::State s )
{
	PrlHandleIOEvent* event = 0;
	PRL_IO_STATE* state = 0;
	PRL_VOID_PTR vptr;

	event = new PrlHandleIOEvent( m_pServer, PET_IO_STATE,
								  sizeof(PRL_IO_STATE), 0 );
	event->GetDataPtr((PRL_VOID_PTR_PTR)&vptr);
	state = (PRL_IO_STATE *)vptr;
	Q_ASSERT(state);

	switch ( s ) {
	case IOService::Channel::Disabled:
		*state = IOS_DISABLED;
		break;
	case IOService::Channel::Started:
		*state = IOS_STARTED;
		break;
	case IOService::Channel::Stopped:
		*state = IOS_STOPPED;
		break;
	case IOService::Channel::UnknownVMId:
		*state = IOS_UNKNOWN_VM_UUID;
		break;
	case IOService::Channel::ConnectionTimeout:
		*state = IOS_CONNECTION_TIMEOUT;
		break;
	case IOService::Channel::AuthenticationFailed:
		*state = IOS_AUTH_FAILED;
		break;

	default :
		*state = IOS_DISABLED;
		break;
	}
	if (m_ioConnection.isValid())
	{
		QMutexLocker g(&m_conMutex);
		m_bStartInProgress = false;
		if (m_ioConnection.isValid())
			m_ioConnection->SetState(s);

		if (s != IOService::Channel::Started)
			m_ioConnection.reset();
	}

	m_eventSource.NotifyListeners(event);
	event->Release();
}

void PrlHandleVm::IOPackageReceived ( const SmartPtr<IOPackage> p )
{
	if ( ! p.isValid() )
		return;

	PrlHandleEvent* event = 0;

	WRITE_TRACE(DBG_DEBUG, "Received IO event: %d '%s'", p->header.type, PRL_EVENT_TYPE_TO_STRING(p->header.type));
	switch ( p->header.type )
	{
	case PET_IO_DEVICE_IS_IN_USE : {
		PRL_IO_DEVICE_USED* devused;

		if (p->data[0].bufferSize < sizeof(*devused))
			break;
		event = new PrlHandleIOEvent(
			m_pServer, PET_IO_DEVICE_IS_IN_USE, sizeof(*devused),
			p->header.numericId);
		if ( event == 0 ) {
			WRITE_TRACE(DBG_FATAL, "Error: memory allocation problems!");
			return;
		}
		event->GetDataPtr((PRL_VOID_PTR_PTR)&devused);
		::memcpy(devused, p->buffers[0].getImpl(), sizeof(*devused));
		break;
	}

	case PET_IO_KEYBOARD_LEDS : {
		PRL_IO_KEYBOARD_LEDS* leds;

		if (p->data[0].bufferSize < sizeof(*leds))
			break;
		event = new PrlHandleIOEvent(
			m_pServer, PET_IO_KEYBOARD_LEDS, sizeof(*leds),
			p->header.numericId);
		if ( event == 0 ) {
			WRITE_TRACE(DBG_FATAL, "Error: memory allocation problems!");
			return;
		}
		event->GetDataPtr((PRL_VOID_PTR_PTR)&leds);
		::memcpy(leds, p->buffers[0].getImpl(), sizeof(*leds));
		break;
	}

	case PET_IO_SCREEN_SURFACE_ATTACHED:
	case PET_IO_SCREEN_SURFACE_DETACHED: {
		PRL_IO_SCREEN_SURFACE *surf;

		if (p->data[0].bufferSize < sizeof(*surf))
			break;
		event = new PrlHandleIOEvent(
			m_pServer, (PRL_EVENT_TYPE)p->header.type,
			sizeof(*surf), p->header.numericId );
		if ( event == 0 ) {
			WRITE_TRACE(DBG_FATAL, "Error: memory allocation problems!");
			return;
		}
		event->GetDataPtr((PRL_VOID_PTR_PTR)&surf);
		::memcpy(surf, p->buffers[0].getImpl(), sizeof(*surf));
		break;
	}

	case PET_IO_TOOLS_CLIPBOARD_DATA: {
		const PRL_IO_TOOLS_UTILITY_COMMAND *clip =
			reinterpret_cast<const PRL_IO_TOOLS_UTILITY_COMMAND*>(p->buffers[0].getImpl());

		if ( p->data[0].bufferSize < sizeof(*clip) )
			break;
		if ( p->data[0].bufferSize < sizeof(*clip) + clip->dataSize )
			break;

		// create event
		event = new PrlHandleIOEvent( m_pServer, PET_IO_TOOLS_CLIPBOARD_DATA,
									  sizeof(*clip) + clip->dataSize,
									  p->header.numericId);
		if ( event == 0 ) {
			WRITE_TRACE(DBG_FATAL, "Error: memory allocation problems!");
			return;
		}

		PRL_IO_TOOLS_UTILITY_COMMAND* clipEvent = 0;
		event->GetDataPtr((PRL_VOID_PTR_PTR)&clipEvent);

		::memcpy( clipEvent, p->buffers[0].getImpl(),
				  sizeof(*clip) + clip->dataSize );
		break;
	}
	case PET_IO_TOOLS_GENERAL_COMMAND : {

		const PRL_IO_TOOLS_UTILITY_COMMAND *clip =
			reinterpret_cast<const PRL_IO_TOOLS_UTILITY_COMMAND*>(p->buffers[0].getImpl());

		if ( p->data[0].bufferSize < sizeof(*clip) )
			break;
		if ( p->data[0].bufferSize < sizeof(*clip) + clip->dataSize )
			break;

		// create event
		event = new PrlHandleIOEvent( m_pServer, PET_IO_TOOLS_GENERAL_COMMAND,
			sizeof(*clip) + clip->dataSize, p->header.numericId );
		if ( event == 0 ) {
			WRITE_TRACE(DBG_FATAL, "Error: memory allocation problems!");
			return;
		}

		PRL_IO_TOOLS_UTILITY_COMMAND* clipEvent = 0;
		event->GetDataPtr((PRL_VOID_PTR_PTR)&clipEvent);

		::memcpy( clipEvent, p->buffers[0].getImpl(),
			sizeof(*clip) + clip->dataSize );
		break;

	}

	case PET_IO_DYNRES_TOOL_STATUS : {
		PRL_BOOL_PTR prlStatusPtr = 0;
		PRL_VOID_PTR vptr;
		unsigned status = 0;

		if ( p->data[0].bufferSize < sizeof(status) )
		{
			WRITE_TRACE(DBG_WARNING, "DynRes: Invalid size (%u)",
						p->data[0].bufferSize);
			break;
		}

		status = *(unsigned *)p->buffers[0].getImpl();

		event = new PrlHandleIOEvent(
			m_pServer, PET_IO_DYNRES_TOOL_STATUS, sizeof(PRL_BOOL),
			p->header.numericId );
		if ( event == 0 ) {
			WRITE_TRACE(DBG_FATAL, "DynRes: Error: memory allocation problems!");
			return;
		}

		event->GetDataPtr(&vptr);
		Q_ASSERT(vptr);
		prlStatusPtr = (PRL_BOOL_PTR)vptr;

		(*prlStatusPtr) = ( (0 == status)? PRL_FALSE: PRL_TRUE );

		break;
	}

	case PET_IO_TOOLS_DESKTOP_UTILITY_STATE : {

		PRL_IO_DESKTOP_UTILITIES_STATE * DUStateEvent = 0;

		if ( p->data[0].bufferSize < sizeof(*DUStateEvent) )
			break;

		event = new PrlHandleIOEvent( m_pServer, PET_IO_TOOLS_DESKTOP_UTILITY_STATE,
			sizeof(*DUStateEvent), p->header.numericId );
		if ( event == 0 ) {
			WRITE_TRACE(DBG_FATAL, "Error: memory allocation problems!");
			return;
		}

		event->GetDataPtr( (PRL_VOID_PTR_PTR)&DUStateEvent );
		::memcpy( DUStateEvent, p->buffers[0].getImpl(), sizeof(*DUStateEvent) );

		break;
	}
	case PET_IO_TOOLS_LANGUAGE_HOTKEY_CHANGED:
	{
		PRL_IO_LANGUAGE_HOTKEYS *LngHotkeysEvent = 0;

		if ( p->data[0].bufferSize < sizeof(*LngHotkeysEvent) )
			break;

		event = new PrlHandleIOEvent(
			m_pServer, PET_IO_TOOLS_LANGUAGE_HOTKEY_CHANGED, sizeof(*LngHotkeysEvent),
			p->header.numericId );
		if ( event == 0 ) {
			WRITE_TRACE(DBG_FATAL, "Error: memory allocation problems!");
			return;
		}

		event->GetDataPtr( (PRL_VOID_PTR_PTR)&LngHotkeysEvent );
		::memcpy( LngHotkeysEvent, p->buffers[0].getImpl(),
			sizeof(*LngHotkeysEvent) );
	}
	break;

	case PET_IO_TOOLS_SIA_DATA:
	{
		const PRL_IO_TOOLS_SIA_DATA* siaData =
			reinterpret_cast<const PRL_IO_TOOLS_SIA_DATA*>(p->buffers[0].getImpl());

		if ( p->data[0].bufferSize < sizeof(*siaData) )
			break;
		if ( p->data[0].bufferSize < siaData->size )
			break;

		event = new PrlHandleIOEvent( m_pServer, PET_IO_TOOLS_SIA_DATA,
									  sizeof(*siaData) + siaData->size,
									  p->header.numericId );
		if ( event == 0 ) {
			WRITE_TRACE(DBG_FATAL, "Error: memory allocation problems!");
			return;
		}
		PRL_IO_TOOLS_SIA_DATA* siaDataEvent = 0;
		event->GetDataPtr( (PRL_VOID_PTR_PTR)&siaDataEvent );
		::memcpy( siaDataEvent, p->buffers[0].getImpl(),
				  sizeof(*siaData) + siaData->size );
	}
	break;

	case PET_IO_STDOUT_PORTION:
	case PET_IO_STDERR_PORTION:
	{
		switch(PrlFileDescriptorsMech::ProcessStdoutStderrEvent(p))
		{
		case PRL_ERR_SUCCESS:
			break;

		case PRL_ERR_FILE_NOT_FOUND:
		{
			event = new PrlHandleIOEvent( m_pServer, PRL_EVENT_TYPE(p->header.type),
									   p->data[0].bufferSize,
									   p->header.numericId);
			if ( event == 0 ) {
				WRITE_TRACE(DBG_FATAL, "Error: memory allocation problems!");
				return;
			}
			PRL_VOID_PTR pDataBuffer = 0;
			event->GetDataPtr( &pDataBuffer );
			if( ! pDataBuffer )
			{
				WRITE_TRACE( DBG_FATAL, "Error: incomming data buffer memory allocation problems! cannot allocate (%d) size",
								p->data[0].bufferSize );
				PRL_ASSERT( false );
				event->Release();
				event = 0;
			}
			else
				::memcpy( pDataBuffer, p->buffers[0].getImpl(),
						p->data[0].bufferSize );
		}
		break;

		case PRL_ERR_FAILURE:
		{
			PrlHandleServerJobPtr pJob = PrlHandleServerJob::GetJobByUuid( Uuid::toString( p->header.parentUuid ) );
			PrlRunProgramInGuestJob *pRunProgramJob = dynamic_cast<PrlRunProgramInGuestJob *>( pJob.getHandle() );
			if ( pRunProgramJob )
			{
				pRunProgramJob->Cancel();
				UnregisterJob( pRunProgramJob );
			}
		}
		break;
		}
	}
	break;

	case PET_IO_FIN_TO_TRANSMIT_STDOUT_STDERR:
	{
		PrlFileDescriptorsMech::ProcessFinPackage(p);
		return;
	}
	break;

	case PET_IO_TTY_SETTINGS_REQUIRED:
	{
		PrlFileDescriptorsMech::ProcessTtyPackage(p);
		return;
	}
	break;

	case PET_IO_AVAILABLE_DISPLAYS : {
		PRL_IO_AVAILABLE_DISPLAYS* availableDisplays;

		if (p->data[0].bufferSize < sizeof(*availableDisplays))
			break;
		event = new PrlHandleIOEvent(
			m_pServer, PET_IO_AVAILABLE_DISPLAYS, sizeof(*availableDisplays),
			p->header.numericId);
		if ( event == 0 ) {
			WRITE_TRACE(DBG_FATAL, "Error: memory allocation problems!");
			return;
		}
		event->GetDataPtr((PRL_VOID_PTR_PTR)&availableDisplays);
		::memcpy(availableDisplays, p->buffers[0].getImpl(), sizeof(*availableDisplays));
		break;
	}

	case PET_IO_SSO_GET_CREDENTIALS:
	{
		PRL_IO_SSO_CLIENT_VM_PACKET* ssoClientVmPacket;

		if (p->data[0].bufferSize < sizeof(*ssoClientVmPacket))
			break;
		event = new PrlHandleIOEvent(
			m_pServer, PET_IO_SSO_GET_CREDENTIALS, sizeof(*ssoClientVmPacket),
			p->header.numericId);

		if ( event == 0 ) {
			WRITE_TRACE(DBG_FATAL, "Error: memory allocation problems!");
			return;
		}
		event->AddRef();
		event->GetDataPtr((PRL_VOID_PTR_PTR)&ssoClientVmPacket);
		::memcpy(ssoClientVmPacket, p->buffers[0].getImpl(), sizeof(*ssoClientVmPacket));
		break;
	}

    /************************************/

	default : {
		// Unknown package. Nothing to do.
		return;
	}}

	if ( event ) {
		m_eventSource.NotifyListeners(event);
		event->Release();
	}
}

void PrlHandleVm::IOResponsePackageReceived (
    IOSendJob::Handle hSendJob,
	const SmartPtr<IOPackage> p )
{
	if ( ! p.isValid() )
		return;

	// Notify waiting IO jobs
	QList< PrlHandleSmartPtr<PrlHandleIOJob> > ioJobs = GetRegisteredIOJobs();
	QList< PrlHandleSmartPtr<PrlHandleIOJob> >::Iterator it =
		ioJobs.begin();
	while ( it != ioJobs.end() ) {
		PrlHandleSmartPtr<PrlHandleIOJob>& ioJob = *it;
		ioJob->ResponseReceived( hSendJob, p );
		++it;
	}
}

void PrlHandleVm::SetServer(const PrlHandleServerPtr &pServer)
{
	m_pServer = pServer;
	m_pServerDisp = PrlHandleServerDispPtr((PrlHandleServerDisp* )pServer.getHandle());
	m_pServerStat = PrlHandleServerStatPtr((PrlHandleServerStat* )pServer.getHandle());
	m_pServerVm = PrlHandleServerVmPtr((PrlHandleServerVm* )pServer.getHandle());
}

QList< PrlHandleSmartPtr<PrlHandleIOJob> > PrlHandleVm::GetRegisteredIOJobs ()
{
	QReadLocker locker( &m_ioJobsListRWLock );
	QList< PrlHandleSmartPtr<PrlHandleIOJob> > ioJobs = m_ioJobsList;
	return ioJobs;
}

void PrlHandleVm::ProcessVmEvent( PrlHandleBasePtr pEvent )
{
	if( pEvent->GetType() != PHT_EVENT )
		return;

	PrlHandleVmEvent *pVmEventHandle = static_cast<PrlHandleVmEvent *>( pEvent.getHandle() );
	CVmEvent &vmEvent = pVmEventHandle->GetEventObject();

	PRL_EVENT_TYPE vmEventType = vmEvent.getEventType();
	switch (vmEventType)
	{
	default:
		break;
	}
}

PRL_RESULT PrlHandleVm::CreateEvent(PRL_HANDLE_PTR phEvent)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleVmEvent *pVmEvent = new PrlHandleVmEvent(PrlHandleVmPtr(this));
	if (!pVmEvent)
		return (PRL_ERR_OUT_OF_MEMORY);
	*phEvent = pVmEvent->GetHandle();
	return (PRL_ERR_SUCCESS);
}

void PrlHandleVm::NotifyControlValidity()
{
	CVmHardware *pHardware = m_VmConfig.getVmHardwareList();
	if (pHardware)
	{
		foreach(CVmFloppyDisk *pFloppy, pHardware->m_lstFloppyDisks)
        PrlControlValidity::MarkAsInvalid(pFloppy);
		foreach(CVmHardDisk *pHardDisk, pHardware->m_lstHardDisks)
		{
			foreach(CVmHddPartition* pPartition, pHardDisk->m_lstPartition)
            PrlControlValidity::MarkAsInvalid(pPartition);
			PrlControlValidity::MarkAsInvalid(pHardDisk);
		}
		foreach(CVmOpticalDisk *pOpticalDisk, pHardware->m_lstOpticalDisks)
        PrlControlValidity::MarkAsInvalid(pOpticalDisk);
		foreach(CVmParallelPort *pParallelPort, pHardware->m_lstParallelPorts)
        PrlControlValidity::MarkAsInvalid(pParallelPort);
		foreach(CVmSerialPort *pSerialPort, pHardware->m_lstSerialPorts)
        PrlControlValidity::MarkAsInvalid(pSerialPort);
		foreach(CVmSoundDevice *pSoundDevice, pHardware->m_lstSoundDevices)
        PrlControlValidity::MarkAsInvalid(pSoundDevice);
		foreach(CVmUsbDevice *pUsbDevice, pHardware->m_lstUsbDevices)
        PrlControlValidity::MarkAsInvalid(pUsbDevice);
		foreach(CVmGenericNetworkAdapter *pNetworkAdapter, pHardware->m_lstNetworkAdapters)
        PrlControlValidity::MarkAsInvalid(pNetworkAdapter);
		foreach(CVmGenericPciDevice *pGenericPci, pHardware->m_lstGenericPciDevices)
        PrlControlValidity::MarkAsInvalid(pGenericPci);
		foreach(CVmGenericScsiDevice *pGenericScsi, pHardware->m_lstGenericScsiDevices)
        PrlControlValidity::MarkAsInvalid(pGenericScsi);
	}
	foreach(CVmSharedFolder *pSharedFolder, m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->getHostSharing()->m_lstSharedFolders)
    PrlControlValidity::MarkAsInvalid(pSharedFolder);
	foreach(CVmScreenResolution *pScrRes, m_VmConfig.getVmHardwareList()->getVideo()->getVmScreenResolutions()->m_lstScreenResolutions)
    PrlControlValidity::MarkAsInvalid(pScrRes);
	foreach(CVmStartupOptions::CVmBootDevice *pBootDev, m_VmConfig.getVmSettings()->getVmStartupOptions()->m_lstBootDeviceList)
    PrlControlValidity::MarkAsInvalid(pBootDev);
}

PRL_RESULT PrlHandleVm::fromString(PRL_CONST_STR vm_config)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	NotifyControlValidity();
	QString u = m_VmConfig.getVmIdentification()->getVmUuid();
	if (IS_OPERATION_SUCCEEDED(m_VmConfig.fromString(UTF8_2QSTR(vm_config))))
	{
		m_pServerVm->UnregisterVm(u, GetHandle());
		m_pServerVm->RegisterVm(m_VmConfig.getVmIdentification()->getVmUuid(), GetHandle());
		return PRL_ERR_SUCCESS;
	}
    
	return PRL_ERR_INVALID_ARG;
}

QString PrlHandleVm::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return m_VmConfig.toString();
}

PRL_RESULT PrlHandleVm::UpdateVmConfig(const QString &sNewConfig)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmConfiguration vmConfig;

	PRL_RESULT rc = vmConfig.fromString(sNewConfig);
	if ( PRL_FAILED(rc) )
	{
		WRITE_TRACE(DBG_FATAL, "An error occured on VM parsing: %.8X '%s'",
				rc, PRL_RESULT_TO_STRING(rc));
		return rc;
	}
	m_pServerVm->UnregisterVm(m_VmConfig.getVmIdentification()->getVmUuid(), GetHandle());

	NotifyControlValidity();

	m_VmConfig.fromString(sNewConfig);
	m_pServerVm->RegisterVm(vmConfig.getVmIdentification()->getVmUuid(), GetHandle());
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVm::SetUuid(PRL_CONST_STR sNewVmUuid)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString sNewVmUuidUtf8 = UTF8_2QSTR(sNewVmUuid);
	QString u = m_VmConfig.getVmIdentification()->getVmUuid();
	m_VmConfig.getVmIdentification()->setVmUuid(sNewVmUuidUtf8);
	if (!sNewVmUuidUtf8.isEmpty())
		m_pServerVm->RegisterVm(m_VmConfig.getVmIdentification()->getVmUuid(), GetHandle());

	m_pServerVm->UnregisterVm(u, GetHandle());

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVm::CreateAnswerEvent(PRL_HANDLE_PTR phEvent, PRL_RESULT nAnswer)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	SmartPtr<CVmEvent> pXmlVmEvent( new CVmEvent );
	pXmlVmEvent->setEventCode(PET_DSP_EVT_VM_QUESTION);
    
	pXmlVmEvent->addEventParameter(
                                   new CVmEventParameter(
                                                         PVE::String,
                                                         m_VmConfig.getVmIdentification()->getVmUuid(),
                                                         EVT_PARAM_VM_UUID
                                                         ) );
    
	pXmlVmEvent->addEventParameter(
                                   new CVmEventParameter(
                                                         PVE::UnsignedInt,
                                                         QString("%1").arg((quint32)nAnswer),
                                                         EVT_PARAM_MESSAGE_CHOICE_0
                                                         ) );
    
	PrlHandleVmEvent *pVmEvent = new PrlHandleVmEvent(PrlHandleVmPtr(this), pXmlVmEvent.getImpl());
	if (!pVmEvent)
		return (PRL_ERR_OUT_OF_MEMORY);
	*phEvent = pVmEvent->GetHandle();
	return (PRL_ERR_SUCCESS);
}

void PrlHandleVm::StartComplete( CResult* pResult )
{
	if( !PRL_SUCCEEDED(pResult->getReturnCode()) )
	{
		m_bStartInProgress = false;
	}
}

QString PrlHandleVm::GetUuid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (m_VmConfig.getVmIdentification())
		return (m_VmConfig.getVmIdentification()->getVmUuid());
	return (QString());
}
