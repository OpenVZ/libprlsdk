/*
 * CXmlModelHelper.cpp: Set of simple methods which help to work
 * with XML model elements.
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


#include <prlcommon/Interfaces/ParallelsSdk.h>
#include "XmlModel/VmConfig/CVmConfiguration.h"

namespace CXmlModelHelper
{

bool indexOrderLessThan(const CVmDevice* pDev1, const CVmDevice* pDev2)
{
	return pDev1->getIndex() < pDev2->getIndex();
}

PRL_CLUSTERED_DEVICE_SUBTYPE syncScsiDevicesSubType(CVmConfiguration* pVmConfigNew,
	const CVmConfiguration* pVmConfigOld)
{
	QList<CVmClusteredDevice* > lstClusteredDevs;
	PRL_CLUSTERED_DEVICE_SUBTYPE ret = PCD_UNKNOWN_DEVICE;

	lstClusteredDevs
		= *((QList<CVmClusteredDevice* >* )(&pVmConfigOld->getVmHardwareList()->m_lstOpticalDisks));
	lstClusteredDevs
		+= *((QList<CVmClusteredDevice* >* )(&pVmConfigOld->getVmHardwareList()->m_lstHardDisks));
	lstClusteredDevs
		+= *((QList<CVmClusteredDevice* >* )(&pVmConfigOld->getVmHardwareList()->m_lstGenericScsiDevices));


	foreach(CVmClusteredDevice* pDev, lstClusteredDevs)
	{
		if (pDev->getInterfaceType() == PMS_SCSI_DEVICE)
		{
			ret = pDev->getSubType();
			break;
		}
	}

	lstClusteredDevs
		= *((QList<CVmClusteredDevice* >* )(&pVmConfigNew->getVmHardwareList()->m_lstOpticalDisks));
	lstClusteredDevs
		+= *((QList<CVmClusteredDevice* >* )(&pVmConfigNew->getVmHardwareList()->m_lstHardDisks));
	lstClusteredDevs
		+= *((QList<CVmClusteredDevice* >* )(&pVmConfigNew->getVmHardwareList()->m_lstGenericScsiDevices));

	if ( ret == PCD_UNKNOWN_DEVICE )
	{
		// If the old configuration doesn't have a SCSI controller, but new one - has, then
		// take the first added SCSI device subtype
		int id = INT_MAX;
		foreach(CVmClusteredDevice* pDev, lstClusteredDevs)
		{
			if ( pDev->getInterfaceType() == PMS_SCSI_DEVICE && pDev->getItemId() < id
				&& pDev->getSubType() != PCD_UNKNOWN_DEVICE )
			{
				id = pDev->getItemId();
				ret = pDev->getSubType();
			}
		}
		if ( ret == PCD_UNKNOWN_DEVICE )
			return ret;
	}

	foreach(CVmClusteredDevice* pDev, lstClusteredDevs)
	{
		if (pDev->getInterfaceType() != PMS_SCSI_DEVICE)
			continue;
		if ( pDev->getSubType() != ret )
		{
			WRITE_TRACE(DBG_FATAL, "SCSI device \"%s\" subtype synced from %d to %d",
				QSTR2UTF8(pDev->getSystemName()), pDev->getSubType(), ret);
			pDev->setSubType(ret);
		}
	}

	return ret;
}

PRL_CLUSTERED_DEVICE_SUBTYPE getDefaultScsiSubTypeByOsVersion(unsigned int nOsVersion)
{
	if (PVS_GUEST_VIRTIO_SUPPORTED(nOsVersion))
		return PCD_VIRTIO_SCSI;
	if (PVS_GUEST_SCSI_LSI_SPI_SUPPORTED(nOsVersion))
		return PCD_LSI_SPI;
	if (PVS_GUEST_SCSI_LSI_SAS_SUPPORTED(nOsVersion))
		return PCD_LSI_SAS;
	return PCD_BUSLOGIC;
}

static QStringList keysToIgnore()
{
	return QStringList() << "Identification.ChangeDateTime";
}

void printConfigDiff(const CVmConfiguration& cfg1,
					 const CVmConfiguration& cfg2,
					 uint logLevel,
					 const QString& qsPrefix )
{
	QStringList diffList;
	cfg1.diffDocuments( &cfg2, diffList );
	if ( ! diffList.size() )
		return;

	foreach( const QString& name, diffList )
	{
		if ( keysToIgnore().contains( name ) )
			continue;

		QString cfg1Param = cfg1.getPropertyValue( name ).toString();
		QString cfg2Param = cfg2.getPropertyValue( name ).toString();

		WRITE_TRACE( logLevel, "%s: Key: '%s', New value: '%s', Old value: '%s'",
					QSTR2UTF8( qsPrefix ),
					QSTR2UTF8( name ),
					QSTR2UTF8( cfg1Param ),
					QSTR2UTF8( cfg2Param ) );
	}
}

}//namespace CXmlModelHelper
