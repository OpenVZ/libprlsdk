/*
 * CodeComposer.h: Definition of the class CodeComposer. This class
 * implements source code composition.
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


#ifndef CODE_COMPOSER_H
#define CODE_COMPOSER_H


#include "XSDParser.h"
#include "CodeTemplates.h"

class CodeComposer
{
public:

	// Initializing constructor
	explicit CodeComposer(const XSDParser& XSDParser, const QString& qsXSDName);
	// Virtual destructor
	virtual ~CodeComposer();

	// Composite source code
	bool CompositeSourceCode(QString Path);
	// Get error message
	const QString& GetErrorMessage() const;

	// Set optional flag
	void  SetFlag(const QString& qsFlag);


private:

	// Default constructor (access denied)
	CodeComposer() {}
	// Copy constructor (access denied)
	CodeComposer(const CodeComposer& ) {}
	// Assign operator (access denied)
	CodeComposer& operator=(const CodeComposer& ) { return *this; }

	// Add a new 'xs:element' and 'xs:complexType' to the collection and find root element
	void AddElement(const QDomNode& ParentNode);
	// Add new relations of the potential class
	void AddRelations(const QDomNode& ParentNode,
					  const QString& class_name,
					  int LineNumber);
	// Check complex element or simple
	bool IsComplexElement(const QDomNode& ParentNode) const;

	// Composite project include file
	bool CompositeProjectIncludeFile(const QString& Path);
	// Composite source files
	bool CompositeSourceFiles(const QString& Path);
	// Composite header
	QString CompositeHeader(const QString& ClassName, const QString& RealClassName);
	// Composite unit
	QString CompositeUnit(const QString& ClassName, const QString& RealClassName, const QString& RealFileName);
	// Convert XSD simple type to real type
	QString ConvertType(const QString& Type,
						bool IsList,
						bool IsCustomType,
						QString& TypePrefix,
						QString* pDefaultValue = 0,
						QString* pConvertValueMethod = 0,
						QString* pConvertFromString = 0,
						QString* pValidate = 0) const;
	// Check file content
	bool CheckFileContent(QFile& File, const QString& Content) const;

	// Checks for enable Qt signals mech
	bool IsSignalsEnabled() const;
	// Checks for enable merge mech
	bool IsMergeEnabled() const;
	// Checks for enable change properties mech
	bool IsChangePropertiesEnabled() const;

	// Scheme name (short file name without extention)
	QString m_qsXSDName;

	QStringList m_lstHeaderFiles;
	QStringList m_lstSourceFiles;

	// Top node of the XSD document
	QDomNode	m_domTopNode;

	typedef QSet<QString >					string_set;
	typedef QSet<int >						int_set;
	typedef QMap<QString , QString >		string_map;
	typedef QMultiMap<QString , QString >	string_multimap;
	typedef QMap<QString , QDomNode >		node_map;
	typedef QMultiMap<QString , QDomNode >	node_multimap;
	typedef QMap<int , QString >			int_map;
	typedef QVector< QDomNode >				node_vector;
	typedef QMap<QString, node_vector >		vector_node_map;;

	// Root node
	QDomNode	m_domRootNode;
	// Collection of all 'xs:element's
	// and they are represented as the pair name and QDomNode
	node_map	m_mapElements;
	// Collection of all 'xs:complexType's
	// with 'name' attribute and they are represented
	// as the pair type name and QDomNode
	node_map	m_mapComplexTypes;
	// Collection of all 'xs:simpleType's
	// with 'name' attribute and they are represented
	// as the pair type name and QDomNode
	node_map	m_mapSimpleTypes;

	// Collection of all names of the classes
	string_set			m_setClassNames;
	// Collection for relation class with its
	// real name
	string_map			m_mapRealClassNames;
	// Collection for relation file with its
	// real name
	string_map			m_mapRealFileNames;
	// Collection for relation class with its
	// real generating class names
	string_map			m_mapSubClassNames;
	// Collection for relation class with its
	// base classes
	string_multimap		m_mapBaseClassNames;
	// Collection for relation external class with its
	// include file
	string_map			m_mapExternClassIncludes;
	// Collection for relation class with its
	// additional includes
	string_multimap		m_mapAdditionalIncludes;
	// Collection for relation class with its
	// lists
	vector_node_map		m_mapLists;
	// Collection for relation list class with its
	// custom name
	string_map			m_mapListNames;
	// Collection for relation list class with its
	// single fixed condition
	string_set			m_setSingleFixed;
	// Collection for relation list class with its
	// object id condition for signals mode
	string_set			m_setObjectId;
	// Collection for relation class with its
	// members and 'is/get/set' methods
	vector_node_map		m_mapMembers;
	// Collection for relation class with its
	// custom header code fragment
	string_multimap		m_mapHeaderFragments;
	// Collection for relation class with its
	// custom unit code fragment
	string_multimap		m_mapUnitFragments;
	// Collection for relation class with its
	// custom post reading XML code fragment
	string_multimap		m_mapPostReadFragments;
	// Collection for relation property with its
	// real method 'is/get/set' names
	int_map				m_mapMethodNames;
	// Collection for relation property virtual with its
	// real method 'is/get/set' names
	int_map				m_mapVirtualPrefixMethodNames;
	// Collection for relation property with its
	// private real method 'is/get'
	int_set				m_setPrivateGetterMethod;
	// Collection for relation property with its
	// private real method 'set'
	int_set				m_setPrivateSetterMethod;
	// Collection for relation property with its
	// attribute in parent element
	int_map				m_mapAttributes;
	// Collection for relation property with its
	// custom type
	int_map				m_mapTypes;
	// Collection for relation property with its
	// simple restriction type
	int_map				m_mapRestrictionTypes;
	// Collection for relation property with its
	// complex default value
	int_map				m_mapComplexDefaults;
	// Collection for relation class with its
	// custom post class information
	string_multimap		m_mapPostClassInfo;
	// Collection for relation class with its
	// custom prefix class information
	string_multimap		m_mapPrefixClassInfo;
	// Collection for relation property with its
	// minimum occurs
	int_map				m_mapMinOccurs;
	// Collection for relation property with its
	// maximum occurs
	int_map				m_mapMaxOccurs;
	// Collection for relation property with its
	// tag occuring condition
	int_map				m_mapConditions;
	// Collection for relation property with its
	// custom patch
	int_map				m_mapPatches;
	// Collection for relation class with its
	// restricted merge option
	string_set			m_setRestrictMergeClasses;

	// tag stands for base class name
	//
	// Note: double colon symbol (::) is added automatically
	// after base class name
	int_map				m_mapMethodBaseClassNames;

	// The classes that contain xs:any element
	string_set			m_setAnyClasses;

	// Error message or warnings
	QString		m_qsErrorMessage;

	// Optional flags
	QStringList	m_qslFlags;

};


#endif
