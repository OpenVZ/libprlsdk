/*
 * CBaseNode.cpp: CBaseNode class implementation.
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


#include "CBaseNode.h"

#ifdef _WIN_
#	include <windows.h>
#	include <io.h> /* for _commit() call */
#	include <errno.h>
#else
#	include <stdio.h>
#	include <sys/errno.h>
#	include <fcntl.h>
#	include <unistd.h>
#endif

#include <prlcommon/Logging/Logging.h>
#include <prlcommon/Std/PrlAssert.h>
#include <prlcommon/HostUtils/HostUtils.h>
#include <prlcommon/PrlCommonUtilsBase/CSimpleFileHelper.h>


//////////////
// Stubs
//////////////

QDomElement CBaseNode::getXml(QDomDocument* /*Document*/, bool ) const
{
	return QDomElement();
}
int CBaseNode::readXml(QDomElement* /*RootElement*/, QString /*ext_tag_name*/, bool /*unite_with_loaded*/)
{
	return 0;
}
void CBaseNode::syncItemIds()
{
}
void CBaseNode::setDefaults(QDomElement* /*RootElement*/)
{
}
void CBaseNode::CustomInit()
{
}
void CBaseNode::InitLists()
{
}
void CBaseNode::ClearLists()
{
}
void CBaseNode::CustomCopy(const CBaseNode* )
{
}

////////////

/**
 * @brief Create XML document
 * @param
 * @return QDomDocument XML document
 */
QDomDocument CBaseNode::getXml(bool IncludeXmlNode, bool no_save_option) const
{
	QDomDocument document;

	if (IncludeXmlNode)
	{
		QDomProcessingInstruction xml_node = document.createProcessingInstruction(
											XML_DOC_VERSION_TAG, XML_DOC_VERSION_DATA);
		document.appendChild(xml_node);
	}

	QDomElement root_element = getXml(&document, no_save_option);

	if ( ! no_save_option )
		root_element.setAttribute("id", getItemId());

	if (IncludeXmlNode)
	{
		root_element.setAttribute(XML_VM_ATTR_SCHEMA_VERSION, XML_VM_CONFIG_SCHEMA_VERSION);
	}

	document.appendChild(root_element);

	return document;
}


/**
 * @brief Serialization XML to string stream
 * @return string XML as string stream
 */
QString CBaseNode::toString(bool IncludeXmlNode, bool no_save_option) const
{
	QDomDocument document = getXml(IncludeXmlNode, no_save_option);

	QString str;
	QTextStream out( &str );
	document.save( out, 3 );
	out.flush();

	if (str.contains(QChar('\0')))
	{
		WRITE_TRACE(DBG_FATAL, "WARNING: problem document was generated contains null symbols - will be patched");
		PUT_RAW_MESSAGE(QSTR2UTF8(str));
		str.replace('\0', ' ');
	}

	return str;
}


/**
 * @brief Deserialization string stream to XML
 * @param[in] source_string XML as string stream
 * @return int value: 0 - success, otherwise error
 */
int CBaseNode::fromString(QString SourceString,
						  bool unite_with_loaded,
						  QString * errorMsg,
						  int * errorLine,
						  int * errorColumn)
{
	return fromString(SourceString, "", unite_with_loaded, 0, errorMsg, errorLine, errorColumn);
}

int CBaseNode::fromString(QString SourceString,
						  QString extTagName,
						  bool unite_with_loaded,
						  QString * errorMsg,
						  int * errorLine,
						  int * errorColumn)
{
	return fromString(SourceString, extTagName, unite_with_loaded, 0, errorMsg, errorLine, errorColumn);
}

int CBaseNode::fromString(QString SourceString,
						  QString extTagName,
						  bool unite_with_loaded,
						  QFile* pFile,
						  QString * errorMsg,
						  int * errorLine,
						  int * errorColumn)
{
	QDomDocument document;

	QString error_message;
	int line = 0;
	int column = 0;
	m_qsErrorMessage.clear();

	m_szErrMsg.clear();
	m_iErrLine = 0;
	m_iErrCol = 0;

	m_uiRcInit = PRL_ERR_PARSE_VM_CONFIG;
	m_iParseRc = PRL_ERR_PARSE_VM_CONFIG;
	bool res = false;

	if (pFile)
	{
		res = document.setContent(pFile, false, &error_message, &line, &column);
	}
	else
	{
		res = document.setContent(SourceString, false, &error_message, &line, &column);
	}

	if (!res)
	{
		m_qsErrorMessage = QString("Error: %1, line: %2, column: %3.\n")
							.arg(error_message).arg(line).arg(column);
		if (errorMsg)
		{
			*errorMsg = error_message;
		}
		if (errorLine)
		{
			*errorLine = line;
		}
		if (errorColumn)
		{
			*errorColumn = column;
		}

		m_szErrMsg = error_message;
		m_iErrLine = line;
		m_iErrCol = column;

//		Trace();

		return PRL_ERR_PARSE_VM_CONFIG;
	}

	m_uiRcInit = 0;
	m_iParseRc = 0;

	QDomElement element = document.firstChildElement();
	while(!element.isNull())
	{
		if (!readXml(&element, extTagName, unite_with_loaded))
		{
			setItemId( element.attribute("id", "-1").toInt() );

			foreach(QString qsField, m_lstDiagnosticFields)
			{
				WRITE_TRACE( DBG_WARNING, "LoadedDoc: path: '%s', value: '%s'",
					QSTR2UTF8(qsField), QSTR2UTF8(getPropertyValue(qsField).toString()) );
			}

			return 0;
		}

//		Trace();

//		m_szErrMsg += m_qsErrorMessage + "\n"
//					+ m_lstWarningList.join("\n");

		element = element.nextSiblingElement();
	}

	int ret = PRL_ERR_PARSE_VM_CONFIG;

	m_uiRcInit = (PRL_RESULT )ret;
	m_iParseRc = ret;

	return ret;
}

void CBaseNode::CopyBase(const CBaseNode* pBN)
{
	if (pBN)
		this->operator=(*pBN);
}

CBaseNode& CBaseNode::operator=(const CBaseNode& rBN)
{
	if (this == &rBN)
		return *this;

	m_uiRcInit = rBN.m_uiRcInit;
	m_szErrMsg = rBN.m_szErrMsg;
	m_iErrLine = rBN.m_iErrLine;
	m_iErrCol = rBN.m_iErrCol;
	m_iParseRc = rBN.m_iParseRc;
	m_qsFileName = rBN.m_qsFileName;
	m_qsErrorMessage = rBN.m_qsErrorMessage;
	m_lstWarningList = rBN.m_lstWarningList;
	m_qsTagName = rBN.m_qsTagName;
	m_qsExtRootTagName = rBN.m_qsExtRootTagName;
	m_mapPatchedFields = rBN.m_mapPatchedFields;
	m_nItemId = rBN.m_nItemId;
	m_mapMaxDynListIds = rBN.m_mapMaxDynListIds;
	m_bFakeSection = rBN.m_bFakeSection;
	m_qsFullItemId = rBN.m_qsFullItemId;
	m_flgCrashSafeSaving = rBN.m_flgCrashSafeSaving;

	m_mapExtDoc.clear();
	m_doc = QDomDocument();
	QMap<int , QDomElement >::const_iterator it;
	for(it = rBN.m_mapExtDoc.begin(); it != rBN.m_mapExtDoc.end(); it++)
	{
		m_mapExtDoc.insert(it.key(), m_doc.importNode(it.value(), true).toElement());
	}

	return *this;
}

int CBaseNode::uniteDocuments( QString qsNewFormatDoc, QString qsOldFormatDoc )
{
	int ret = fromString(qsNewFormatDoc);
	if ( ret )
		return ret;
	ret = fromString(qsOldFormatDoc, true);
	return ret;
}

/**
 * @brief Load XML document from file
 * @param[in] File Pointer to XML file
 * @return int value: 0 - success, otherwise error
 */
int CBaseNode::loadFromFile(QFile* File, bool )
{
	if (!File)
	{
		m_qsErrorMessage = "Error: loadFromFile - File pointer is NULL!";
		WRITE_TRACE(DBG_FATAL, "%s", QSTR2UTF8( m_qsErrorMessage ) );
		m_uiRcInit = PRL_ERR_PARSE_VM_CONFIG;
		return m_uiRcInit;
	}

	if (File->isOpen())
	{
		File->close();
	}

	if (!File->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		m_qsErrorMessage = QString("Error: cannot open XML file '%1', err='%2'!")
					.arg(File->fileName()).arg(File->errorString());

		WRITE_TRACE(DBG_FATAL, "%s", QSTR2UTF8( m_qsErrorMessage ) );
		m_uiRcInit = PRL_ERR_PARSE_VM_CONFIG;
		return m_uiRcInit;
	}

	if (fromString(QString(), "", false, File))
	{
		File->close();
		return m_uiRcInit;
	}

	m_qsFileName = File->fileName();

	File->close();

	return 0;
}


/**
 * @brief Load XML document from file
 * @param[in] FileName File name of the XML file
 * @return int value: 0 - success, otherwise error
 */
int CBaseNode::loadFromFile(QString FileName, bool )
{
	QFile file(FileName);
	return loadFromFile(&file);
}


/**
 * @brief Enable crash safe mech to save to file
 * @return true in success
 * @note It works only for file owner with files with default permissions!
 */
bool CBaseNode::enableCrashSafeSaving()
{
	m_flgCrashSafeSaving = true;

	return true;
}


/**
 * @brief Save XML document to file
 * @param[in] File Pointer to XML file
 * @return int value: 0 - success, otherwise error
 */
int CBaseNode::saveToFile(QFile* pOrigFile, bool , bool )
{
	if (!pOrigFile)
	{
		m_qsErrorMessage = "Error: saveToFile - File pointer is NULL!";

		WRITE_TRACE(DBG_FATAL, "%s", QSTR2UTF8( m_qsErrorMessage ) );
		return PRL_ERR_XML_WRITE_FILE;
	}

	if (pOrigFile->isOpen())
	{
		pOrigFile->close();
	}

	QFile* pFile = pOrigFile;
	QFile tmpFile;
	if( m_flgCrashSafeSaving )
	{
		pFile = &tmpFile;

		QString tmpFileName = QString( "%1.tmp.%2" )
			.arg( pOrigFile->fileName() )
			.arg( Uuid::createUuid().toString() );

		tmpFile.setFileName( tmpFileName );
	}

	if ( !pFile->open(QIODevice::ReadWrite) )
	{
		m_qsErrorMessage = QString("Error: cannot open XML file '%1', err='%2'!")
					.arg(pFile->fileName()).arg(pFile->errorString());

		WRITE_TRACE(DBG_FATAL, "%s", QSTR2UTF8( m_qsErrorMessage ) );

		return PRL_ERR_XML_WRITE_FILE;
	}

	// check on possibility save file
	// bug #128539
	// new data size
	QByteArray data = toString( true, true ).toUtf8();
	int iDataSize = data.size();
	bool bRes = pFile->resize( iDataSize );
	int iResult = PRL_ERR_SUCCESS;
	if ( !bRes &&
		pFile->size() != iDataSize )
	//On remote filesystems QFile::resize() can return false but actually file will be resized.
	{
		iResult = PRL_ERR_XML_WRITE_FILE;
		// check free disk space for write
		PRL_UINT64 uiFreeSize = HostUtils::GetFreeSpaceByName( pFile->fileName() );
		if ( uiFreeSize < (PRL_UINT64)iDataSize )
			iResult = PRL_ERR_NOT_ENOUGH_DISK_SPACE_TO_XML_SAVE;
	}

	if ( PRL_FAILED( iResult ) )
	{
		pFile->close();
		return iResult;
	}

	if( !pFile->seek(0) || ( pFile->write( data ) != iDataSize ) )
	{
		WRITE_TRACE(DBG_FATAL, "Unable to write to file '%s'", QSTR2UTF8( pFile->fileName() ) );
		iResult = PRL_ERR_XML_WRITE_FILE;
	}
	if ( !pFile->flush() )
	{
		WRITE_TRACE(DBG_FATAL, "Unable to flush data '%s' with %d", QSTR2UTF8( pFile->fileName() ), pFile->error() );
		iResult = PRL_ERR_XML_WRITE_FILE;
	}


	// sync OS disk buffers with disk.

	int fsync_fd = pFile->handle();
#ifdef _WIN_
	// to prevent assert() detected by sandro@
	if( fsync_fd < 0 || _commit( fsync_fd ))
#else
	if(fsync( fsync_fd ))
#endif
	{
		WRITE_TRACE( DBG_FATAL, "fsync() failed - errno %d. fd %d. Filename = %s"
			, errno, fsync_fd, QSTR2UTF8( pFile->fileName() ) );
	}

	pFile->close();

	if ( PRL_FAILED( iResult ) )
		return iResult;

	// mv pFile to pOrigFile
	if( m_flgCrashSafeSaving )
	{
		if( !CSimpleFileHelper::AtomicMoveFile( pFile->fileName(), pOrigFile->fileName() ) )
		{
			// rm.old
			PRL_ASSERT( pFile!= pOrigFile );
			if( ! pFile->remove() )
			{
				WRITE_TRACE(DBG_FATAL, "Can't remove tmp file '%s'", QSTR2UTF8( pFile->fileName() ) );
			}

			return PRL_ERR_XML_WRITE_FILE;
		}
	}

	m_qsFileName = pOrigFile->fileName();

	foreach(QString qsField, m_lstDiagnosticFields)
	{
		WRITE_TRACE( DBG_WARNING, "SavedDoc: path: '%s', value: '%s'",
			QSTR2UTF8(qsField), QSTR2UTF8(getPropertyValue(qsField).toString()) );
	}

	return 0;
}


/**
 * @brief Save XML document to file
 * @param[in] FileName File name of the XML file
 * @return int value: 0 - success, otherwise error
 */
int CBaseNode::saveToFile(QString FileName, bool , bool )
{
	QFile file(FileName);
	return saveToFile(&file);
}


/**
 * @brief Save XML document to file
 * @return int value: 0 - success, otherwise error
 */
int CBaseNode::saveToFile()
{
	QFile file(m_qsFileName);
	return saveToFile(&file);
}


/**
 * @brief Get error message
 * @return string Error text
 */
QString CBaseNode::GetErrorMessage() const
{
	return m_qsErrorMessage;
}


/**
 * @brief Get all warnings
 * @return list List of all warnings as text
 */
QStringList CBaseNode::GetWarningList() const
{
	return m_lstWarningList;
}

void CBaseNode::Trace() const
{
	if (!m_qsErrorMessage.isEmpty())
	{
		WRITE_TRACE(DBG_FATAL, "Xml model error: %s!\n", QSTR2UTF8(m_qsErrorMessage));
	}

	for(int i = 0; i < m_lstWarningList.size(); i++)
	{
		WRITE_TRACE(DBG_FATAL, "Xml model warning: %s!\n", QSTR2UTF8(m_lstWarningList[i]));
	}
}

void CBaseNode::checkAndInsertExtDocElement(QDomElement root_element, int& nElemIdx) const
{
	while(m_mapExtDoc.contains(nElemIdx))
	{
		root_element.appendChild(
			root_element.ownerDocument()
				.importNode(m_mapExtDoc.value(nElemIdx), true).toElement() );
		++nElemIdx;
	}
	++nElemIdx;
}

//////////////////////
// For compatibility
//////////////////////

void CBaseNode::cleanupClassProperties()
{
	ClearLists();
	InitLists();
	setDefaults();
}

QString CBaseNode::getOutFileName() const
{
	return m_qsFileName;
}

/**
 * Append node and its value to the document element.
 * @param parent_doc [in] A parent document.
 * @param parent_element [in] A parent element.
 * @param element_name [in] An element name.
 * @param value [in] A element value.
 * @param value_type [in] A element value type.
 * @param attribute_name [in] An attribute name.
 */
void CBaseNode::addNodeValue(
	QDomDocument* parent_doc, QDomElement* parent_element,
	const QString& element_name,	const QString& value,
	PVE::ParamFieldDataType value_type, const QString& attribute_name )
{
	QDomElement xmlNode = parent_doc->createElement( element_name );
	switch (value_type)
	{
	case PVE::Text:
	{
		QDomText xmlText = parent_doc->createTextNode( value );
		xmlNode.appendChild( xmlText );
		break;
	}
	case PVE::CData:
	{
		QDomCDATASection xmlCData = parent_doc->createCDATASection( value );
		xmlNode.appendChild( xmlCData );
		break;
	}
	case PVE::String:
	{
		xmlNode.setAttribute( attribute_name, value );
		break;
	}
	default:
		break;
	}
	parent_element->appendChild( xmlNode );
}

/**
 * Get parameter node value.
 * @param parent_element [in] A parent element.
 * @param param_name [in] A parameter name.
 * @param param_value [out] A parameter value.
 * @param value_type [in] A parameter value type.
 * @param attribute_name [in] An attribute name.
 * @return True if it is ok, else False.
 */
bool CBaseNode::getParamNodeValue(
	QDomElement* parent_element, const QString& param_name, QString& param_value,
	PVE::ParamFieldDataType value_type, const QString& attribute_name )
{
	param_value = "";
	QDomNode paramNode = parent_element->namedItem( param_name );
	if ( paramNode.isNull() )
		return false;

	switch (value_type)
	{
	case PVE::Text:
	{
		QDomText xmlText = paramNode.firstChild().toText();
		if ( !xmlText.isNull() )
		{
			param_value = xmlText.data();
			return true;
		}
		break;
	}
	case PVE::CData:
	{
		QDomCDATASection xmlCData = paramNode.firstChild().toCDATASection();
		if ( !xmlCData.isNull() )
		{
			param_value = xmlCData.data();
			return true;
		}
		break;
	}
	case PVE::String:
	{
		QDomAttr xmlValueAttr = paramNode.toElement().attributeNode( attribute_name );
		if ( !xmlValueAttr.isNull() )
		{
			param_value = xmlValueAttr.value();
			return true;
		}
		break;
	}
	default:
		break;
	}
	return false;
}

QString CBaseNode::ElementToString(const QString& tagName) const
{
	QDomDocument xmlDummyParentDoc;
	QDomElement xmlVmConfigRoot;
	QDomNode xmlXmlVersionNode;

	xmlXmlVersionNode = xmlDummyParentDoc.createProcessingInstruction(
		XML_DOC_VERSION_TAG, XML_DOC_VERSION_DATA );
	xmlDummyParentDoc.insertBefore( xmlXmlVersionNode, xmlDummyParentDoc.firstChild() );

	// create root element
	xmlVmConfigRoot = xmlDummyParentDoc.createElement( tagName );
	xmlDummyParentDoc.appendChild( xmlVmConfigRoot );
	// append given element
	QDomElement root_element = getXml( &xmlDummyParentDoc );
	root_element.setAttribute("id", getItemId());
	xmlVmConfigRoot.appendChild(root_element);

	QString strOutput;
	QTextStream out( &strOutput );

	xmlDummyParentDoc.save( out, 3 );
	out.flush();

	return strOutput;
}

////////////////

#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
void CBaseNodeSignals::connectNotify(const QMetaMethod& signal)
#else
void CBaseNodeSignals::connectNotify(const char* signal)
#endif // QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
{
	Q_UNUSED(signal);

	++m_nSignalsCount;
}

#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
void CBaseNodeSignals::disconnectNotify(const QMetaMethod& signal)
#else
void CBaseNodeSignals::disconnectNotify(const char* signal)
#endif // QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
{
	Q_UNUSED(signal);

	if (m_nSignalsCount > 0)
		--m_nSignalsCount;
}

bool CBaseNodeSignals::isSignalsEnabled() const
{
	return m_nSignalsCount != 0;
}
