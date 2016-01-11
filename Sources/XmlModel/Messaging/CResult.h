/*
 * CResult.h: Definition of the class CResult. This class implements
 * generic resultset.
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


#ifndef CRESULT_H
#define CRESULT_H

#include <QString>
#include <QHash>
#include <QEvent>

#include <prlcommon/Interfaces/ParallelsNamespace.h>

class CVmEvent;


/**
 * @brief This class implements generic resultset.
 * @author SergeyM
 */
class CResult : public QEvent
{

public:
	// Collection of results
	QHash<int, QString> m_hashResultSet;

public:
	// Standard class constructor
	CResult();

	// Standard copy constructor
	CResult(const CResult &_obj);

	// Copy constructor
	CResult(CResult* other);

	// Initializing constructor
	CResult(QString request_id, PVE::IDispatcherCommands op_code, PRL_RESULT ret_code,
		PVE::ResultType obj_type = PVE::GenericResult, CVmEvent* p_error = 0,
		QString exec_server = "");

	// Class destructor
	~CResult();

	// Standard assign operator
	CResult &operator=(const CResult &_obj);

	// Set request ID
	void setRequestId(QString value);

	// Get request ID
	QString getRequestId() const;

	// Set server
	void setExecutiveServer(QString value);

	// Get server
	QString getExecutiveServer() const;

	// Set result type
	void setResultObjectType(PVE::ResultType value);

	// Get result type
	PVE::ResultType getResultObjectType() const;

	// Set return code
	void setReturnCode(PRL_RESULT value);

	// Get return code
	PRL_RESULT getReturnCode() const;

	// Get error
	CVmEvent* GetError() const;

	// Set error
	void setError ( CVmEvent* );

	// Add item to result set
	void addResultSetItem(int key, QString item);
	void addResultSetItem(int key, const char* item);

	QString getResultItemByKey( int key );

	// Add param token to result set
	void addParamToken(QString item);
	void addParamToken(const char* item);

	// Get param token by index
	QString GetParamToken(int nIndex) const;

	// Get count of params token
	int GetParamsCount() const;

	// Set operation code
	void setOpCode(PVE::IDispatcherCommands value);

	// Get operation code
	PVE::IDispatcherCommands getOpCode() const;

	/**
	 * Applies to response object binary content. Note that object
	 * instance became owner of applied content and operating with
	 * content object life time.
	 * @param pointer to VM event object contains reponse binary data
	 */
	void setBinaryContent(CVmEvent *pBinaryContent);

	/**
	 * Returns binary content of response object.
	 * Do not destroy returned object and make deep copy of it if want to store
	 * instance somewhere. CResult class operates with owned content and will
	 * destroy it on new content assignment or CResult object destruction.
	 */
	CVmEvent *getBinaryContent();

	// Set numeric identifier of response package
	void setPackageId(PRL_UINT64 id);

	// Get numeric identifier of response package
	PRL_UINT64 getPackageId() const;
private:
	// Request ID
	QString m_strRequestId;

	// Operation code
	PVE::IDispatcherCommands m_uiOpCode;

	// Server, which processed the request
	QString m_strExecutiveServer;

	// Result type
	PVE::ResultType m_uiResultObjectType;

	// Return code
	PRL_RESULT m_uiReturnCode;

	// Error
	CVmEvent* m_pError;

	/**
	 * Event that contains binary content of response. This content
	 * actually represents universal mech of data transmitting at
	 * response packages and this content should override all response
	 * standard parameters passing through XML model mechanism
	 */
	CVmEvent *m_pBinaryContent;

	// Numeric identifier of response package
	PRL_UINT64 m_uiPackageId;
private:
	// Check if option is set correctly
	bool isCorrectResultObjectType(PVE::ResultType value);

	// Check if option is set correctly
	bool isCorrectReturnCode(PRL_RESULT value);

	// Clean up class-specific properties
	void cleanupClassProperties();

};

#endif
