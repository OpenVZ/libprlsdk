/*
 * CBaseNode.h: Definition of the class CBaseNode. This class is
 * abstract base class of the XML serialization and deserialization.
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


#ifndef CBASENODE_H
#define CBASENODE_H


#include <QtCore>
#include <QtXml>

#include "Build/Current.ver"
#include "Interfaces/ParallelsDomModel.h"
#include "Interfaces/ParallelsQt.h"
#include "Interfaces/ParallelsNamespace.h"
#include "SDK/Include/PrlEnums.h"
#include "SDK/Include/PrlErrors.h"
#include "Libraries/PrlUuid/Uuid.h"
#include "Libraries/Logging/Logging.h"
#include "Libraries/PrlCommonUtilsBase/ParallelsDirs.h"
#include "Libraries/PrlDataSerializer/CPrlOpaqueTypeDataSerializer.h"
#include "Libraries/PrlDataSerializer/CPrlStringDataSerializer.h"
// For compatibility
	#include <vector>
//////

// Megre options are used in mergeDocuments method
typedef enum _MergeOptions
{
	moNoOptions					= 0,	// full merge by each field
	moEnableRestrictSections	= 1,	// at merging the section is taken as a whole entity
	moEnableFixedFields			= 2,	// switch on read only fields (values are taken from the current doc)
	moEnableAllOptions			= moEnableRestrictSections | moEnableFixedFields,
} MergeOptions;


class CBaseNode
{
public:

	// Create XML document
	QDomDocument getXml(bool IncludeXmlNode = XML_DOC_INCLUDE_XML_NODE, bool no_save_option = false) const;
	// Get XML element
	virtual QDomElement getXml(QDomDocument* Document, bool no_save_option = false) const;// = 0;
	// Read XML
	virtual int readXml(QDomElement* RootElement,
						QString ext_tag_name = QString(),
						bool unite_with_loaded = false);// = 0;
	virtual void syncItemIds();// = 0

	// Set default values
	virtual void setDefaults(QDomElement* RootElement = 0);// = 0;
	// Custom initialization
	virtual void CustomInit();
	// Init lists
	virtual void InitLists();// = 0;
	// Clear lists
	virtual void ClearLists();// = 0;
	// Custom copying
	virtual void CustomCopy(const CBaseNode* );

	// Serialization XML to string stream
	QString toString(bool IncludeXmlNode = false, bool no_save_option = false) const;
	// Deserialization string stream to XML
	int fromString( QString SourceString,
					bool unite_with_loaded = false,
					QString * errorMsg = 0,
					int * errorLine = 0,
					int * errorColumn = 0 );
	int fromString( QString SourceString,
					QString extTagName,
					bool unite_with_loaded = false,
					QString * errorMsg = 0,
					int * errorLine = 0,
					int * errorColumn = 0 );

	virtual QVariant getPropertyValue(QString path) const
	{
		Q_UNUSED(path);
		return QVariant();
	}
	virtual bool setPropertyValue(QString path, QVariant value, bool* pbValueChanged = 0)
	{
		Q_UNUSED(path);
		Q_UNUSED(value);
		Q_UNUSED(pbValueChanged);
		return false;
	}

	// Unite new and old format XML documents
	int uniteDocuments( QString qsNewFormatDoc, QString qsOldFormatDoc );

	template<class T>
	int mergeDocuments( const T* pCur, const T* pPrev, MergeOptions nOptions = moNoOptions )
	{
		T* pNew = dynamic_cast<T* >(this);
		if ( ! pNew || ! pCur || ! pPrev )
		{
			m_iParseRc = PRL_ERR_FAILURE;
			return m_iParseRc;
		}

		pNew->syncItemIds();

		T cur;
		m_iParseRc = cur.fromString(pCur->toString());
		if (PRL_FAILED(m_iParseRc))
			return m_iParseRc;

		T prev;
		m_iParseRc = prev.fromString(pPrev->toString());
		if (PRL_FAILED(m_iParseRc))
			return m_iParseRc;

		bool bRes = pNew->merge(&cur, &prev, nOptions);
		if (bRes)
		{
			m_qsErrorMessage.clear();
			return PRL_ERR_SUCCESS;
		}

		m_iParseRc = PRL_ERR_MERGE_XML_DOCUMENT_CONFLICT;
		return m_iParseRc;
	}

	template<class T>
	int diffDocuments( const T* pOld, QStringList& lstDiffFullItemIds ) const
	{
		const T* pNew = dynamic_cast<const T* >(this);
		if ( ! pNew || ! pOld )
			return PRL_ERR_FAILURE;

		T diff_new;
		diff_new.fromString(pNew->toString());
		T diff_old;
		diff_old.fromString(pOld->toString());

		diff_new.diff(&diff_old, lstDiffFullItemIds);

		return PRL_ERR_SUCCESS;
	}

	// Load from file
	virtual int loadFromFile(QFile* File, bool BNeedLoadAbsolutePath = true);
	virtual int loadFromFile(QString FileName, bool BNeedLoadAbsolutePath = true);

	// Save to file
	virtual int saveToFile(QFile* File, bool do_replace = true, bool BNeedToSaveRelativePath = true);
	virtual int saveToFile(QString FileName, bool do_replace = true, bool BNeedToSaveRelativePath = true);
	virtual int saveToFile();

	/**
	* @brief Enable crash safe mech to save to file
	* @return true in success
	* @note It works only for file owner with files with default permissions!
	*/
	bool enableCrashSafeSaving();

	// Get error message
	QString GetErrorMessage() const;
	// Get all warnings
	QStringList GetWarningList() const;

	void Trace() const;

	// Get own root tag name
	QString getTagName() const
	{
		return m_qsTagName;
	}

	// Set ext root tag name
	void setExtRootTagName(QString qsTagName)
	{
		m_qsExtRootTagName = qsTagName;
	}
	// Get root tag name
	QString getExtRootTagName() const
	{
		return m_qsExtRootTagName;
	}

	// Set item ID
	void setItemId(int nItemId)
	{
		m_nItemId = nItemId;
	}
	// Get item ID
	int getItemId() const
	{
		return m_nItemId;
	}
	// Clean max IDs' map
	void clearMaxDynListIds()
	{
		m_mapMaxDynListIds.clear();
	}

	// Fake single fixed section (i.e. fields with default values)
	bool isSectionFake() const
	{
		return m_bFakeSection;
	}
	void setSectionFakeFlag(bool bFakeSection)
	{
		m_bFakeSection = bFakeSection;
	}

	QString getFullItemId() const
	{
		return m_qsFullItemId;
	}
	void makeFullItemId(QString qsParentFullItemId, QString tag_name, int num = -1)
	{
		m_qsFullItemId.clear();

		if ( ! qsParentFullItemId.isEmpty() )
			m_qsFullItemId = qsParentFullItemId + ".";
		m_qsFullItemId += tag_name;
		if ( num >= 0 )
			m_qsFullItemId += QString("[%1]").arg(num);
	}

	int getMaxDynListId(QString tag_name) const
	{
		if (m_mapMaxDynListIds.contains(tag_name))
			return m_mapMaxDynListIds.value(tag_name);
		// Unexpected
		return -1;
	}

	bool isFieldPatched(QString tag_name) const
	{
		return !getFieldPatchedValue( tag_name ).isEmpty();
	}
	void markPatchedField(QString tag_name, QString patch_stamp)
	{
		m_mapPatchedFields.insert(tag_name, patch_stamp);
	}
	QString getFieldPatchedValue(QString tag_name) const
	{
		return m_mapPatchedFields.value(tag_name, "");
	}

	void setDiagnosticFields(const QStringList& lstDiagnosticFields)
	{
		m_lstDiagnosticFields = lstDiagnosticFields;
	}

	// Additional special helper function for serialization
	QString ElementToString(const QString& tagName) const;

// For compatibility
	// Class initialization RC
	// see PRL_RESULT
	PRL_RESULT m_uiRcInit;
	// Parse error string
	QString m_szErrMsg;
	// Parse error line
	int m_iErrLine;
	// Parse error column
	int m_iErrCol;
	// Parse specific rc
	// see PVE::ConfigurationParser
	int m_iParseRc;

	virtual void cleanupClassProperties();
	QString getOutFileName() const;
	// Delete all list elements and clear list
	template <class T> void ClearList(QList<T*>& list)
	{
		T* pObject = 0;
		while(!list.isEmpty())
		{
			pObject = list.takeFirst();
			list.removeAll(pObject);
			if (pObject)
			{
				delete pObject;
			}
		}
	}

protected:

	// Default constructor
	CBaseNode() : m_uiRcInit(0), m_iErrLine(0),
		m_iErrCol(0), m_iParseRc(0),
		m_nItemId(-1),
		m_bFakeSection(false),
		m_flgCrashSafeSaving(false)
		{}
	// Virtual destructor
	virtual ~CBaseNode() {}

	void CopyBase(const CBaseNode* pBN);

// Obsolete methods
	// Append node and its value to the document element.
	virtual void addNodeValue( QDomDocument* parent_doc,
		QDomElement* parent_element,
		const QString& element_name, const QString& value,
		PVE::ParamFieldDataType value_type = PVE::Text,
		const QString& attribute_name = XML_VM_ATTR_VALUE );
	// Get parameter node value.
	virtual bool getParamNodeValue( QDomElement* parent_element,
		const QString& param_name, QString& param_value,
		PVE::ParamFieldDataType value_type = PVE::Text,
		const QString& attribute_name = XML_VM_ATTR_VALUE );
///////

	// Initialize list
	template <class T> void InitList(int min_occur, QList<T*>& list)
	{
		int nInit = list.size();
		for(int i = nInit; i < min_occur; i++)
		{
			list += new T();
		}
	}

	// Convert from UTC to local time
	QDateTime toLocalTime(QDateTime dt)
	{
		dt.setTimeSpec(Qt::UTC);
		return dt.toLocalTime();
	}
 
	int syncMaxItemId(int nMaxItemId, QString tag_name)
	{
		if ( ! m_mapMaxDynListIds.contains(tag_name) )
			m_mapMaxDynListIds.insert(tag_name, nMaxItemId);
		else if ( nMaxItemId > m_mapMaxDynListIds.value(tag_name) )
			m_mapMaxDynListIds[tag_name] = nMaxItemId;
		else
			nMaxItemId = m_mapMaxDynListIds.value(tag_name);
		return nMaxItemId;
	}

	int syncMaxItemId(int nMaxItemId, QString tag_name, QMap<QString , int >& mapMaxDynListIds) const
	{
		if ( ! mapMaxDynListIds.contains(tag_name) )
			mapMaxDynListIds.insert(tag_name, nMaxItemId);
		else if ( nMaxItemId > mapMaxDynListIds.value(tag_name) )
			mapMaxDynListIds[tag_name] = nMaxItemId;
		else
			nMaxItemId = mapMaxDynListIds.value(tag_name);
		return nMaxItemId;
	}

	void initMaxItemIds(QStringList dyn_lists)
	{
		QString qsPrevItem;
		foreach(QString qsItem, dyn_lists)
		{
			if (qsItem.isEmpty()) continue;
			if (qsItem.at(0).isLetter())
				m_mapMaxDynListIds.insert(qsItem, 0);
			else if ( ! qsPrevItem.isEmpty()
						&& qsPrevItem.at(0).isLetter() && qsItem.at(0).isDigit()
						&& m_mapMaxDynListIds.contains(qsPrevItem) )
				m_mapMaxDynListIds[qsPrevItem] = qsItem.toInt();

			qsPrevItem = qsItem;
		}
	}

	template <class T>
	int getMaxItemId(const QList<T* >& lstItems, QString tag_name)
	{
		 int nMaxItemId = -1;
		 foreach(T* pBN, lstItems)
		 {
			 if (pBN && pBN->m_nItemId > nMaxItemId)
				 nMaxItemId = pBN->m_nItemId;
		 }
		 return syncMaxItemId(++nMaxItemId, tag_name);
	}

	template <class T>
	int getMaxItemId(const QList<T* >& lstItems, QString tag_name, QMap<QString , int >& mapMaxDynListIds) const
	{
		 int nMaxItemId = -1;
		 foreach(T* pBN, lstItems)
		 {
			 if (pBN && pBN->m_nItemId > nMaxItemId)
				 nMaxItemId = pBN->m_nItemId;
		 }
		 return syncMaxItemId(++nMaxItemId, tag_name, mapMaxDynListIds);
	}

	int getMaxItemId2(QDomElement element, QString tag_name)
	{
		int nMaxItemId = -1;
		while(!element.isNull())
		{
			if ( element.tagName() == tag_name )
			{
				int nItemId = element.attribute("id", "-1").toInt();
				if (nItemId > nMaxItemId)
					nMaxItemId = nItemId;
			}
			element = element.nextSiblingElement();
		}
		return syncMaxItemId(++nMaxItemId, tag_name);
	}

	template <class T>
	void fillItemIdMap(const QList<T* >& lstItems, QMap<int , QString >& mapItemIds) const
	{
		foreach(T* pBN, lstItems)
		{
			if (pBN)
				mapItemIds.insert(pBN->m_nItemId, pBN->toString());
		}
	}

	template <class T>
	QMap<int , T* > getItemMap(const QList<T* >& lstItems) const
	{
		QMap<int , T* > mapIdObj;
		foreach(T* pBN, lstItems)
		{
			mapIdObj.insert(pBN->getItemId(), pBN);
		}
		return mapIdObj;
	}

	template <class T>
	bool mergeList(QList<T* >& lstNew, const QList<T* >& lstCur, const QList<T* >& lstPrev,
					QString tag_name, MergeOptions nOptions)
	{
		QMap<int , T* > mapNew = getItemMap<T>(lstNew);
		QMap<int , T* > mapCur = getItemMap<T>(lstCur);
		QMap<int , T* > mapPrev = getItemMap<T>(lstPrev);

		if (mapNew.keys() != mapCur.keys() && mapNew.keys() != mapPrev.keys() && mapCur.keys() != mapPrev.keys())
		{
			m_qsErrorMessage += "." + tag_name;
			return false;
		}

		if (mapNew.keys() != mapCur.keys() && mapNew.keys() == mapPrev.keys())
		{
			foreach(T* object, lstCur)
			{
				if ( mapNew.contains(object->getItemId()) ) continue;
				lstNew += new T();
				lstNew.last()->setItemId(object->getItemId());
				lstNew.last()->fromString(object->toString());
			}
			foreach(T* object, lstNew)
			{
				if ( mapCur.contains(object->getItemId()) ) continue;
				mapNew.remove(object->getItemId());
				lstNew.removeAll(object);
				delete object;
			}
		}

		typename QMap<int , T* >::const_iterator it;
		for(it = mapNew.begin(); it != mapNew.end(); ++it)
		{
			if ( ! mapCur.contains(it.key()) || ! mapPrev.contains(it.key()) ) continue;
			if ( it.value()->merge( mapCur.value(it.key()), mapPrev.value(it.key()), nOptions ) ) continue;
			m_qsErrorMessage += ".[id=" + QString::number(it.key()) + "]" + it.value()->GetErrorMessage();
			return false;
		}

		return true;
	}

	template <class T>
	void diffList(const QList<T* >& lstNew, const QList<T* >& lstOld, QStringList& lstDiffFullItemIds) const
	{
		QMap<int , T* > mapNew = getItemMap<T>(lstNew);
		QMap<int , T* > mapOld = getItemMap<T>(lstOld);

		foreach(T* object, lstOld)
		{
			if ( mapNew.contains(object->getItemId()) ) continue;
			lstDiffFullItemIds += object->getFullItemId();
		}

		foreach(T* object, lstNew)
		{
			int nItemId = object->getItemId();
			if ( mapOld.contains(nItemId) )
			{
				mapNew[nItemId]->diff(mapOld[nItemId], lstDiffFullItemIds);
				continue;
			}
			lstDiffFullItemIds += object->getFullItemId();
		}
	}

	void checkAndInsertExtDocElement(QDomElement root_element, int& nElemIdx) const;

	QString	m_qsFileName;

	// Error message
	QString			m_qsErrorMessage;
	// All warnings
	QStringList		m_lstWarningList;
	// Own tag name
	QString			m_qsTagName;
	// Ext root tag
	QString			m_qsExtRootTagName;
	// Patched fields
	QMap<QString , QString >	m_mapPatchedFields;
	// Object ID (used now only for items of the sections as dynamic lists)
	int				m_nItemId;
	// Maximum id per list
	QMap<QString , int >	m_mapMaxDynListIds;
	// Section fake flag (used now only for items of the single fixed sections)
	bool			m_bFakeSection;
	// Full item id
	QString			m_qsFullItemId;
	// Diagnostic fields
	QStringList		m_lstDiagnosticFields;
	// Document extentions
	QDomDocument	m_doc;
	QMap<int , QDomElement >	m_mapExtDoc;

	//////////////////////////////////////////////////////////////////
	// NOTE: After adding member do not forget about assign operator !
	//////////////////////////////////////////////////////////////////

private:

	int fromString( QString SourceString,
					QString extTagName,
					bool unite_with_loaded,
					QFile* pFile,
					QString * errorMsg = 0,
					int * errorLine = 0,
					int * errorColumn = 0 );

	bool			m_flgCrashSafeSaving;

	// Copy constructor (access denied)
	CBaseNode(const CBaseNode& rBN) { CopyBase(&rBN); }
	// Assign operator (access denied)
	CBaseNode& operator=(const CBaseNode& rBN);
};


// Base class helper for implementation of singals mech in Xml model

class CBaseNodeSignals : public QObject, public CBaseNode
{
	Q_OBJECT
protected:

	// Default constructor
	CBaseNodeSignals()
		: m_nSignalsCount(0)
		{}
	// Virtual destructor
	virtual ~CBaseNodeSignals() {}

	// Check for enable signals
	bool isSignalsEnabled() const;

private:

#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
	virtual void connectNotify(const QMetaMethod& signal);
	virtual void disconnectNotify(const QMetaMethod& signal);
#else
	virtual void connectNotify(const char* signal);
	virtual void disconnectNotify(const char* signal);
#endif // QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )

	int		m_nSignalsCount;

	// Copy constructor (access denied)
	CBaseNodeSignals(const CBaseNodeSignals& ) : QObject(), CBaseNode() {}
	// Assign operator (access denied)
	CBaseNodeSignals& operator=(const CBaseNodeSignals& ) { return *this; }
};


#ifndef DECLARE_METATYPE_QLIST_INT
#define DECLARE_METATYPE_QLIST_INT
Q_DECLARE_METATYPE(QList<int >)
#endif


#endif
