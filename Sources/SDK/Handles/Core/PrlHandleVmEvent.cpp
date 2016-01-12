/*
 * PrlHandleVmEvent.cpp
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


#include "PrlHandleVmEvent.h"
#include "PrlHandleVm.h"
#include "PrlHandleServerJob.h"
#include "PrlHandleEventParam.h"
#include "PrlErrStringsStorage.h"
#include <prlcommon/Logging/Logging.h>

#include <prlxmlmodel/Messaging/CVmEventParameter.h>

namespace
{

/* Stolen from QString sources. */
QString toHtmlEscaped(const QString &s)
{
	QString rich;
	const int len = s.length();
	rich.reserve(int(len * 1.1));
	for (int i = 0; i < len; ++i) {
		if (s.at(i) == QLatin1Char('<'))
			rich += QLatin1String("&lt;");
		else if (s.at(i) == QLatin1Char('>'))
			rich += QLatin1String("&gt;");
		else if (s.at(i) == QLatin1Char('&'))
			rich += QLatin1String("&amp;");
		else if (s.at(i) == QLatin1Char('"'))
			rich += QLatin1String("&quot;");
		else
			rich += s.at(i);
																    }
	rich.squeeze();
	return rich;
}

} // namespace

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


PrlHandleVmEvent::PrlHandleVmEvent( const PrlHandleServerPtr &pServer, PRL_EVENT_TYPE eType, CVmEvent* pVmEvent )
: PrlHandleEvent( pServer, eType, pVmEvent ? pVmEvent->getEventId() : 0 ), m_VmEvent(pVmEvent)
{
	if (pVmEvent && pServer)
	{
		if (pVmEvent->getEventIssuerType() == PIE_VIRTUAL_MACHINE)
			m_pVm = pServer->GetVmHandleByUuid(pVmEvent->getEventIssuerId());
	}
}

PrlHandleVmEvent::PrlHandleVmEvent( const PrlHandleVmPtr &pVm )
: PrlHandleEvent((pVm ? pVm->GetServer() : PrlHandleServerPtr(0)), PET_VM_INF_UNINITIALIZED_EVENT_CODE, 0)
{
	m_pVm = pVm;
}

PrlHandleVmEvent::PrlHandleVmEvent( const PrlHandleVmPtr &pVm, CVmEvent *pVmEvent )
: PrlHandleEvent((pVm ? pVm->GetServer() : PrlHandleServerPtr(0)), pVmEvent->getEventType(),
				  pVmEvent ? pVmEvent->getEventId() : 0),
				  m_VmEvent(pVmEvent)
{
	m_pVm = pVm;
}

PrlHandleVmEvent::~PrlHandleVmEvent()
{}


PRL_RESULT PrlHandleVmEvent::GetDataPtr( PRL_VOID_PTR_PTR data_ptr )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	char *sBuf = strdup(m_VmEvent.toString().toUtf8().data());
	if (!sBuf) return (PRL_ERR_OUT_OF_MEMORY);
	*data_ptr = (void *)sBuf;
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmEvent::GetJob(PRL_HANDLE_PTR phJob)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleServerJobPtr pJob = PrlHandleServerJob::GetJobByUuid(m_VmEvent.getInitRequestId());
	if (pJob)
	{
		pJob->AddRef();
		*phJob = pJob->GetHandle();
		return (PRL_ERR_SUCCESS);
	}
	else
	{
		*phJob = PRL_INVALID_HANDLE;
		return (PRL_ERR_UNINITIALIZED);
	}
}

PRL_RESULT PrlHandleVmEvent::GetParamsCount(PRL_UINT32_PTR pnParamsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnParamsCount = PRL_UINT32(m_VmEvent.m_lstEventParameters.size());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmEvent::GetParam(PRL_UINT32 nIndex, PRL_HANDLE_PTR phEventParam)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nIndex >= PRL_UINT32(m_VmEvent.m_lstEventParameters.size()))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleEventParam *pEventParam = new PrlHandleEventParam(PrlHandleVmEventPtr(this), m_VmEvent.m_lstEventParameters.value(nIndex));
	if (!pEventParam)
		return (PRL_ERR_OUT_OF_MEMORY);

	*phEventParam = pEventParam->GetHandle();

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmEvent::GetParamByName(PRL_CONST_STR sParamName, PRL_HANDLE_PTR phEventParam)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmEventParameter *pParam = m_VmEvent.getEventParameter(UTF8_2QSTR(sParamName));
	if (!pParam)
		return (PRL_ERR_NO_DATA);

	PrlHandleEventParam *pEventParam = new PrlHandleEventParam(PrlHandleVmEventPtr(this), pParam);
	if (!pEventParam)
		return (PRL_ERR_OUT_OF_MEMORY);

	*phEventParam = pEventParam->GetHandle();

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmEvent::FromString(PRL_CONST_STR sXml)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	foreach(CVmEventParameter *pParam, m_VmEvent.m_lstEventParameters)
		PrlControlValidity::MarkAsInvalid(pParam);

	if (PRL_FAILED(m_VmEvent.fromString(UTF8_2QSTR(sXml))))
	{
		LOG_MESSAGE(DBG_FATAL, "Couldn't to initialize VM event object with XML string: [%s]", sXml);
		return (PRL_ERR_INVALID_ARG);
	}

	//Update event type due it can be changed
	m_EventType = m_VmEvent.getEventType();

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmEvent::GetErrCode(PRL_RESULT_PTR pnErrCode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	*pnErrCode = m_VmEvent.getEventCode();

	return (PRL_ERR_SUCCESS);
}

namespace {
    enum {
        rp_MAP_SIZE = 10,
        rp_UNDEF    = 0,
        rp_FOUND    = 1,
        rp_IGNORE  = 2
    } ;
}
QString& PrlHandleVmEvent::ReplaceParams(QString &result, CVmEvent &event)
{
    QRegExp param_re("[^%]?(%([0-9]+))[^0-9]?") ;
    typedef QPair<int, QString> MapItemT ;
    MapItemT params_map[rp_MAP_SIZE] ;
    for(int i=rp_MAP_SIZE; i--; params_map[i].first=rp_UNDEF) ;
    int start_pos = 0 ;
    while ((start_pos = param_re.indexIn(result, start_pos))>=0) {
        // we use start_pos only as start searching position
        // increment it for prevent circling
        ++start_pos ;
        QString param = param_re.cap(2) ;
		if (param.toInt() <= 0)
		{
			WRITE_TRACE(DBG_FATAL, "Ignore wrong parameter: %s", param.toUtf8().constData());
			continue;
		}
        int prm_no = param.toInt()-1;
        QString prm_value ;
        if (prm_no >= rp_MAP_SIZE || (params_map[prm_no].first == rp_UNDEF))
        {
            QString prm_name(EVT_PARAM_MESSAGE_PARAM_PREFIX) ;
            if (CVmEventParameter* prm = event.getEventParameter(prm_name + QString::number(prm_no)))
            {
				if (   prm->getParamType() == PVE::DateTime
					|| prm->getParamType() == PVE::Date
					|| prm->getParamType() == PVE::Time)
				{
					QDateTime dt = QDateTime::fromString(prm->getParamValue(), XML_DATETIME_FORMAT);
					dt.setTimeSpec(Qt::UTC);
					dt = dt.toLocalTime();

					if (prm->getParamType() == PVE::Date)
						prm_value = dt.date().toString(Qt::DefaultLocaleShortDate);
					else if (prm->getParamType() == PVE::Time)
						prm_value = dt.time().toString(Qt::DefaultLocaleShortDate);
					else
						prm_value = dt.toString(Qt::DefaultLocaleShortDate);
				}
				else
				{
            		prm_value = GetLocalizedParamInErrString( event.getEventCode(),
            												  prm_no,
            												  prm->getParamValue() );
				}

				if (prm_no < rp_MAP_SIZE)
                    params_map[prm_no] = MapItemT(rp_FOUND, prm_value) ;
            }
            else {
                if (prm_no < rp_MAP_SIZE)
                    params_map[prm_no].first = rp_IGNORE ;
                WRITE_TRACE(DBG_FATAL, "ignore param: %s", param.toUtf8().constData()) ;
                continue ;
            }
        }
        else if (params_map[prm_no].first == rp_IGNORE)
            continue ;
        if (prm_no < rp_MAP_SIZE)
            prm_value = params_map[prm_no].second ;

        result.replace(param_re.pos(1), param.length()+1, toHtmlEscaped(prm_value)) ;
    }

    return result ;
}

QString PrlHandleVmEvent::GetErrString(PRL_BOOL bIsBriefMessage, PRL_BOOL bFormated)
{
	PRL_RESULT nErrCode = m_VmEvent.getEventCode() ;

	if ( !PrlErrStringsStorage::Contains(nErrCode) )
        return UTF8_2QSTR(PRL_RESULT_TO_STRING(nErrCode)) ;

    QString result = PrlErrStringsStorage::GetErrString( nErrCode, bIsBriefMessage, bFormated==PRL_TRUE, &m_VmEvent );
    ReplaceParams(result, m_VmEvent) ;
#ifdef _DEBUG
    static const QRegExp arg_param("[^%]?(%[0-9]+)[^0-9]?") ;
    if (result.indexOf(arg_param, 0)!=-1)
        WRITE_TRACE(DBG_FATAL, "Not all arguments was passed for message code '%s': %s",
                    PRL_RESULT_TO_STRING(nErrCode), result.toUtf8().constData()) ;
#endif
    return result ;
}

PRL_RESULT PrlHandleVmEvent::GetErrString(PRL_BOOL bIsBriefMessage, PRL_BOOL bFormated,
                                          PRL_STR sErrString, PRL_UINT32_PTR pnErrStringBufLength)
{
	return CopyStringValue(GetErrString(bIsBriefMessage, bFormated), sErrString, pnErrStringBufLength);
}

PRL_RESULT PrlHandleVmEvent::CanBeIgnored(PRL_BOOL_PTR pbCanBeIgnored)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbCanBeIgnored = (PRL_BOOL)PrlErrStringsStorage::CanMessageBeIgnored(m_VmEvent.getEventCode());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmEvent::IsAnswerRequired(PRL_BOOL_PTR pbIsAnswerRequired)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbIsAnswerRequired = (PRL_BOOL)m_VmEvent.getRespRequired();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmEvent::GetIssuerType( PRL_EVENT_ISSUER_TYPE_PTR pnIssuerType )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnIssuerType = m_VmEvent.getEventIssuerType();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmEvent::GetIssuerId(PRL_STR	sIssuerId,	PRL_UINT32_PTR pnIssuerIdBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmEvent.getEventIssuerId(), sIssuerId, pnIssuerIdBufLength);
}

PRL_RESULT PrlHandleVmEvent::CreateAnswerEvent(PRL_HANDLE_PTR phEvent, PRL_RESULT nAnswer)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (m_VmEvent.getInitRequestId().isEmpty())//Question object do not bind to any request - generate it
		m_VmEvent.setInitRequestId(Uuid::createUuid().toString());
	SmartPtr<CVmEvent> pXmlVmEvent( new CVmEvent );
	pXmlVmEvent->setEventCode(PET_DSP_EVT_VM_QUESTION);
	pXmlVmEvent->setEventIssuerType(m_VmEvent.getEventIssuerType());
	pXmlVmEvent->setEventIssuerId(m_VmEvent.getEventIssuerId());
	pXmlVmEvent->setInitRequestId(m_VmEvent.getInitRequestId());

	pXmlVmEvent->addEventParameter(
		new CVmEventParameter(
				PVE::String,
				m_VmEvent.getEventIssuerId(),
				EVT_PARAM_VM_UUID
				) );

	pXmlVmEvent->addEventParameter(
			new CVmEventParameter(
					PVE::UnsignedInt,
					QString("%1").arg((quint32)nAnswer),
					EVT_PARAM_MESSAGE_CHOICE_0
					) );

	PrlHandleVmEvent *pVmEvent = new PrlHandleVmEvent(PrlHandleVmPtr(0), pXmlVmEvent.getImpl());
	if (!pVmEvent)
		return (PRL_ERR_OUT_OF_MEMORY);
	*phEvent = pVmEvent->GetHandle();
	return (PRL_ERR_SUCCESS);
}

QString PrlHandleVmEvent::GetLocalizedParamInErrString( PRL_RESULT eventCode,
														int paramNo,
														const QString& paramValue )
{
	QString result;

	switch ( eventCode )
	{
	case PRL_WARN_UNABLE_OPEN_DEVICE:
	case PRL_ERR_DISK_SHARED_BLOCK:
	case PRL_ERR_DEV_ALREADY_CONNECTED:
		// event param #0 is the device class in the numeric form
		if ( paramNo == 0 )
			result = GetDeviceClass( ( PRL_DEVICE_TYPE )paramValue.toUInt() );
		break;
	default:
		result = GetVmState( paramValue );
		break;
	}

	return result.isEmpty() ? paramValue : result;
}

QString PrlHandleVmEvent::GetVmState( const QString& paramValue )
{
	QString result;

	if( paramValue == "VMS_STOPPED" )
		result = QObject::tr("stopped");
	else if( paramValue == "VMS_STARTING"    )
		result = QObject::tr("starting");
	else if( paramValue == "VMS_RESTORING"   )
		result = QObject::tr("restoring");
	else if( paramValue == "VMS_RUNNING"     )
		result = QObject::tr("running");
	else if( paramValue == "VMS_PAUSED"      )
		result = QObject::tr("paused");
	else if( paramValue == "VMS_SUSPENDING"  )
		result = QObject::tr("suspending");
	else if( paramValue == "VMS_STOPPING"    )
		result = QObject::tr("stopping");
	else if( paramValue == "VMS_COMPACTING"  )
		result = QObject::tr("compacting");
	else if( paramValue == "VMS_SUSPENDED"   )
		result = QObject::tr("suspended");
	else if( paramValue == "VMS_SNAPSHOTING" )
		result = QObject::tr("taking snapshot");
	else if( paramValue == "VMS_DELETING_STATE" )
		result = QObject::tr("deleting snapshot");
	else if( paramValue == "VMS_RESETTING"	)
		result = QObject::tr("resetting");
	else if( paramValue == "VMS_PAUSING"	)
		result = QObject::tr("pausing");
	else if( paramValue == "VMS_CONTINUING"	)
		result = QObject::tr("continuing");
	else if( paramValue == "VMS_MIGRATING"      )
		result = QObject::tr("migrating");
	else if( paramValue == "VMS_RESUMING"   )
		result = QObject::tr("resuming");
	else if( paramValue == "VMS_SUSPENDNG_SYNC"   )
		result = QObject::tr("suspending synchronization");
	else if( paramValue == "VMS_UNKNOWN"    )
		result = QObject::tr("unknown");
	else if( paramValue == "VMS_RECONNECTING" )
		result = QObject::tr("reconnecting");
	else if( paramValue == "VMS_MOUNTED"      )
		result = QObject::tr("mounted");

	return result;
}

// adapted from EnumUtils::enumToString(PRL_DEVICE_TYPE)
QString PrlHandleVmEvent::GetDeviceClass( PRL_DEVICE_TYPE type )
{
	QString result;

	if( type == PDE_GENERIC_DEVICE || type == PDE_CLUSTERED_DEVICE )
		result = QObject::tr("Generic Device");
	else if( type == PDE_STORAGE_DEVICE )
		result = QObject::tr("Storage Device");
	else if( type == PDE_FLOPPY_DISK )
		result = QObject::tr("Floppy Disk");
	else if( type == PDE_MASSSTORAGE_DEVICE )
		result = QObject::tr("Mass Storage Device");
	else if( type == PDE_OPTICAL_DISK )
		result = QObject::tr("CD/DVD");
	else if( type == PDE_HARD_DISK )
		result = QObject::tr("Hard Disk");
	else if( type == PDE_GENERIC_NETWORK_ADAPTER )
		result = QObject::tr("Network");
	else if( type == PDE_GENERIC_PORT )
		result = QObject::tr("Generic Port");
	else if( type == PDE_SERIAL_PORT )
		result = QObject::tr("Serial Port");
	else if( type == PDE_PARALLEL_PORT )
		result = QObject::tr("Parallel Port");
	else if( type == PDE_SOUND_DEVICE )
		result = QObject::tr("Sound");
	else if( type == PDE_MIXER_DEVICE )
		result = QObject::tr("Mixer Device");
	else if( type == PDE_USB_DEVICE )
		result = QObject::tr("USB");
	else if( type == PDE_PRINTER )
		result = QObject::tr("Printer");
	else if( type == PDE_GENERIC_PCI_DEVICE )
		result = QObject::tr("Intel VT-d PCIe Device");
	else if( type == PDE_PCI_VIDEO_ADAPTER )
		result = QObject::tr("Video Adapter");
	else if( type == PDE_GENERIC_SCSI_DEVICE )
		result = QObject::tr("Generic SCSI");
	else if( type == PDE_ATTACHED_BACKUP_DISK )
		result = QObject::tr("Attached Backup");

	return result;
}
