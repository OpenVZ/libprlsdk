/*
 * PrlErrStringsStorage.h
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


#ifndef __VIRTUOZZO_ERR_STRINGS_STORAGE_H__
#define __VIRTUOZZO_ERR_STRINGS_STORAGE_H__

#include "SDK/Include/PrlErrors.h"
#include "SDK/Include/PrlPrivateErrorsValues.h"
#include "SDK/Include/PrlEnums.h"
#include <QMap>
#include <QMutex>
#include <QRecursiveMutex>
#include <QString>
#include <QObject>

extern const char* TAG_TEXT_PRODUCT_NAME_SERVER;
/**
 * Error string description structure
 */
class CVmEvent;
struct PrlErrStringData
{
	/** Class default constructor */
	PrlErrStringData()
	: m_bCanBeIgnored(false), m_nMessageType(PMT_WARNING)
		, m_nBriefParamNumberToCheckForSingular(0)
		, m_nLongParamNumberToCheckForSingular(0)
	{}
	/** Class constructor. Initializes structure members */
	PrlErrStringData(const QString &sBriefMessage, const QString &sLongMessage, bool bCanBeIgnored, PRL_MESSAGE_TYPE_ENUM nMessageType)
	: m_sBriefMessage(sBriefMessage), m_sLongMessage(sLongMessage), m_bCanBeIgnored(bCanBeIgnored), m_nMessageType(nMessageType)
		, m_nBriefParamNumberToCheckForSingular(0)
		, m_nLongParamNumberToCheckForSingular(0)
	{}

	PrlErrStringData(const QString &sBriefMessage
		, int nBriefSingularParamNum, const QString &sBriefSingularMessage
		, const QString &sLongMessage
		, int nLongSingularParamNum, const QString &sLongSingularMessage
		, bool bCanBeIgnored, PRL_MESSAGE_TYPE_ENUM nMessageType)
		: m_sBriefMessage(sBriefMessage), m_sLongMessage(sLongMessage), m_bCanBeIgnored(bCanBeIgnored), m_nMessageType(nMessageType)
		, m_nBriefParamNumberToCheckForSingular(nBriefSingularParamNum)
		, m_sBriefSingularMessage(sBriefSingularMessage)
		, m_nLongParamNumberToCheckForSingular(nLongSingularParamNum)
		, m_sLongSingularMessage(sLongSingularMessage)
	{}
	/** Error brief message */
	QString m_sBriefMessage;

	/** Error long message */
	QString m_sLongMessage;

	/** Sign whether message can be ignored */
	bool m_bCanBeIgnored;
	/** Message classification type (question, critical and etc.) */
	PRL_MESSAGE_TYPE_ENUM m_nMessageType;

	/***********************************************/
	/** Singular string messages support. Plural form used by default. */
	/***********************************************/

	/** Parameter number to select singular brief message (if value is int and == 1 ) */
	int m_nBriefParamNumberToCheckForSingular;

	/** Error brief message for singular parameter value */
	QString m_sBriefSingularMessage;

	/** Parameter number to select singular brief message (if value is int and == 1 ) */
	int m_nLongParamNumberToCheckForSingular;

	/** Error long message for singular parameter value */
	QString m_sLongSingularMessage;

};

typedef QMap<PRL_RESULT, PrlErrStringData> PrlErrStringsStorageMap;

/**
 * This class implements central errors strings storage.
 */
class PrlErrStringsStorage : public QObject
{
Q_OBJECT

private:
	/**
	 * Class default constructor.
	 * It's private due pattern Singleton usage here
	 */
	PrlErrStringsStorage();
	/**
	 * Initializes all errors strings database
	 */
	void Initialize();

	/**
	 * Initializes all answers strings database
	 */
	void InitializeAnswers();

	/**
	 * Initializes all questions strings database
	 */
	void InitializeQuestions();

	/**
	 * Initializes all informations strings database
	 */
	void InitializeInformations();

	/**
	 * Initializes Dispatcher Server specific errors strings database
	 */
	void InitializePSErrorMessages();
	/**
	 * Returns corresponding to return code error string object
	 * @param error message code
	 * @param sign whether corresponding message was found
	 */
	static PrlErrStringData GetErrorStringData(PRL_RESULT nErrCode, bool &bMessageWasFound);

public://Static tools for manipulating errors strings storage
	/**
	 * Returns pointer to global storage object
	 */
	static PrlErrStringsStorage *GetStorageInstance();
	/**
	 * Checks whether data with specified error code presents at errors storage
	 * @param error code
	 * @return sign whether data presents
	 */
	static bool Contains(PRL_RESULT nErrCode);

	/**
	 * Returns error message by specified error code
	 * @param error code
	 * @param sign whether brief message must be returned
	 * @param sign that parameter Specifies whether HTML tags used for text formatting
	 *		should be included in the resulting string.
	 * @param params to check for singular message.
	 * @return error string message
	 */
	static QString GetErrString(PRL_RESULT nErrCode, bool bIsBriefMessage, bool bFormated=true, const CVmEvent* pParams = 0);

	/**
	 * Returns sign whether error message can be ignored
	 * @param error code
	 * @return sign whether error message can be ignored
	 */
	static bool CanMessageBeIgnored(PRL_RESULT nErrCode);

	/**
	 * Returns message classification type (question, critical and etc.)
	 * @param error code
	 * @return message classification type
	 */
	static PRL_MESSAGE_TYPE_ENUM GetMessageType(PRL_RESULT nErrCode);

public:
	/** Common errors strings database */
	PrlErrStringsStorageMap m_CommonErrorsStringsMap;
	/** Server specific errors strings database */
	PrlErrStringsStorageMap m_PSErrorsStringsMap;
	/** Desktop specific errors strings database */
	PrlErrStringsStorageMap m_PDErrorsStringsMap;
	/** Workstation specific errors strings database */
	PrlErrStringsStorageMap m_PWErrorsStringsMap;
	/** Access specific errors strings database */
	PrlErrStringsStorageMap m_PAErrorsStringsMap;

private://Global objects
	/** Pointer to global errors strings storage object */
	static PrlErrStringsStorage *g_pErrStringsStorage;
	/** Global errors strings storage access synchronization object */
	static QRecursiveMutex *g_pErrorsStringsStorageMutex;
};

#endif // __VIRTUOZZO_ERR_STRINGS_STORAGE_H__
