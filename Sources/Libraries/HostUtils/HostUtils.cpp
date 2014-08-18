/*
 * HostUtils.cpp: Some useful functions collection
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


#include <QFSFileEngine>
#include <QMutexLocker>
#include <QTemporaryFile>
#include <QUrl>
#include <QStringList>

#include <Interfaces/ParallelsPlatform.h>
#include <Interfaces/ParallelsQt.h>
#include "Libraries/HostUtils/HostUtils.h"
#include <Libraries/HostUtils/PCSUtils.h>
#include "Libraries/Logging/Logging.h"
#include "Libraries/PrlUuid/Uuid.h"
#include "Libraries/Std/PrlAssert.h"
#include "Libraries/Std/PrlTime.h"
#if defined(_WIN_)
#include <Libraries/HostUtils/UserProfiles_win.h>
#endif

#if defined(_LIN_)
	#include <sys/stat.h>
	#include <sys/statfs.h>
	#include <errno.h>
	#include <malloc.h>
	#include <stdlib.h>
	#include <sys/types.h>
	#include <fcntl.h>
	#include <unistd.h>
#else
	#include <sys/stat.h>
#endif

#ifdef _LIN_
	#define ADFS_SUPER_MAGIC		0xadf5
	#define AFFS_SUPER_MAGIC		0xadff
	#define AFS_SUPER_MAGIC         0x5346414F
	#define AUTOFS_SUPER_MAGIC		0x0187
	#define CODA_SUPER_MAGIC		0x73757245
	#define EFS_SUPER_MAGIC			0x414A53
	#define EXT_SUPER_MAGIC			0xEF53
	#define HPFS_SUPER_MAGIC		0xf995e849
	#define ISOFS_SUPER_MAGIC		0x9660
	#define JFFS2_SUPER_MAGIC		0x72b6
	#define MSDOS_SUPER_MAGIC		0x4d44
	#define NFS_SUPER_MAGIC			0x6969
	#define PROC_SUPER_MAGIC		0x9fa0
	#define QNX4_SUPER_MAGIC		0x002f
	#define REISERFS_SUPER_MAGIC	0x52654973
	#define SMB_SUPER_MAGIC			0x517B
	#define GFS_SUPER_MAGIC			0x01161970
	#define FUSE_SUPER_MAGIC		0x65735546
#endif

#define RESOLVE_BUF_SIZE	2048
#define PROCESS_FINISHED_TIMEOUT	60000	// 60 seconds

#if !defined(_WIN_)
#include <sys/types.h>
#include <sys/time.h>
#endif

#if defined(_WIN_)
// Definitions required for  HostUtils::GetAtaDriveRate
#include "WinIoctl.h"
#include "ntddscsi.h"
#include <TlHelp32.h>
#endif

/**
 * @function Function checks is specified file device or a standard file.
 * @brief Function checks is specified file device or a standard file.
 *
 * @param File name, to discover
 *
 * @return If specified file is a device, function returns true, else false
 *
 * @author antonz@
 */
bool HostUtils::IsDevice(const QString& fileName)
{
  bool bRetVal = false;
#if defined _WIN_
  // Just check file name
  if (fileName.left(4) == QString("\\\\.\\"))
	bRetVal = true;
#elif defined _LIN_
  struct stat StatBuf;
  char* sBuffer;

  // Allocate buffer
  sBuffer = (char*)malloc(RESOLVE_BUF_SIZE * 2);

  if (!sBuffer)
	return false;

  if (fileName.length())
	{
	  // Return value for readlink
	  int iRLRet = 0;
	  // Pointers to buffer
	  char* Buffer1 = sBuffer;
	  char* Buffer2 = &sBuffer[RESOLVE_BUF_SIZE];

	  // resolve link
	  strncpy(Buffer1, fileName.toUtf8().data(), RESOLVE_BUF_SIZE);
	  memset(Buffer2, 0, RESOLVE_BUF_SIZE);

	  while((iRLRet = readlink(Buffer1, Buffer2, RESOLVE_BUF_SIZE)) >= 0)
	{
	  // swap buffers (valid path is always in Buffer1)
	  char* tmp = Buffer2;
	  Buffer2 = Buffer1;
	  Buffer1 = tmp;
	  // Clean target buffer
	  memset(Buffer2, 0, RESOLVE_BUF_SIZE);
	}

	  // EINVAL - file name is not a symbolic link
	  if (errno == EINVAL)
	{
	  // Check is it a device ?
	  stat(fileName.toUtf8().data(), &StatBuf);
	  bRetVal = (S_ISBLK(StatBuf.st_mode) || S_ISCHR(StatBuf.st_mode));
	}
	}

  free(sBuffer);
#endif
  return bRetVal;
}


/*
 * @function Check is file a valid floppy image
 * @brief Check is file a valid floppy image
 *
 * @param File name, to discover
 *
 * @return If specified file is a valid floppy image, function returns true, else false
 *
 * @author antonz@
 */
bool HostUtils::IsFddImage(const QString& fileName)
{
	QFileInfo fi(fileName);
	quint64 size = fi.size();

	// Check for the size, taking into consideration that
	// some image creators add their signature after main floppy image body.
	// For example, once popular DiskImage 2.01 adds -
	// "DiskImage 2.01 (C) 1990,1991 Digital Research Inc." (overall 418 bytes).
	//
	// To fix the issue we soften the check:
	// (1,474,560) <= X <= (1,474,560 + 1024)

	return (CFG_144_IMAGE_SIZE <= size) && (size <= CFG_144_IMAGE_SIZE + 1024);
}


/**
 * @function Check is file a valid CD image
 * @brief Check is file a valid CD image
 *
 * @param File name, to discover
 *
 * @return If specified file is a valid CD image, function returns true, else false
 *
 * @author antonz@
 * Check is file a valid CD image
 */
bool HostUtils::IsCdImage(const QString& fileName)
{
	Q_UNUSED(fileName);
	return false;
}


/**
 * @function Get last error value. Host independent.
 * @brief Get last error value. Host independent.
 *
 * @return Error code (specific to OS)
 *
 * @author antonz@
 */
UINT HostUtils::GetLastError()
{
#ifdef _WIN_
	return ::GetLastError();
#else
	return errno;
#endif
}


/**
 * Set last error value
 *
 * @author maximk
 */
void HostUtils::SetLastError( UINT err )
{
#ifdef _WIN_
	::SetLastError( err );
#else
	errno = err;
#endif
}

/**
 * @function Set size(end) of file
 * @brief Set size(end) of file, enlarges or crops file.
 * Sets file pointer to the end of file.
 *
 * @param File handle
 * @param New file size
 *
 * @return File size
 *
 * @author artemk@
 */
bool HostUtils::SetFileSize(
#ifdef _WIN_
	HANDLE hFile,
#else
	int hFile,
#endif
	UINT64 uiSize
	)
{
#ifdef _WIN_
	LARGE_INTEGER liSize;
	liSize.QuadPart = uiSize;

	DWORD dwLowPart = ::SetFilePointer(hFile, liSize.LowPart, (PLONG)&liSize.HighPart, FILE_BEGIN);
	if (INVALID_SET_FILE_POINTER == dwLowPart && ::GetLastError() != NO_ERROR)
		return false;

	if (!::SetEndOfFile(hFile))
#else
	if (ftruncate(hFile, uiSize) < 0)
#endif
	{
		return false;
	}

	return true;
}


/**
 * Get filesystem type
 *
 * @param File/path name
 *
 * @return Name of the filesystem
 *
 * @author antonz@
 */
PRL_FILE_SYSTEM_FS_TYPE HostUtils::GetFSType(const QString& fileName)
{
#if defined _LIN_
	return GetFSType_Lin(fileName);
#elif defined _WIN_
	return GetFSType_Win(fileName);
#endif
}

#ifdef _WIN_
/*
 * Windows specific part to get mount point
 */
static wchar_t* GetVolumeMount(const QString& fileName)
{
	DWORD bufSize = 0;
	// Pointer to the volume mount point
	wchar_t* Buffer = NULL;
	// File name (not used)
	wchar_t** lpFileName = NULL;

	// Get buffer size
	bufSize = GetFullPathNameW(
				fileName.utf16(),
				bufSize,
				Buffer,
				lpFileName);

	if (!bufSize)
	{
		WRITE_TRACE(DBG_FATAL, "getFullPathName for %s failed %u",
					QSTR2UTF8(fileName), GetLastError());
		return NULL;
	}

	// + ending '\0'
	bufSize += 1;
	Buffer = (wchar_t*)malloc(bufSize * sizeof(wchar_t));

	if (!Buffer)
	{
		WRITE_TRACE(DBG_FATAL, "memory allocation failed");
		return NULL;
	}

	/*
	 * Volume mount point length is smaller than full path.
	 * They wrote this in MSDN. No any other way to get Volume mount point size
	 */

	// Get mount point
	if (GetVolumePathNameW(fileName.utf16(), Buffer, bufSize))
		return Buffer;

	WRITE_TRACE(DBG_FATAL, "getVolumePathName with allocated buffer for %s failed %u",
			fileName.toUtf8().data(), GetLastError());

	free(Buffer);
	return NULL;
}
#endif

/**
 * Get filesystem type (win version)
 *
 * @param File/path name
 *
 * @return Name of the filesystem
 *
 * @author antonz@
 */
PRL_FILE_SYSTEM_FS_TYPE HostUtils::GetFSType_Win(const QString& fileName)
{
	// Make compiler happy
	(void)fileName;

#ifdef _WIN_
	// Filesystems types (as Mac OS X provides them)
	const wchar_t fsNTFS[] = L"NTFS";
	const wchar_t fsFAT[] = L"FAT";
	const wchar_t fsFAT32[] = L"FAT32";

	// flags that describe the file system
	DWORD dwSysFlags;
	// File system name
	wchar_t wFileSysNameBuf[FS_NAME_SIZE];

	wchar_t* VolMP = GetVolumeMount(fileName);

	if (!VolMP)
		return PRL_FS_INVALID;

	// Get filesystem name
	bool Res = GetVolumeInformationW(VolMP, NULL, 0, NULL, NULL,
									&dwSysFlags, wFileSysNameBuf,
									FS_NAME_SIZE);

	free(VolMP);

	// Skip checks
	if (!Res)
	{
		WRITE_TRACE(DBG_FATAL, "getVolumePathName with allocated buffer for %s failed %u",
				fileName.toUtf8().data(), GetLastError());
		return PRL_FS_INVALID;
	}

	if ( UTF16_2QSTR( wFileSysNameBuf) == UTF16_2QSTR( fsNTFS ) )
		return PRL_FS_NTFS;

	if ( UTF16_2QSTR(wFileSysNameBuf) == UTF16_2QSTR( fsFAT ) )
		return PRL_FS_FAT;

	if ( UTF16_2QSTR( wFileSysNameBuf ) == UTF16_2QSTR( fsFAT32 ) )
		return PRL_FS_FAT32;

#endif
	return PRL_FS_INVALID;
}

/**
 * Get filesystem type (linux version)
 *
 * @param File/path name
 *
 * @return Name of the filesystem
 *
 * @author antonz@
 */
PRL_FILE_SYSTEM_FS_TYPE HostUtils::GetFSType_Lin(const QString& fileName)
{
	PRL_FILE_SYSTEM_FS_TYPE retType = PRL_FS_INVALID;
	// Make compiler happy
	(void)fileName;
#ifdef _LIN_
	// Structure to receive fs info
	struct statfs FSStat;

	memset(&FSStat, 0, sizeof(struct statfs));

	if (statfs(fileName.toUtf8().data(), &FSStat) < 0)
	{
		WRITE_TRACE(DBG_FATAL, "statfs() failed %u", errno);
		return retType;
	}

	switch(FSStat.f_type)
	{
	case ADFS_SUPER_MAGIC:
		retType = PRL_FS_ADFS;
		break;
	case AFFS_SUPER_MAGIC:
		retType = PRL_FS_AFFS;
		break;
	case AFS_SUPER_MAGIC:
		retType = PRL_FS_AFS;
		break;
	case AUTOFS_SUPER_MAGIC:
		retType = PRL_FS_AUTOFS;
		break;
	case CODA_SUPER_MAGIC:
		retType = PRL_FS_CODA;
		break;
	case EFS_SUPER_MAGIC:
		retType = PRL_FS_EFS;
		break;
	case EXT_SUPER_MAGIC:
		retType = PRL_FS_EXTFS;
		break;
	case HPFS_SUPER_MAGIC:
		retType = PRL_FS_HPFS;
		break;
	case ISOFS_SUPER_MAGIC:
		retType = PRL_FS_ISOFS;
		break;
	case JFFS2_SUPER_MAGIC:
		retType = PRL_FS_JFFS2;
		break;
	case MSDOS_SUPER_MAGIC:
		retType = PRL_FS_FAT32;
		break;
	case NFS_SUPER_MAGIC:
		retType = PRL_FS_NFS;
		break;
	case QNX4_SUPER_MAGIC:
		retType = PRL_FS_QNX4;
		break;
	case REISERFS_SUPER_MAGIC:
		retType = PRL_FS_REISERFS;
		break;
	case SMB_SUPER_MAGIC:
		retType = PRL_FS_SMBFS;
		break;
	case GFS_SUPER_MAGIC:
		retType = PRL_FS_GFS;
		break;
	case FUSE_SUPER_MAGIC:
		retType = PRL_FS_FUSE;
		break;
	}
#endif

	return retType;
}

/**
 * Get maximum file size for given file name
 *
 * @param File name
 *
 * @return Maximum file size
 *
 * @author antonz@
 */
PRL_UINT64 HostUtils::GetMaxFileSize(const QString& fileName)
{
	// Let return maximum value for unknown systems
	PRL_UINT64 uiRetVal = (PRL_UINT64)-1;

	switch(GetFSType(fileName))
	{
	case PRL_FS_FAT:
	case PRL_FS_FAT32:
		// Fat file size limit is 2 GB
		uiRetVal = FS_FAT_FILE_SIZE_LIMIT;
	default:
		break;
	}

	return uiRetVal;
}

/**
 * Get free space on disk where the file lays
 *
 * @param File name
 *
 * @author antonz@
 */
PRL_UINT64 HostUtils::GetFreeSpaceByName(const QString& fileName)
{
	// Is physical device? No checks!
#if defined _LIN_
	if (fileName.startsWith("/dev/", Qt::CaseInsensitive))
		return (PRL_UINT64)-1;
#elif defined _WIN_
	if ((fileName.startsWith("//") || fileName.startsWith("\\\\")))
		return (PRL_UINT64)-1;
#endif

	// In case of PCS statfs() causes requests to MDS, which may flood it
	// in case of thousands of VMs, so do not use it.
	if (pcs_fs(QSTR2UTF8(fileName)))
		return (PRL_UINT64)-1;

#if defined _LIN_
	struct statfs64 StatFs;

	if (statfs64(QSTR2UTF8(fileName), &StatFs))
		return (PRL_UINT64)-1;

	return (PRL_UINT64)StatFs.f_bavail * (PRL_UINT64)StatFs.f_bsize;
#elif defined _WIN_
	PRL_UINT64 uiSize;

	// File info to simplify operations
	QFileInfo fInfo(fileName);

	if (!GetDiskFreeSpaceEx((TCHAR*)fInfo.absolutePath().constData(),
				(PULARGE_INTEGER)&uiSize, NULL, NULL)) {
		WRITE_TRACE(DBG_FATAL, "Failed to GetDiskFreeSpaceEx %s %x",
					QSTR2UTF8(fileName), GetLastError());
		return (PRL_UINT64)-1;
	}

	LOG_MESSAGE(DBG_DEBUG, "Free space: Avail %llu", uiSize);
	return uiSize;
#else
#error Undefined GetFreeSpaceByName implementation
#endif
}

static QString GatherCmdListOutput(const QStringList& cmdList)
{
	QString output;

	Q_FOREACH (const QString& cmd, cmdList)
	{
		QString cmdOutput;

		bool bRes = HostUtils::RunCmdLineUtility(cmd, cmdOutput, PROCESS_FINISHED_TIMEOUT);
		output += QString("\n======= %1%2 =======\n").arg(!bRes ? "FAILED: " : "").arg( cmd ) ;
		output += cmdOutput;
	}

	return output;
}

QString HostUtils::GetAllProcesses(bool bDetailed)
{
	QStringList cmdList;

#ifdef _WIN_
	cmdList << "tasklist /V";
	cmdList << "tasklist /SVC";
	cmdList << "sc query type= service state= all";
	if (bDetailed)
		cmdList << "wmic path win32_process";
#elif _LIN_
	(void) bDetailed;
	cmdList << "ps auxf";
#else
	cmdList << "ps aux";
	cmdList << "top -i 1 -l 3 -o cpu -S";
	if (bDetailed)
		cmdList << "lsof";
#endif

	return GatherCmdListOutput(cmdList);
}

QStringList HostUtils::GetProcessPidsByName(const QString& procName)
{
	Q_UNUSED(procName);
	QStringList pids;
#if defined _WIN_

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (INVALID_HANDLE_VALUE != snapshot)
	{
		PROCESSENTRY32W entry = {};
		entry.dwSize = sizeof(entry);
		for (BOOL bErr = Process32NextW(snapshot, &entry); bErr; bErr = Process32NextW(snapshot, &entry))
		{
			if (_wcsicmp((const wchar_t*)entry.szExeFile, (const wchar_t*)QSTR2UTF16(procName)))
				continue;
			pids << QString("%1").arg(entry.th32ProcessID);
		}
		CloseHandle(snapshot);
	}
	else
		WRITE_TRACE(DBG_FATAL, "Error : Failed to enum processes. Error code : %lu", GetLastError());

#endif
	return pids;
}

QString HostUtils::GetMoreHostInfo()
{
	QProcess proc;
	QString strResult;
#if _LIN_
	// TODO: Add here Linux specific logic
	QProcess uNameProc;
	uNameProc.start("uname -a");
	uNameProc.waitForFinished( PROCESS_FINISHED_TIMEOUT );
	strResult = UTF8_2QSTR( uNameProc.readAllStandardOutput() ) + "\n";
	proc.start("lsb_release -a");
#elif _WIN_
	proc.start("systeminfo");
#endif
	proc.waitForFinished( PROCESS_FINISHED_TIMEOUT );
	strResult += UTF8_2QSTR( proc.readAllStandardOutput() );
	return strResult;
}

QString HostUtils::GetMountInfo()
{
	QString qsOutput;
#ifndef _WIN_
	if ( ! RunCmdLineUtility("mount", qsOutput) )
		return "";
#endif
	return qsOutput;
}

/**
 * Flush file buffers for QFile
 *
 * @param QFile object (opened)
 *
 * @return true if operation succeed
 */
bool HostUtils::FlushQFile(QFile& file)
{
	bool bRet = false;

	if (file.isOpen())
	{
		file.flush();

		// Flush file caches
#if defined _WIN_
		bRet = FlushFileBuffers((HANDLE)file.handle());
#elif defined _LIN_
		bRet = fsync(file.handle()) < 0;
#endif
	}

	return bRet;
}

#if defined(_LIN_)
#include <sched.h>

UINT HostUtils::GetAvailCpu()
{
	cpu_set_t mask;
	if (!sched_getaffinity(0, sizeof(mask), &mask))
	{
		unsigned n = 0;
		for (int i = 0; i < CPU_SETSIZE; ++i)
			if (CPU_ISSET(i, &mask))
				++n;
		return n;
	} else {
		WRITE_TRACE(DBG_FATAL,
			"get_avail_cpu() failed to query affinity %s",
			strerror(errno));
		return 1;
	}
}
#elif defined(_WIN_)
#include <windows.h>

UINT HostUtils::GetAvailCpu()
{
	ULONG_PTR proc_mask = 0, sys_mask = 0;
	if (GetProcessAffinityMask(GetCurrentProcess(), &proc_mask, &sys_mask))
	{
		int n = 0;
		for (ULONG_PTR bit = 1; bit; bit <<= 1)
			if (proc_mask & bit)
				++n;
		return n;
	} else {
		WRITE_TRACE(DBG_FATAL, "GetProcessAffinityMask() failed: %u", GetLastError());
		return 1;
	}
}
#endif

#if defined(_MSC_VER)
	extern "C" void cpuid(UINT eax, UINT ecx, UINT *res);
#endif

void HostUtils::GetCpuid(UINT& uEAX, UINT& uECX, UINT& uEDX, UINT& uEBX)
{
#if defined(_MSC_VER)

	UINT res[4];

	cpuid(uEAX, uECX, (UINT*)res);

	uEAX = res[0];
	uECX = res[1];
	uEDX = res[2];
	uEBX = res[3];

#elif defined(__GNUC__)

#ifdef _X86_
	__asm__ __volatile__(
		"pushl %%ebx\n"		// EBX can't be obtained directly due to PIC
		"cpuid\n"			// generation problems
		"movl %%ebx, %3\n"
		"popl %%ebx\n"
		: "=a" (uEAX), "=c" (uECX), "=d" (uEDX), "=S" (uEBX)
		: "0" (uEAX), "1" (uECX));
#else
	__asm__ __volatile__(
		"cpuid\n"
		: "=a" (uEAX), "=c" (uECX), "=d" (uEDX), "=b" (uEBX)
		: "0" (uEAX), "1" (uECX));

#endif

#else
	#error "Unknown compiler"
#endif
}

UINT HostUtils::GetCpuidEax(UINT uLevel, UINT uSubLeaf)
{
	UINT uEAX = uLevel, uECX = uSubLeaf, uEDX, uEBX;
	GetCpuid(uEAX, uECX, uEDX, uEBX);
	return uEAX;
}

UINT HostUtils::GetCpuidEcx(UINT uLevel, UINT uSubLeaf)
{
	UINT uEAX = uLevel, uECX = uSubLeaf, uEDX, uEBX;
	GetCpuid(uEAX, uECX, uEDX, uEBX);
	return uECX;
}

UINT HostUtils::GetCpuidEdx(UINT uLevel, UINT uSubLeaf)
{
	UINT uEAX = uLevel, uECX = uSubLeaf, uEDX, uEBX;
	GetCpuid(uEAX, uECX, uEDX, uEBX);
	return uEDX;
}

UINT HostUtils::GetCpuidEbx(UINT uLevel, UINT uSubLeaf)
{
	UINT uEAX = uLevel, uECX = uSubLeaf, uEDX, uEBX;
	GetCpuid(uEAX, uECX, uEDX, uEBX);
	return uEBX;
}

UINT HostUtils::GetCurrentRealCpuId()
{
	return GetCpuidEbx(1) >> 24;
}

#define MHZ 1000000


UINT HostUtils::GetCPUMhzByTscOne()
{
	UINT64 tsc_start, tsc_end;
	UINT64 tv_start, tv_end, tv_now, usec_delay;
	UINT cpuid, i = 0;

#define CPU_DELAY_MSEC	20	//=0.02sec
#define CPU_LOOP_MAX	50

	do
	{
		cpuid = GetCurrentRealCpuId();
		tsc_start = GetTsc();
		tv_start = PrlGetTickCount();
		// loop until TickCount will be changed to be sure we start from the tick's beginning
		tv_end = tv_start;
		while (tv_start == tv_end)
			tv_start = PrlGetTickCount();

		tv_end = tv_start + CPU_DELAY_MSEC;

		// wait cycle
		do
		{
			tv_now = PrlGetTickCount();
			tsc_end = GetTsc();
		} while (tv_now < tv_end);

	} while ((cpuid != GetCurrentRealCpuId()) && (++i < CPU_LOOP_MAX));

	if (i == CPU_LOOP_MAX)
	{
		WRITE_TRACE(DBG_WARNING, "Can't calibrate TSC for %u iterations", CPU_LOOP_MAX);
		return 0;
	}

	usec_delay = 1000L * (tv_now - tv_start);

	return ((tsc_end - tsc_start) / usec_delay);
}

UINT HostUtils::GetCPUMhzByTsc()
{
#define FREQ_SAMPLES 3
	int samples = FREQ_SAMPLES;
	UINT freq_mhz_cur, freq_mhz = GetCPUMhzByTscOne();
	while (--samples > 0)
	{
		freq_mhz_cur = GetCPUMhzByTscOne();
		if (freq_mhz_cur > 0)
			freq_mhz = MIN(freq_mhz, freq_mhz_cur);
	}
	return freq_mhz;
}

#ifdef _WIN_
UINT HostUtils::GetHostCPUMhz()
{
	LONG	lResult;
	DWORD	dwMhz, dwLength;
	HKEY	hkProcessor;

	lResult = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
		0,
		KEY_QUERY_VALUE,
		&hkProcessor);
	if (lResult != ERROR_SUCCESS)
	{
		LOG_MESSAGE(DBG_FATAL, "RegOpenKeyEx() failed, rc=%d", lResult);
		return 0;
	}
	dwLength = sizeof(dwMhz);
	lResult = RegQueryValueEx(
		hkProcessor,
		L"~MHz",
		NULL,
		NULL,
		(LPBYTE) &dwMhz,
		&dwLength);
	if (lResult != ERROR_SUCCESS)
	{
		LOG_MESSAGE(DBG_FATAL, "RegQueryValueEx() failed, rc=%d", lResult);
		return 0;
	}
	RegCloseKey(hkProcessor);
	return dwMhz;
}

#endif // _WIN_

#ifdef _LIN_
UINT HostUtils::GetHostCPUMhz()
{
	UINT nFreq = 0;
	const char *file_freq_name = "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq";
	QFile file_freq(file_freq_name);
	if (file_freq.open(QIODevice::ReadOnly))
	{
		QString qsFreq = file_freq.readAll();
		file_freq.close();

		nFreq = qsFreq.toUInt() / 1000;
	}

	if (nFreq == 0)
		WRITE_TRACE(DBG_FATAL, "can't get CPU freq from %s", file_freq_name);

	return nFreq;
}
#endif // _LIN_

UINT HostUtils::GetCPUMhz()
{
	UINT freq, freq_tsc;

	freq = GetHostCPUMhz();
	freq_tsc = GetCPUMhzByTsc();

	if (freq == 0)
	{
		WRITE_TRACE(DBG_WARNING, "Use CPU freq obtained via TSC calibration: %u MHz", freq_tsc);
		return freq_tsc;
	}

	if ((UINT)abs((int)(freq - freq_tsc)) > freq/10)
		WRITE_TRACE(DBG_WARNING, "CPU freq difference is more than 10%%: freq=%u freq_tsc=%u", freq, freq_tsc);

	WRITE_TRACE(DBG_INFO, "CPU frequency: %u MHz", freq);

	return freq;
}

UINT HostUtils::GetBusMhz()
{
	return 333;
}

PRL_RESULT HostUtils::CopyAccessRights(const QString& oldFile, const QString& newFile)
{
#ifdef _WIN_
	DWORD bytesNeeded = 0;
	PRL_UINT8_PTR pData = NULL;

	// Get size of SID (it will fail anyway)
	GetFileSecurity(oldFile.utf16(),
					DACL_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION |
					OWNER_SECURITY_INFORMATION,
					(PSECURITY_DESCRIPTOR)pData, 0, &bytesNeeded);

	// Allocate memory for SID
	pData = (PRL_UINT8_PTR)malloc(bytesNeeded);

	if (!pData)
	{
		WRITE_TRACE(DBG_FATAL, "Error allocating memory %u", bytesNeeded);
		return PRL_ERR_OUT_OF_MEMORY;
	}

	// Get it
	if (!GetFileSecurity(oldFile.utf16(),
						DACL_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION |
						OWNER_SECURITY_INFORMATION,
						(PSECURITY_DESCRIPTOR)pData, bytesNeeded, &bytesNeeded))
	{
		WRITE_TRACE(DBG_FATAL, "GetFileSecurity failed [%u] for %s", HostUtils::GetLastError(),
														  oldFile.toUtf8().data());
		free(pData);
		return PRL_ERR_FAILURE;
	}

	// Set SID
	if (!SetFileSecurity(newFile.utf16(),
						DACL_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION |
						OWNER_SECURITY_INFORMATION,
						(PSECURITY_DESCRIPTOR)pData))
	{
		WRITE_TRACE(DBG_FATAL, "SetFileSecurity failed [%u] for %s", HostUtils::GetLastError(),
														  newFile.toUtf8().data());
		free(pData);
		return PRL_ERR_FAILURE;
	}

	free(pData);
#else
	// File info
	QFileInfo fInfo(oldFile);
	// Access modes
	PRL_UINT32 uiOwnerID, uiGroupID;
	// Permissions
	QFile::Permissions Perms;

	uiOwnerID = fInfo.ownerId();
	uiGroupID = fInfo.groupId();
	Perms = fInfo.permissions();

	// Now we need to set it to the file
	if (!QFile::setPermissions(newFile, Perms))
	{
		WRITE_TRACE(DBG_FATAL, "Error [%u] setting permissions to the file: %s", HostUtils::GetLastError(),
																	  newFile.toUtf8().data());
		return PRL_ERR_FAILURE;
	}

	if (chown(newFile.toUtf8().data(), uiOwnerID, uiGroupID) < 0)
	{
		WRITE_TRACE(DBG_FATAL, "Error [%u] setting UID&GID to the file: %s", HostUtils::GetLastError(),
																  newFile.toUtf8().data());
		return PRL_ERR_FAILURE;
	}
#endif
	return PRL_ERR_SUCCESS;
}

bool HostUtils::RunCmdLineUtility(const QString& qsCmdLine,
								  QString& qsOutput,
								  int nFinishTimeout,
								  QProcess* pProcess,
								  void (*pfnAfterStartCAllback)(QProcess* ))
{
	QProcess proc;
	if ( ! pProcess )
		pProcess = &proc;

	pProcess->start(qsCmdLine);
	if ( ! pProcess->waitForStarted() )
	{
		WRITE_TRACE(DBG_FATAL, "Cannot execute '%s' !", QSTR2UTF8(qsCmdLine));
		return false;
	}

	if ( pfnAfterStartCAllback )
		pfnAfterStartCAllback(pProcess);

	bool res = true;

	// Check that process is not finished: state could change during callback.
	if (pProcess->state() != QProcess::NotRunning)
	{
		res = (nFinishTimeout == 0) ? pProcess->waitForFinished()
			: pProcess->waitForFinished(nFinishTimeout);
	}

	// read stdout regardless of exit code, because the process could have had
	// printed something before timeout/crash/error
	qsOutput = UTF8_2QSTR(pProcess->readAllStandardOutput());

	if ( !res )
	{
		WRITE_TRACE(DBG_FATAL, "Timeout at finish '%s' !", QSTR2UTF8(qsCmdLine));
		return false;
	}

	if (pProcess->exitStatus() == QProcess::CrashExit)
	{
		WRITE_TRACE(DBG_FATAL, "The '%s' was crashed !", QSTR2UTF8(qsCmdLine));
		return false;
	}

	if ( pProcess->exitCode() )
	{
		WRITE_TRACE(DBG_FATAL, "The '%s' returned exit code: '%d' !",
						QSTR2UTF8(qsCmdLine), pProcess->exitCode());

		QByteArray baErr = pProcess->readAllStandardError();
		if ( ! baErr.isEmpty() )
			WRITE_TRACE(DBG_FATAL, "The '%s' returned with error: '%s' !",
							QSTR2UTF8(qsCmdLine), baErr.constData());

		return false;
	}

	return true;
}

/*
 * Check is the file path absolute or relative
 */
bool HostUtils::isPathAbsolute(const QString& path)
{
	return !QFSFileEngine(path).isRelativePath();
}

#ifdef _WIN_
QString GetWindowsVersionName()
{
	QString strOsName;

	// Check if the host operating system is Windows XP.
	OSVERSIONINFOEX osVersionInfo;

	ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFOEX));
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if ( ! GetVersionEx((OSVERSIONINFO*) &osVersionInfo ) )
		return strOsName;

	switch ( osVersionInfo.dwPlatformId )
	{
		case VER_PLATFORM_WIN32_WINDOWS:
		{
			if ( 4 == osVersionInfo.dwMajorVersion && 0 == osVersionInfo.dwMinorVersion )
				strOsName = "Windows 95";
			else if ( 4 == osVersionInfo.dwMajorVersion && 10 == osVersionInfo.dwMinorVersion )
				strOsName = "Windows 98";
			else if ( 4 == osVersionInfo.dwMajorVersion && 90 == osVersionInfo.dwMinorVersion )
				strOsName = "Windows Me";

			break;
		}
		case VER_PLATFORM_WIN32_NT:
		{
			if ( osVersionInfo.dwMajorVersion <= 4 )
				strOsName = "Windows NT";
			else if ( 5 == osVersionInfo.dwMajorVersion && 0 == osVersionInfo.dwMinorVersion )
				strOsName = "Windows 2000";
			else if ( ( 5 == osVersionInfo.dwMajorVersion && 1 == osVersionInfo.dwMinorVersion )
				  || ( 5 == osVersionInfo.dwMajorVersion && 2 == osVersionInfo.dwMinorVersion
					   && VER_NT_WORKSTATION == osVersionInfo.wProductType ) )
			{
				strOsName = "Windows XP";
			}
			else if ( 5 == osVersionInfo.dwMajorVersion && 2 == osVersionInfo.dwMinorVersion
				  && VER_NT_WORKSTATION != osVersionInfo.wProductType )
			{
				strOsName = "Windows Server 2003";
			}
			else if ( 6 == osVersionInfo.dwMajorVersion && 0 == osVersionInfo.dwMinorVersion
				  && VER_NT_WORKSTATION == osVersionInfo.wProductType )
				strOsName = "Windows Vista";
			else if ( 6 == osVersionInfo.dwMajorVersion && 0 == osVersionInfo.dwMinorVersion
					&& VER_NT_WORKSTATION != osVersionInfo.wProductType )
			{
				strOsName = "Windows Server 2008";
			}
			else if ( 6 == osVersionInfo.dwMajorVersion && 1 == osVersionInfo.dwMinorVersion
					&& VER_NT_WORKSTATION != osVersionInfo.wProductType )
			{
				strOsName = "Windows Server 2008 R2";
			}
			else if ( 6 == osVersionInfo.dwMajorVersion && 1 == osVersionInfo.dwMinorVersion
					&& VER_NT_WORKSTATION == osVersionInfo.wProductType )
			{
				strOsName = "Windows 7";
			}
			break;
		}
	}

	return strOsName;
}
#endif // _WIN_

QString HostUtils::GetOsVersionName()
{
#if defined(_WIN_)
	return GetWindowsVersionName();
#elif defined(_LIN_)
	return "Linux";
#endif
	return QString();
}

uint HostUtils::GetRecommendedMaxVmMemory( uint hostMemory )
{
	static QMutex mutex;
	static bool bInit = false;
	static QMap<int, int> g_maxRecMemTable;

	if ( ! bInit )
	{
		mutex.lock();
		if ( ! bInit )
		{
			g_maxRecMemTable.insert( 1024, 512 );
			g_maxRecMemTable.insert( 1536, 512 );
			g_maxRecMemTable.insert( 2048, 512 );
			g_maxRecMemTable.insert( 3072, 1024 );
			g_maxRecMemTable.insert( 4096, 1536 );
			g_maxRecMemTable.insert( 6144, 2048 );
			g_maxRecMemTable.insert( 8192, 4096 );
			bInit = true;
		}
		mutex.unlock();
	}

	int guestMemory = 0;

	if ( g_maxRecMemTable.contains( hostMemory ) )
	{
		guestMemory = g_maxRecMemTable.value( hostMemory );
	}
	else {
		QMap<int, int>::ConstIterator it = g_maxRecMemTable.lowerBound( hostMemory );
		if ( it == g_maxRecMemTable.begin() )
			guestMemory = hostMemory / 2;
		else if ( it == g_maxRecMemTable.end() )
		{
			int hostMemoryMaxInTable = g_maxRecMemTable.keys().last();
			int guestMemoryMaxInTable = g_maxRecMemTable.values().last();
			int hostMemoryToKeep = hostMemoryMaxInTable - guestMemoryMaxInTable;

			guestMemory = hostMemory - hostMemoryToKeep;
		}
		else
		{
			QMap<int, int>::ConstIterator it0 = it - 1;
			int h0 = it0.key();
			int h1 = it.key();
			int g0 = it0.value();
			int g1 = it.value();
			guestMemory = g0 + (hostMemory - h0) * (g1 - g0) / (h1 - h0);
		}
	}

	return uint(guestMemory) & ~3;
}

QString HostUtils::GetVolumeID(const QString& fileName)
{
	static const QString Undef("Undefined");
#ifdef _WIN_
	wchar_t* Vol = GetVolumeMount(fileName);

	if (!Vol)
		return Undef;

	QString QVol(UTF16_2QSTR(Vol));

	free(Vol);

	return QVol;
#else
	// Structure to receive fs info
	struct stat Stat;

	memset(&Stat, 0, sizeof(struct stat));

	if (stat(QSTR2UTF8(fileName), &Stat) < 0)
	{
		WRITE_TRACE(DBG_FATAL, "stat(%s) at resolving MP failed %u",
					QSTR2UTF8(fileName), errno);
		return Undef;
	}

	return QString::number(Stat.st_dev);
#endif
}

#ifdef _WIN_
QString HostUtils::GetVolumeGUID(const QString& fileName)
{
	/*
	* "A reasonable size for the buffer to accommodate the largest possible
	* volume GUID path is 50 characters." MSDN
	*/
	const int Size = 50 + sizeof(TCHAR); // including terminating "\0"
	TCHAR Buf[Size];  // temporary buffer for volume name
	if (GetVolumeNameForVolumeMountPoint(fileName.utf16(), Buf, Size ))
		 return QString(UTF16_2QSTR(Buf));
	return QString();
}
#else
QString HostUtils::GetVolumeGUID(const QString& fileName)
{
   Q_UNUSED(fileName);
   return  QString();
}
#endif

int HostUtils::GetMemoryUsage(struct HostMemUsage *mu)
{
#if defined( _LIN_ )
	FILE *f;
	char buf[128];
	UINT64 swap_total, swap_free;

	if (!mu)
		return -1;

	memset(mu, 0, sizeof(*mu));

	/* set some default page size */
	mu->pagesize = 4096;

	f = fopen("/proc/meminfo", "rt");
	if (!f)
	{
		WRITE_TRACE(DBG_FATAL, "Failed to open '/proc/meminfo'");
		return -1;
	}

	while (!feof(f))
	{
		if (!fgets(buf, sizeof(buf), f)) break;
		buf[sizeof(buf)-1] = 0;
		sscanf(buf, "MemTotal: %lld kB", &mu->total);
		sscanf(buf, "MemFree: %lld kB", &mu->free);
		sscanf(buf, "Inactive: %lld kB", &mu->inactive);
		sscanf(buf, "Active: %lld kB", &mu->active);
		//no wired memory in linux
		sscanf(buf, "SwapTotal: %lld kB", &swap_total);
		sscanf(buf, "SwapFree: %lld kB", &swap_free);
		sscanf(buf, "Cached: %lld kB", &mu->cached);
	}

	fclose(f);

	mu->swap_used = swap_total - swap_free;

	//convert to bytes
	mu->total *= 1024;
	mu->free *= 1024;
	mu->inactive *= 1024;
	mu->active *= 1024;
	mu->swap_used *= 1024;
	mu->cached *= 1024;

	return 0;
#else
	Q_UNUSED(mu);
	return -1;
#endif
}

#ifdef _WIN_
/**
 * Issue IOCTL_ATA_PASS_THROUGH to device
 * to obtain drive rotation rate.
 */
static PRL_RESULT GetAtaDriveRate_win(Qt::HANDLE devHandle, unsigned  int& rate)
{
	static const int timeout = 60;
	ATA_PASS_THROUGH_EX* Buffer;
	ULONG BufferSize = sizeof(*Buffer) + sizeof(HDD_IDENTIFY);
	ULONG Size;
	PRL_RESULT ret = PRL_ERR_OPERATION_FAILED;

	Buffer = (ATA_PASS_THROUGH_EX*)malloc(BufferSize);
	if (!Buffer)
		return PRL_ERR_OUT_OF_MEMORY;

	memset(Buffer, 0, BufferSize);

	Buffer->Length = sizeof(*Buffer);
	Buffer->AtaFlags = ATA_FLAGS_DRDY_REQUIRED | ATA_FLAGS_DATA_IN;
	Buffer->DataTransferLength = sizeof(HDD_IDENTIFY);
	Buffer->TimeOutValue = timeout;
	Buffer->DataBufferOffset = Buffer->Length;
	Buffer->CurrentTaskFile[0] = 0;
	Buffer->CurrentTaskFile[1] = 1;
	Buffer->CurrentTaskFile[2] = 1;
	Buffer->CurrentTaskFile[6] = ID_CMD;

	if (!DeviceIoControl(devHandle, IOCTL_ATA_PASS_THROUGH,
						Buffer, sizeof(*Buffer),
						Buffer, BufferSize,
						&Size, FALSE))
		goto cleanup;

	HDD_IDENTIFY* id = (HDD_IDENTIFY*)&((CHAR*)Buffer)[Buffer->DataBufferOffset];
	rate = id->wMediaRotationRate;
	ret = PRL_ERR_SUCCESS;

cleanup:
	free(Buffer);
	return ret;
}

static PRL_RESULT GetAtaDriveRate_win(const QString& devName, unsigned  int& rate)
{
	Qt::HANDLE h = CreateFile(QSTR2UTF16(devName), GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
						OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
	if (h == INVALID_HANDLE_VALUE)
		return 0;

	PRL_RESULT  res = GetAtaDriveRate_win(h, rate);
	CloseHandle(h);
	return res;
}
#endif

/**
 * Obtain drive rotation rate.
 * According to ATA spec field 217 contains
 * device rate. For SSD devices this field is equal to 1.
 * Device may not report this data within its IDENTIFY data.
 *
 * @author mihailm
 *
 * @param device name
 *
 * @return obtained device rate
 */
PRL_RESULT HostUtils::GetAtaDriveRate(const QString& devName, unsigned  int& rate)
{
#ifdef _WIN_
	return GetAtaDriveRate_win(devName, rate);
#else
	Q_UNUSED(devName);
	rate = 0;
	return PRL_ERR_UNIMPLEMENTED;
#endif
}


PRL_RESULT HostUtils::GetAtaDriveRate(Qt::HANDLE devHandle, unsigned  int& rate)
{
#ifdef _WIN_
	return GetAtaDriveRate_win(devHandle, rate);
#else
	Q_UNUSED(devHandle);
	rate = 0;
	return PRL_ERR_UNIMPLEMENTED;
#endif
}

bool HostUtils::isFastRebootNodeAllowed()
{
	bool ret = false;

#if defined(_LIN_)
	int fd;
	char buf[10];

	fd = open("/sys/kernel/kexec_loaded", O_RDONLY);
	if (fd < 0)
		return false;

	if (read(fd, buf, sizeof(buf)) <= 0)
	{
		close(fd);
		return false;
	}

	if (!strncmp(buf, "1", 1))
		 ret = true;

	close(fd);
#endif

	return ret;
}

bool HostUtils::MountPram(bool init, QString& point)
{
	QString opts, cmd, out;

	if (!QDir(point).exists())
	{
		if (!QDir().mkpath(point))
			return false;
	}

	if (init)
		opts = "-o noload,pram_name=vm";
	else
		opts = "-o pram_name=vm";

	cmd = "mount ";
	cmd += opts;
	cmd += " -t pram none ";
	cmd += point;

	bool ok = RunCmdLineUtility(cmd, out);
	WRITE_TRACE(DBG_DEBUG, "Pram: run %s, rc=%d", QSTR2UTF8(cmd), ok);

	return ok;
}

bool HostUtils::UMountPram(const QString& point)
{
	QString cmd, out;

	cmd = "umount ";
	cmd += point;

	bool ok = RunCmdLineUtility(cmd, out);
	WRITE_TRACE(DBG_DEBUG, "Pram: run %s, rc=%d", QSTR2UTF8(cmd), ok);

	return ok;
}

bool HostUtils::IsEfi64BootDevice(const QString& mountPoint)
{
	WRITE_TRACE(DBG_DEBUG, "Checking mount point: %s", QSTR2UTF8(mountPoint));
	if (GetFSType(mountPoint) != PRL_FS_FAT32)
		return false;
	WRITE_TRACE(DBG_DEBUG, "Checking mount point. Device has FAT32 FS");
	// check for file efi/boot/bootx64.efi.
	// this checking is manually case insensitive.
	QDir rootDir(mountPoint);
	QString rootEntry;
	QStringList rootList = rootDir.entryList(
			QStringList("[Ee][Ff][Ii]"), QDir::Dirs);
	foreach(rootEntry, rootList)
	{
		WRITE_TRACE(DBG_DEBUG, "rootEntry: %s", QSTR2UTF8(rootEntry));
		QDir efiDir(rootDir.filePath(rootEntry));
		QString efiEntry;
		// efi standard: Efi/Boot/bootx64.efi
		QStringList efiList = efiDir.entryList(
				QStringList("[Bb][Oo][Oo][Tt]"), QDir::Dirs);
		foreach(efiEntry, efiList)
		{
			WRITE_TRACE(DBG_DEBUG, "efiEntry: %s", QSTR2UTF8(efiEntry));
			QDir bootDir(efiDir.filePath(efiEntry));
			QString bootEntry;
			QStringList bootList = bootDir.entryList(QStringList(
						"[Bb][Oo][Oo][Tt][Xx]64.[Ee][Ff][Ii]"), QDir::Files);
			foreach(bootEntry, bootList)
			{
				WRITE_TRACE(DBG_DEBUG, "bootEntry: %s", QSTR2UTF8(bootEntry));
				return true; // one efi-boot file is enough
			}
		}
		// ubuntu specific: EFI/ubuntu/grubx64.efi
		efiList = efiDir.entryList(
				QStringList("[Uu][Bb][Uu][Nn][Tt][Uu]"), QDir::Dirs);
		foreach(efiEntry, efiList)
		{
			WRITE_TRACE(DBG_DEBUG, "efiEntry: %s", QSTR2UTF8(efiEntry));
			QDir bootDir(efiDir.filePath(efiEntry));
			QString bootEntry;
			QStringList bootList = bootDir.entryList(QStringList(
						"[Gg][Rr][Uu][Bb][Xx]64.[Ee][Ff][Ii]"), QDir::Files);
			foreach(bootEntry, bootList)
			{
				WRITE_TRACE(DBG_DEBUG, "bootEntry: %s", QSTR2UTF8(bootEntry));
				return true; // one efi-boot file is sufficient
			}
		}
	}
	return false;
}

QString HostUtils::GetLoadedDrivers()
{
	QString qsOutput;
#if _WIN_
	if ( ! RunCmdLineUtility("sc query type= driver", qsOutput) )
		return "";
#endif
	return qsOutput;
}

#define F_OSXSAVE (1 << 27)
bool HostUtils::ReadXCr0(UINT64* pXCr0)
{
	// Check that XGETBV is supported and enabled
	if(~GetCpuidEcx(1) & F_OSXSAVE)
		return false;

#if defined (_MSC_VER)
#if defined(_AMD64_)
	(void)pXCr0;
	// XGetBV intrinsic is available only starting from VS2010 SP1
	return false;
#else
	__asm
	{
		xor ecx, ecx
		_emit 0x0F  // xgetbv
		_emit 0x01
		_emit 0xD0
		mov ecx, pXCr0
		mov dword ptr [ecx], eax
		mov dword ptr [ecx + 4], edx
	}
#endif
#else
	UINT32 eax, edx;
	__asm__ volatile(
		"xor %%ecx, %%ecx\n"
		".byte 0x0f, 0x01, 0xd0" // xgetbv
		: "=a" (eax), "=d" (edx));
	*pXCr0 = ((UINT64)edx << 32) | (UINT64)eax;
#endif
	return true;
}

