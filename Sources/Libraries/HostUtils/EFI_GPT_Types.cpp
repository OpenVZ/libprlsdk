/*
 * EFI_GPT_Types.cpp: GUID partitions types implementation
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

#include <map>
#include "SDK/Include/PrlErrors.h"
#include "SDK/Include/PrlDisk.h"
#include "EFI_GPT_Types.h"

#define APPEND_PARTITION(Type, MBRType, Uid, Descr)\
			Tmp.OsType = Type;     		  \
			Tmp.MbrType = MBRType;     	  \
			Tmp.Description = Descr;      \
			m_GMap.insert(PartsPair(Uid, Tmp));

////////////////////////////////////////////////////////////////////////
/**
 * GPT managing table (internal class)
 *
 * @author antonz@
 */
////////////////////////////////////////////////////////////////////////
class CGPT
{
private:
	// Define types
	typedef std::pair<Uuid, GPT_DESCR> PartsPair;
	typedef std::map<Uuid, GPT_DESCR> PartsMap;
	typedef std::map<Uuid, GPT_DESCR>::iterator PartsMapIter;
	typedef std::map<Uuid, GPT_DESCR>::const_iterator PartsMapConstIter;
public:
	// Constructor
	CGPT();
	// Destructor
	~CGPT();
	// Get description of partition type by GUID
	QString GetDescriptionByGUID(const Uuid& guid) const;
	// Get description of partition type by GUID
	GPT_OS_TYPE GetTypeByGUID(const Uuid& guid) const;
	// Get list of supported partitions
	QList<QString> GetDataPartitionsList() const;
	// Get data type by GUID
	PRL_UINT8 GetMbrTypeByGUID(const Uuid& guid) const;
private:
    // list
	PartsMap m_GMap;
};


///////////////////////////////////////////////////////////////////////////////
/**
 * GPT transcoder constructor
 *
 * @author antonz@
 */
///////////////////////////////////////////////////////////////////////////////
CGPT::CGPT()
{
	GPT_DESCR	Tmp;

	// Generic
	APPEND_PARTITION(GPT_OS_GENERIC, PRL_INVALID_PARTITION,
											  Uuid("{00000000-0000-0000-0000-000000000000}"),
											  QT_TRANSLATE_NOOP("QObject", "Unused entry"));
	APPEND_PARTITION(GPT_OS_GENERIC, PRL_EFI_PARTITION_GPT,
											  Uuid("{024DEE41-33E7-11D3-9D69-0008C781F39F}"),
 											  QT_TRANSLATE_NOOP("QObject", "MBR partition scheme"));
	APPEND_PARTITION(GPT_OS_GENERIC, PRL_EFI_PARTITION,
											  Uuid("{C12A7328-F81F-11D2-BA4B-00A0C93EC93B}"),
 											  QT_TRANSLATE_NOOP("QObject", "EFI System Partition"));
	// Common
	APPEND_PARTITION(GPT_OS_COMMON, PRL_DATA_PARTITION,
  											  Uuid("{EBD0A0A2-B9E5-4433-87C0-68B6B72699C7}"),
 											  QT_TRANSLATE_NOOP("QObject", "Data partition"));
	// Windows
	APPEND_PARTITION(GPT_OS_WINDOWS, PRL_WINDOWS_PARTITION_NTFS,
											  Uuid("{E3C9E316-0B5C-4DB8-817D-F92DF00215AE}"),
 											  QT_TRANSLATE_NOOP("QObject", "Microsoft Reserved Partition"));
	APPEND_PARTITION(GPT_OS_WINDOWS, PRL_WINDOWS_PARTITION_NTFS,
  											  Uuid("{5808C8AA-7E8F-42E0-85D2-E1E90434CFB3}"),
 											  QT_TRANSLATE_NOOP("QObject",
																 "Logical Disk Manager metadata partition"));
	APPEND_PARTITION(GPT_OS_WINDOWS, PRL_WINDOWS_PARTITION_NTFS,
  											  Uuid("{AF9B60A0-1431-4F62-BC68-3311714A69AD}"),
 											  QT_TRANSLATE_NOOP("QObject", "Logical Disk Manager data partition"));
	// HP-UX
	APPEND_PARTITION(GPT_OS_HPUX, PRL_INVALID_PARTITION,
  											  Uuid("{75894C1E-3AEB-11D3-B7C1-7B03A0000000}"),
 											  QT_TRANSLATE_NOOP("QObject", "Data partition"));
	APPEND_PARTITION(GPT_OS_HPUX, PRL_INVALID_PARTITION,
  											  Uuid("{E2A1E728-32E3-11D6-A682-7B03A0000000}"),
 											  QT_TRANSLATE_NOOP("QObject", "Service Partition"));
	// Linux
	APPEND_PARTITION(GPT_OS_LINUX, PRL_LINUX_PARTITION_DATA,
  											  Uuid("{A19D880F-05FC-4D3B-A006-743F0F84911E}"),
 											  QT_TRANSLATE_NOOP("QObject", "RAID partition"));
	APPEND_PARTITION(GPT_OS_LINUX, PRL_LINUX_PARTITION_SWAP,
  											  Uuid("{0657FD6D-A4AB-43C4-84E5-0933C84B4F4F}"),
 											  QT_TRANSLATE_NOOP("QObject", "Swap partition"));
	APPEND_PARTITION(GPT_OS_LINUX, PRL_LINUX_PARTITION_DATA,
  											  Uuid("{E6D6D379-F507-44C2-A23C-238F2A3DF928}"),
 											  QT_TRANSLATE_NOOP("QObject", "Logical Volume Manager (LVM) partition"));
	APPEND_PARTITION(GPT_OS_LINUX, PRL_LINUX_PARTITION_DATA,
  											  Uuid("{8DA63339-0007-60C0-C436-083AC8230908}"),
 											  QT_TRANSLATE_NOOP("QObject", "Reserved"));
	// Free BSD
	APPEND_PARTITION(GPT_OS_FREEBSD, PRL_INVALID_PARTITION,
  											  Uuid("{516E7CB4-6ECF-11D6-8FF8-00022D09712B}"),
 											  QT_TRANSLATE_NOOP("QObject", "Data partition"));
	APPEND_PARTITION(GPT_OS_FREEBSD, PRL_INVALID_PARTITION,
  											  Uuid("{516E7CB5-6ECF-11D6-8FF8-00022D09712B}"),
 											  QT_TRANSLATE_NOOP("QObject", "Swap partition"));
	APPEND_PARTITION(GPT_OS_FREEBSD, PRL_INVALID_PARTITION,
 											  Uuid("{516E7CB6-6ECF-11D6-8FF8-00022D09712B}"),
 											  QT_TRANSLATE_NOOP("QObject", "Unix File System (UFS) partition"));
	APPEND_PARTITION(GPT_OS_FREEBSD, PRL_INVALID_PARTITION,
  											  Uuid("{516E7CB8-6ECF-11D6-8FF8-00022D09712B}"),
 											  QT_TRANSLATE_NOOP("QObject", "Vinum volume manager partition"));
	// Mac OS X
	APPEND_PARTITION(GPT_OS_MACOSX, PRL_MACOS_PARTITION,
  											  Uuid("{48465300-0000-11AA-AA11-00306543ECAC}"),
 											  QT_TRANSLATE_NOOP("QObject",
																 "Hierarchical File System (HFS+) partition"));
	APPEND_PARTITION(GPT_OS_MACOSX, PRL_MACOS_PARTITION,
  											  Uuid("{55465300-0000-11AA-AA11-00306543ECAC}"),
 											  QT_TRANSLATE_NOOP("QObject", "Apple UFS"));
	APPEND_PARTITION(GPT_OS_MACOSX, PRL_MACOS_PARTITION,
  											  Uuid("{52414944-0000-11AA-AA11-00306543ECAC}"),
 											  QT_TRANSLATE_NOOP("QObject", "Apple RAID partition"));
	APPEND_PARTITION(GPT_OS_MACOSX, PRL_MACOS_PARTITION,
  											  Uuid("{52414944-5F4F-11AA-AA11-00306543ECAC}"),
 											  QT_TRANSLATE_NOOP("QObject", "Apple RAID partition, offline"));
	APPEND_PARTITION(GPT_OS_MACOSX, PRL_MACOS_BOOT_PARTITION,
  											  Uuid("{426F6F74-0000-11AA-AA11-00306543ECAC}"),
 											  QT_TRANSLATE_NOOP("QObject", "Apple Boot partition"));
	APPEND_PARTITION(GPT_OS_MACOSX, PRL_MACOS_PARTITION,
  											  Uuid("{4C616265-6C00-11AA-AA11-00306543ECAC}"),
 											  QT_TRANSLATE_NOOP("QObject", "Apple Label"));
	APPEND_PARTITION(GPT_OS_MACOSX, PRL_MACOS_PARTITION,
  											  Uuid("{5265636F-7665-11AA-AA11-00306543ECAC}"),
 											  QT_TRANSLATE_NOOP("QObject", "Apple TV Recovery partition"));
	// Solaris
	APPEND_PARTITION(GPT_OS_SOLARIS, PRL_INVALID_PARTITION,
  											  Uuid("{6A82CB45-1DD2-11B2-99A6-080020736631}"),
 											  QT_TRANSLATE_NOOP("QObject", "Boot partition"));
	APPEND_PARTITION(GPT_OS_SOLARIS, PRL_INVALID_PARTITION,
  											  Uuid("{6A85CF4D-1DD2-11B2-99A6-080020736631}"),
 											  QT_TRANSLATE_NOOP("QObject", "Root partition"));
	APPEND_PARTITION(GPT_OS_SOLARIS, PRL_INVALID_PARTITION,
  											  Uuid("{6A87C46F-1DD2-11B2-99A6-080020736631}"),
 											  QT_TRANSLATE_NOOP("QObject", "Swap partition"));
	APPEND_PARTITION(GPT_OS_SOLARIS, PRL_INVALID_PARTITION,
  											  Uuid("{6A8B642B-1DD2-11B2-99A6-080020736631}"),
 											  QT_TRANSLATE_NOOP("QObject", "Backup partition"));
	APPEND_PARTITION(GPT_OS_SOLARIS, PRL_INVALID_PARTITION,
  											  Uuid("{6A898CC3-1DD2-11B2-99A6-080020736631}"),
 											  QT_TRANSLATE_NOOP("QObject", "/usr partition"));
	APPEND_PARTITION(GPT_OS_SOLARIS, PRL_INVALID_PARTITION,
  											  Uuid("{6A8EF2E9-1DD2-11B2-99A6-080020736631}"),
 											  QT_TRANSLATE_NOOP("QObject", "/var partition"));
	APPEND_PARTITION(GPT_OS_SOLARIS, PRL_INVALID_PARTITION,
  											  Uuid("{6A90BA39-1DD2-11B2-99A6-080020736631}"),
 											  QT_TRANSLATE_NOOP("QObject", "/home partition"));
	APPEND_PARTITION(GPT_OS_SOLARIS, PRL_INVALID_PARTITION,
  											  Uuid("{6A9283A5-1DD2-11B2-99A6-080020736631}"),
 											  QT_TRANSLATE_NOOP("QObject", "EFI_ALTSCTR"));
	APPEND_PARTITION(GPT_OS_SOLARIS, PRL_INVALID_PARTITION,
  											  Uuid("{6A945A3B-1DD2-11B2-99A6-080020736631}"),
 											  QT_TRANSLATE_NOOP("QObject", "Reserved partition"));
	APPEND_PARTITION(GPT_OS_SOLARIS, PRL_INVALID_PARTITION,
  											  Uuid("{6A9630D1-1DD2-11B2-99A6-080020736631}"),
 											  QT_TRANSLATE_NOOP("QObject", "Reserved partition"));
	APPEND_PARTITION(GPT_OS_SOLARIS, PRL_INVALID_PARTITION,
  											  Uuid("{6A980767-1DD2-11B2-99A6-080020736631}"),
 											  QT_TRANSLATE_NOOP("QObject", "Reserved partition"));
	APPEND_PARTITION(GPT_OS_SOLARIS, PRL_INVALID_PARTITION,
  											  Uuid("{6A96237F-1DD2-11B2-99A6-080020736631}"),
 											  QT_TRANSLATE_NOOP("QObject", "Reserved partition"));
	APPEND_PARTITION(GPT_OS_SOLARIS, PRL_INVALID_PARTITION,
  											  Uuid("{6A8D2AC7-1DD2-11B2-99A6-080020736631}"),
 											  QT_TRANSLATE_NOOP("QObject", "Reserved partition"));
}

///////////////////////////////////////////////////////////////////////////////
/**
 * GPT transcoder destructor
 *
 * @author antonz@
 */
///////////////////////////////////////////////////////////////////////////////
CGPT::~CGPT()
{
	// Cleanup
	m_GMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
/**
 * Get description of partition type by GUID
 *
 * @author antonz@
 *
 * @param GUID of partition
 *
 * @return Description of partition in native language or empty string
 * if there are no such GUID
 */
///////////////////////////////////////////////////////////////////////////////
QString CGPT::GetDescriptionByGUID(const Uuid& guid) const
{
	// Return value
	QString RetVal;
	// Iterator through list
	PartsMapConstIter mapIter = m_GMap.find(guid);

	if (mapIter != m_GMap.end())
		RetVal = mapIter->second.Description;

	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
/**
 * Get description of partition type by GUID
 *
 * @author antonz@
 *
 * @param GUID of partition
 *
 * @return Partition OS type or invalid type if there are no such GUID
 */
///////////////////////////////////////////////////////////////////////////////
GPT_OS_TYPE CGPT::GetTypeByGUID(const Uuid& guid) const
{
	// Iterator through list
	PartsMapConstIter mapIter = m_GMap.find(guid);
	// Return value
	GPT_OS_TYPE RetVal = GPT_OS_INVALID;

	if (mapIter != m_GMap.end())
		RetVal = mapIter->second.OsType;

	return RetVal;
}

/**
 * Get MBR type of partition by GUID
 *
 * @author antonz@
 *
 * @param GUID of partition
 *
 * @return Partition OS type or invalid type if there are no such GUID
 */
PRL_UINT8 CGPT::GetMbrTypeByGUID(const Uuid& guid) const
{
	// Iterator through list
	PartsMapConstIter mapIter = m_GMap.find(guid);
	// Return value
	PRL_UINT8 RetVal = PRL_INVALID_PARTITION;

	if (mapIter != m_GMap.end())
		RetVal = mapIter->second.MbrType;

	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
/**
 * Get list of all supported GUIDs
 *
 * @author antonz@
 *
 * @return List of GUIDs
 */
///////////////////////////////////////////////////////////////////////////////
QList<QString> CGPT::GetDataPartitionsList() const
{
	// Temporary string
	QString tmpStr;
	// Return value
	QList<QString> RetVal;
	// Iterator through list
	PartsMapConstIter mapIter;
	PartsMapConstIter mapIterEnd = m_GMap.end();
	// Descrition
	GPT_DESCR Descr;

	// Get all types that are not generic or invalid
	for(mapIter = m_GMap.begin(); mapIter != mapIterEnd; mapIter++)
	{
		// We find it!
		if ((mapIter->second.OsType != GPT_OS_INVALID) &&
		    (mapIter->second.OsType != GPT_OS_GENERIC))
		{
			tmpStr = mapIter->first.toString();
			// Remove { and } from string
			tmpStr = tmpStr.mid(1, tmpStr.size() - 2);
			tmpStr = tmpStr.toUpper();
			RetVal.append(tmpStr);
		}
	}

	return RetVal;
}

// Convertion class static declaration
static CGPT s_GptConvertor;

/**
 * Get description of partition type by GUID
 *
 * @author antonz@
 *
 * @param GUID of partition
 *
 * @return Description of partition in native language or empty string
 * if there are no such GUID
 */
QString CGPTConvertor::GetDescriptionByGUID(const Uuid& guid)
{
	return s_GptConvertor.GetDescriptionByGUID(guid);
};

/**
 * Get type of partition type by GUID
 *
 * @author antonz@
 *
 * @param GUID of partition
 *
 * @return Partition OS type or invalid type if there are no such GUID
 */
GPT_OS_TYPE CGPTConvertor::GetTypeByGUID(const Uuid& guid)
{
	return s_GptConvertor.GetTypeByGUID(guid);
};

/**
 * Get list of all supported GUIDs
 *
 * @author antonz@
 *
 * @return List of GUIDs
 */
QList<QString> CGPTConvertor::GetDataPartitionsList()
{
	return s_GptConvertor.GetDataPartitionsList();
};

/**
 * Get MBR of partition type by GUID
 *
 * @author antonz@
 *
 * @param GUID of partition
 *
 * @return Partition OS type or invalid type if there are no such GUID
 */
PRL_UINT8 CGPTConvertor::GetMbrTypeByGUID(const Uuid& guid)
{
	return s_GptConvertor.GetMbrTypeByGUID(guid);
};
