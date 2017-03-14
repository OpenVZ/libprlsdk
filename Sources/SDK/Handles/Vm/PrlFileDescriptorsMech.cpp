/*
 * PrlFileDescriptorsMech.cpp
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


#include "PrlFileDescriptorsMech.h"
#include "PrlCommon.h"
#include "PrlHandleVm.h"

#include <prlcommon/Std/PrlAssert.h>

#include <QReadWriteLock>
#include <QMap>
#include <QMutex>
#include <QWaitCondition>
#include <QByteArray>
#include <QPair>
#include <QScopedPointer>

#ifndef _WIN_
#include <errno.h>
#include <fcntl.h>
#else
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef _LIN_
#include <pty.h>
#endif

namespace PrlFileDescriptorsMech
{
typedef QMap<QString, QPair<PRL_FILE_DESC, PRL_FILE_DESC> > TFileDescriptorsMap;

Q_GLOBAL_STATIC(TFileDescriptorsMap, FileDescriptorsMap)
Q_GLOBAL_STATIC(QReadWriteLock, FileDescriptorsMapLocker)

/**
 * Implementation of stdin file descriptors multiplexer
 */
class CStdinMaintainer : public Attached
{
public:
	typedef QMap<QString, QPair<PRL_FILE_DESC, bool> > TStdinDescsMap;
	/**
	 * Class constructor
	 */
	CStdinMaintainer();
	/**
	 * Class destructor
	 */
	~CStdinMaintainer();
	/**
	 * Registries specified job at stdin descriptors list
	 * @param pointer to registering job instance
	 */
	void RegisterJob( PrlRunProgramInGuestJob *pJob );
	/**
	 * Unregistries specified job from stdin descriptors list
	 * @param pointer to unregistering job instance
	 */
	void UnregisterJob( PrlRunProgramInGuestJob *pJob );
	/**
	 * Marks specified job entry with sign that remote side ready to accept
	 * stdin portions.
	 * @param pointer to target job instance
	 */
	void MarkReadyToAcceptStdin( PrlRunProgramInGuestJob *pJob );
	/**
	 * Finalizes thread work
	 */
	void reactQuit();

public:
	/** Returns pointer to the global stdin file descriptors mech instance */
	static CStdinMaintainer *instance();

private:
	/** Overridden template thread function */
	void concreteRun();

private:
	/** List of stdin descriptors */
	TStdinDescsMap m_StdinDescsMap;
	/** Mutex for synchronization access to the stdin descriptors list */
	QMutex m_StdinDescsMapMutex;
	/** Condition object to simplify context switching actions */
	QWaitCondition m_cond;
	/** Sign specifies whether thread should finalize it work or not */
	bool m_bFinalizeWork;
};

Q_GLOBAL_STATIC(CStdinMaintainer, getStdinMaintainer)

CStdinMaintainer::~CStdinMaintainer()
{
	QMutexLocker _lock(&m_StdinDescsMapMutex);
	m_StdinDescsMap.clear();
}

void CStdinMaintainer::RegisterJob( PrlRunProgramInGuestJob *pJob )
{
	QMutexLocker _lock( &m_StdinDescsMapMutex );
#ifndef _WIN_
	if (fcntl(pJob->GetStdin(), F_SETFL, O_NONBLOCK) != 0)
		WRITE_TRACE(DBG_FATAL, "an error on fcntl exec: %d", errno);
#endif
	m_StdinDescsMap.insert( pJob->GetJobUuid(), qMakePair( pJob->GetStdin(), false ) );
	m_cond.wakeAll();
}

void CStdinMaintainer::UnregisterJob( PrlRunProgramInGuestJob *pJob )
{
	QMutexLocker _lock( &m_StdinDescsMapMutex );
	TStdinDescsMap::iterator _it = m_StdinDescsMap.find( pJob->GetJobUuid() );
	if ( m_StdinDescsMap.end() != _it )
		m_StdinDescsMap.erase( _it );
}

void CStdinMaintainer::MarkReadyToAcceptStdin( PrlRunProgramInGuestJob *pJob )
{
	QMutexLocker _lock( &m_StdinDescsMapMutex );
	TStdinDescsMap::iterator _it = m_StdinDescsMap.find( pJob->GetJobUuid() );
	if ( m_StdinDescsMap.end() != _it )
	{
		_it.value().second = true;
		m_cond.wakeAll();
	}
}

void CStdinMaintainer::reactQuit()
{
	QMutexLocker _lock( &m_StdinDescsMapMutex );
	m_bFinalizeWork = true;
	m_cond.wakeAll();
}

CStdinMaintainer *CStdinMaintainer::instance()
{
	CStdinMaintainer* output = getStdinMaintainer();
	if (!output->m_bFinalizeWork)
	{
		QMutexLocker g(&output->m_StdinDescsMapMutex);
		if (!output->m_bFinalizeWork && !output->isRunning() &&
			output->connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), SLOT(reactQuit())))
			output->start();
	}
	return output;
}

CStdinMaintainer::CStdinMaintainer(): m_bFinalizeWork(false)
{
}

#define STDIN_THREAD_TIMEOUT 50
void CStdinMaintainer::concreteRun()
{
	while ( !m_bFinalizeWork )
	{
		QMutexLocker _lock( &m_StdinDescsMapMutex );
		TStdinDescsMap::iterator _it = m_StdinDescsMap.begin();
		while( m_StdinDescsMap.end() != _it )
		{
			if ( _it.value().second )
			{
				PrlHandleServerJobPtr pJob = PrlHandleServerJob::GetJobByUuid( _it.key() );
				if ( !pJob.getHandle() )//Already destroyed job object detected
				{
					_it = m_StdinDescsMap.erase( _it );
					continue;
				}
				else
				{
					PrlHandleVmPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVm>( pJob->GetVmHandle() );
					if ( pVm.getHandle() && pVm->GetIOChannel() )//Perhaps job object wasn't initialized yet
					{
// 1 Mb
#define MAX_STDIN_PORTION 1048576
						QByteArray _buffer;
						char nChar = 0;
#ifndef _WIN_
						int nCharNums = read( _it.value().first, &nChar, sizeof(nChar) );
						int nRetCode = errno;
#else
						DWORD nCharNums = 0;
						PeekNamedPipe( _it.value().first, &nChar, sizeof(nChar), &nCharNums, 0, 0 );
						//Under Win platform we have to process console events explicitly
						if ( nCharNums == 0)
						{
							INPUT_RECORD _record;
							DWORD nNumOfEvents = 0;
							if ( PeekConsoleInput( _it.value().first, &_record, 1, &nNumOfEvents) && 1 == nNumOfEvents )
							{
								if ( KEY_EVENT == _record.EventType)
								{
									_buffer.resize( BUFSIZ );
									ReadFile( _it.value().first, _buffer.data(), _buffer.size(), &nCharNums, 0 );
									if ( nCharNums < BUFSIZ )
										_buffer.resize( nCharNums );
								}
								else
									ReadConsoleInput( _it.value().first, &_record, 1, &nNumOfEvents);
							}
						}
#endif
						while( sizeof(nChar) == nCharNums )
						{
							_buffer.append( nChar );
							if ( _buffer.size() >= MAX_STDIN_PORTION )
								break;
#ifndef _WIN_
							nCharNums = read( _it.value().first, &nChar, sizeof(nChar) );
							nRetCode = errno;
#else
							ReadFile( _it.value().first, &nChar, sizeof(nChar), &nCharNums, 0 );
							PeekNamedPipe( _it.value().first, &nChar, sizeof(nChar), &nCharNums, 0, 0 );
#endif
						}
						if (_buffer.size())
						{
							SmartPtr<IOPackage> p = IOPackage::createInstance( PET_IO_STDIN_PORTION, 1);
							p->fillBuffer(0, IOPackage::RawEncoding, _buffer.constData(), _buffer.size() );
							Uuid_t parentUuid;
							Uuid::dump( _it.key(), parentUuid );
							::memcpy( p->header.parentUuid,	parentUuid, sizeof(Uuid_t) );
							pVm->GetIOChannel()->sendPackage(p);
						}
						//Process broken pipe case
						else if ( 0 == nCharNums || -1 == nCharNums )
						{
#ifndef _WIN_
							if ( ( 0 == nCharNums ) || ( nRetCode != EAGAIN && nRetCode != EINTR ) )
#else
							//FIXME: need to investigate correspond errors
							//codes for Windows platform
							if ( false )
#endif
							{
#ifndef _WIN_
								if ( -1 == nCharNums )
									WRITE_TRACE( DBG_FATAL, "Failed to read from stdin with error: %d", nRetCode );
#endif
								SmartPtr<IOPackage> p = IOPackage::createInstance( PET_IO_STDIN_WAS_CLOSED, 0);
								Uuid_t parentUuid;
								Uuid::dump( _it.key(), parentUuid );
								::memcpy( p->header.parentUuid,	parentUuid, sizeof(Uuid_t) );
								_it = m_StdinDescsMap.erase( _it );
								pVm->GetIOChannel()->sendPackage(p);
								continue;
							}
						}
					}
				}
			}
			++_it;
		}
		m_cond.wait( &m_StdinDescsMapMutex, STDIN_THREAD_TIMEOUT );
	}
}

void RegisterJob(PrlRunProgramInGuestJob *pJob)
{
	if ( PRL_INVALID_FILE_DESCRIPTOR != pJob->GetStdin() )
		CStdinMaintainer::instance()->RegisterJob( pJob );
	QWriteLocker _lock( FileDescriptorsMapLocker() );
	FileDescriptorsMap()->insert( pJob->GetJobUuid(), qMakePair( pJob->GetStdout(), pJob->GetStderr() ) );
}

void UnregisterJob(PrlRunProgramInGuestJob *pJob)
{
	if ( PRL_INVALID_FILE_DESCRIPTOR != pJob->GetStdin() )
		CStdinMaintainer::instance()->UnregisterJob( pJob );
	QWriteLocker _lock( FileDescriptorsMapLocker() );
	TFileDescriptorsMap::iterator _it = FileDescriptorsMap()->find( pJob->GetJobUuid() );
	if ( FileDescriptorsMap()->end() != _it )
		FileDescriptorsMap()->erase( _it );
}

void MarkReadyToAcceptStdin( const QString &sRequestId )
{
	PrlHandleServerJobPtr pJob = PrlHandleServerJob::GetJobByUuid( sRequestId );
	PrlRunProgramInGuestJob *pRunProgramJob = dynamic_cast<PrlRunProgramInGuestJob *>( pJob.getHandle() );
	if ( ! pRunProgramJob )
		return;

	if ( PRL_INVALID_FILE_DESCRIPTOR != pRunProgramJob->GetStdin() )
		CStdinMaintainer::instance()->MarkReadyToAcceptStdin( pRunProgramJob );

	PrlHandleVmPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVm>( pRunProgramJob->GetVmHandle() );
	PRL_ASSERT( pVm.getHandle() );

	if ( pVm.getHandle() )
	{
		SmartPtr<IOPackage> p = IOPackage::createInstance( PET_IO_READY_TO_ACCEPT_STDOUT_STDERR_PKGS, 0);
		Uuid_t parentUuid;
		Uuid::dump( pRunProgramJob->GetJobUuid(), parentUuid );
		::memcpy( p->header.parentUuid,	parentUuid, sizeof(Uuid_t) );
		pVm->GetIOChannel()->sendPackage(p);
	}
}

#ifndef _WIN_
//Implementation of full write to descriptor
namespace {
/**
 * Safe write to descriptor which wrapping EINTR and EINVAL errors
 * @param descriptor to which write
 * @param pointer to the buffer
 * @param buffer size
 * @return actual num bytes written or -1 in error case
 */
size_t safe_write(int fd, void const *buf, size_t count)
{
	for (;;)
	{
		ssize_t ret = write(fd, buf, count);

		if (0 <= ret)
			return ret;
		else if (EINTR == errno)
			continue;
		else
			return ret;
	}
}
/**
 * Complete write of specified buffer to the descriptor.
 * Wrapping EAGAIN and another staffs.
 * @param descriptor to which write
 * @param pointer to the buffer
 * @param buffer size
 * @return actual num bytes written or -1 in error case
 */
size_t full_write(int fd, const void *buf, size_t count)
{
	size_t total_bytes = 0;
	const char *p = (const char *) buf;

	while (count > 0)
	{
		size_t nWritten = safe_write(fd, p, count);
		if (nWritten == (size_t) -1 && EAGAIN != errno)
			break;
		if (nWritten == 0)
		{
			errno = ENOSPC;
			break;
		}
		if ((size_t)-1 != nWritten)
		{
			total_bytes += nWritten;
			p += nWritten;
			count -= nWritten;
		}
	}

	return total_bytes;
}

}
#endif

bool ProcessStdoutStderrEvent( const SmartPtr<IOPackage> &p )
{
	QReadLocker _lock( FileDescriptorsMapLocker() );
	TFileDescriptorsMap::const_iterator _it = FileDescriptorsMap()->find( Uuid::toString( p->header.parentUuid ) );
	if ( FileDescriptorsMap()->end() != _it )
	{
		if ( PET_IO_STDOUT_PORTION == p->header.type )
		{
			if ( PRL_INVALID_FILE_DESCRIPTOR != _it.value().first )
			{
#ifndef _WIN_
				int ret = full_write( _it.value().first, p->buffers[0].getImpl(), p->data[0].bufferSize );
				if (int(p->data[0].bufferSize) != ret)
					WRITE_TRACE(DBG_DEBUG, "Couldn't to write data due: '%s' written bytes: %d", strerror(errno), ret);
#else
				DWORD ret = 0;
				WriteFile( _it.value().first, p->buffers[0].getImpl(), p->data[0].bufferSize, &ret, 0 );
#endif
				return ( true );
			}
		}
		else if ( PET_IO_STDERR_PORTION == p->header.type )
		{
			if ( PRL_INVALID_FILE_DESCRIPTOR != _it.value().second )
			{
#ifndef _WIN_
				int ret = full_write( _it.value().second, p->buffers[0].getImpl(), p->data[0].bufferSize );
				if (int(p->data[0].bufferSize) != ret)
					WRITE_TRACE(DBG_DEBUG, "Couldn't to write data due: '%s' written bytes: %d", strerror(errno), ret);
#else
				DWORD ret = 0;
				WriteFile( _it.value().second, p->buffers[0].getImpl(), p->data[0].bufferSize, &ret, 0 );
#endif
				return ( true );
			}
		}
	}
	return ( false );
}

void ProcessFinPackage( const SmartPtr<IOPackage> &p )
{
	PrlHandleServerJobPtr pJob = PrlHandleServerJob::GetJobByUuid( Uuid::toString( p->header.parentUuid ) );
	PrlRunProgramInGuestJob *pRunProgramJob = dynamic_cast<PrlRunProgramInGuestJob *>( pJob.getHandle() );
	if ( pRunProgramJob )
	{
		UnregisterJob( pRunProgramJob );
		PrlHandleVmPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVm>( pRunProgramJob->GetVmHandle() );
		PRL_ASSERT( pVm.getHandle() );
		if ( pVm.getHandle() )
		{
			SmartPtr<IOPackage> pResponse = IOPackage::createInstance( PET_IO_CLIENT_PROCESSED_ALL_DESCS_DATA, 0);
			Uuid_t parentUuid;
			Uuid::dump( pRunProgramJob->GetJobUuid(), parentUuid );
			::memcpy( pResponse->header.parentUuid,	parentUuid, sizeof(Uuid_t) );
			pVm->GetIOChannel()->sendPackage(pResponse);
		}
	}
}

#ifdef _WIN_
#define PackTermSettings(qb) do {} while (0)
#else
static void PackTermSettings(QByteArray &qb)
{
			struct termios tios;
			struct winsize ws;
			char *ptr_p_tios;

			tcgetattr(0, &tios);
#if defined(_LIN_)
			ptr_p_tios = (char *)&tios;
#endif
			ioctl(0, TIOCGWINSZ, &ws);
			qb.append(QByteArray(ptr_p_tios,
				sizeof(struct termios)));
			qb.append(QByteArray((char *)&ws, sizeof(struct winsize)));
}
#endif /* _WIN_ */

void ProcessTtyPackage( const SmartPtr<IOPackage> &p )
{
	PrlHandleServerJobPtr pJob = PrlHandleServerJob::GetJobByUuid( Uuid::toString( p->header.parentUuid ) );
	PrlRunProgramInGuestJob *pRunProgramJob = dynamic_cast<PrlRunProgramInGuestJob *>( pJob.getHandle() );
	if ( pRunProgramJob)
	{
		PrlHandleVmPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVm>( pRunProgramJob->GetVmHandle() );
		PRL_ASSERT( pVm.getHandle() );
		if ( pVm.getHandle() )
		{
			QByteArray _buffer;
			PackTermSettings(_buffer);
			pRunProgramJob->MakeTerminalRaw(PRL_TRUE);
			SmartPtr<IOPackage> p = IOPackage::createInstance(PET_IO_TTY_SETTINGS_PROVIDED, 1);
			p->fillBuffer(0, IOPackage::RawEncoding, _buffer.constData(), _buffer.size() );
			Uuid_t parentUuid;
			Uuid::dump( pRunProgramJob->GetJobUuid(), parentUuid );
			::memcpy( p->header.parentUuid,	parentUuid, sizeof(Uuid_t) );
			pVm->GetIOChannel()->sendPackage(p);
		}
	}
}

}//namespace PrlFileDescriptorsMech

