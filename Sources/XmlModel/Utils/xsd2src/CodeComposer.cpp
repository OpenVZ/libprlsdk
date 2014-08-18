/*
 * CodeComposer.cpp: CodeComposer class implementation.
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


#include "CodeComposer.h"


/**
 * @brief This constructor assigns 'xs:schema' as the top QDomNode object
 * @param[in] xsd_parser XSD document
 */
CodeComposer::CodeComposer(const XSDParser& XSDParser, const QString& qsXSDName)
: m_qsXSDName(qsXSDName)
{
	QDomElement element = XSDParser.GetTopNode().firstChildElement();
	while(!element.isNull())
	{
		QString tag_name = element.tagName();
		if (tag_name.contains(":schema"))
		{
			m_domTopNode = element;
			break;
		}

		element = element.nextSiblingElement();
	}
}


/**
 * @brief Virtual destructor
 */
CodeComposer::~CodeComposer()
{
}


/**
 * @brief Composite source code using XML scheme (XSD)
 * @param[in] path Directory for storing the new source files
 * @param[in] LibraryName Compiling library name
 * @return boolean value: true - success, false - something wrong
 */
bool CodeComposer::CompositeSourceCode( QString Path)
{
// Check path

	QDir directory(Path);
	if (!directory.exists()
		&& !directory.mkpath(directory.absolutePath()))
	{
		m_qsErrorMessage = QString("Cannot create '%1' directory!\n").arg(Path);
		return false;
	}

// 1st step

	if (m_domTopNode.isNull())
	{
		m_qsErrorMessage = "Schema as top node does not exist!\n";
		return false;
	}

	AddElement(m_domTopNode);

	if (m_domRootNode.isNull())
	{
		m_qsErrorMessage = "Root element does not exist!\n";
		return false;
	}

// 2nd step

	QString class_name = *m_setClassNames.begin();
	AddRelations(m_domRootNode, class_name, m_domRootNode.lineNumber());

// 3rd step

	if (!CompositeSourceFiles(Path))
	{
		return false;
	}

// 4th step

	if (!CompositeProjectIncludeFile(Path))
	{
		return false;
	}

	return true;
}


/**
 * @brief Get error message
 * @return error message
 */
const QString& CodeComposer::GetErrorMessage() const
{
	return m_qsErrorMessage;
}


/**
 * Recursive method adds a new 'xs:element' and 'xs:complexType'
 * to the collection and finds root element
 * @param[in] parent_node Current node
 */
void CodeComposer::AddElement(const QDomNode& ParentNode)
{
	QString parent_tag_name;
	if (ParentNode.isElement())
	{
		QDomElement element = ParentNode.toElement();
		parent_tag_name = element.tagName();
	}

	QDomNodeList node_list = ParentNode.childNodes();
	for(int i = 0; i < node_list.size(); i++)
	{
		QDomNode node = node_list.at(i);
		if (node.isElement())
		{
			QDomElement element = node.toElement();

			QString tag_name = element.tagName();
			if (tag_name.contains(":element"))
			{
				QString name = element.attribute("name");
				if (!element.hasAttribute("ref"))
				{
					m_mapElements.insert(name, node);
				}

				if (m_domTopNode == node.parentNode()
					&& IsComplexElement(node))
				{
					m_domRootNode = node;
					m_setClassNames.insert(name);
				}
			}

			if (tag_name.contains(":complexType")
				&& element.hasAttribute("name")
				&& node.hasChildNodes())
			{
				QString name = element.attribute("name");
				if (m_mapComplexTypes.contains(name))
				{
					return;
				}
				m_mapComplexTypes.insert(name, node);
			}

			if (tag_name.contains(":simpleType")
				&& element.hasAttribute("name"))
			{
				QString name = element.attribute("name");
				m_mapSimpleTypes.insert(name, node);
			}

			if (tag_name.contains(":extension")
				&& element.hasAttribute("base")
				&& node.hasChildNodes()
				&& parent_tag_name.contains(":complexContent"))
			{
				QString base = element.attribute("base");
				if (m_mapComplexTypes.contains(base))
				{
					return;
				}
				m_mapComplexTypes.insert(base, node);
			}
		}

		AddElement(node);
	}
}


/**
 * @brief Recursive method adds new relations of the potential class
 * @param[in] ParentNode Current node
 * @param[in] ClassName Current class name
 * @param[in] LineNumber Current element parsing line number
 */
void CodeComposer::AddRelations(const QDomNode& ParentNode,
								const QString& ClassName,
								int LineNumber)
{
	QDomDocument temp_doc;
	QDomNodeList node_list = ParentNode.childNodes();
	for(int i = 0; i < node_list.size(); i++)
	{
		QDomNode node = node_list.at(i);

		QString new_class_name = ClassName;
		int line_number = LineNumber;

		if (node.isElement())
		{
			QDomElement element = node.toElement();

			QString tag_name = element.tagName();
			if (tag_name.contains(":element"))
			{
				line_number = node.lineNumber();
				if (IsComplexElement(node))
				{
					node_vector vector = m_mapLists.value(new_class_name);
					vector += node;
					m_mapLists.insert(new_class_name, vector);

					vector = m_mapMembers.value(new_class_name);
					vector += temp_doc.createComment("list member");
					m_mapMembers.insert(new_class_name, vector);

					new_class_name = element.attribute("name");
					m_setClassNames.insert(new_class_name);

					if (element.attribute("block") == "restriction")
						m_setRestrictMergeClasses.insert(new_class_name);
				}
				else
				{
					QString type = element.attribute("type");
					if (m_mapComplexTypes.contains(type))
					{
						node_vector vector = m_mapLists.value(new_class_name);
						vector += node;
						m_mapLists.insert(new_class_name, vector);

						vector = m_mapMembers.value(new_class_name);
						vector += temp_doc.createComment("list member");
						m_mapMembers.insert(new_class_name, vector);

						new_class_name = element.attribute("name");
						if (!m_setClassNames.contains(new_class_name))
						{
							AddRelations(node, new_class_name, line_number);

							m_setClassNames.insert(new_class_name);

							if (element.attribute("block") == "restriction")
								m_setRestrictMergeClasses.insert(new_class_name);

							node = m_mapComplexTypes.value(type);
						}
						else
						{
							return;
						}
					}
					else
					{
						node_vector vector = m_mapMembers.value(new_class_name);
						vector += node;
						m_mapMembers.insert(new_class_name, vector);

						if (element.hasAttribute("minOccurs"))
						{
							m_mapMinOccurs.insert(line_number, element.attribute("minOccurs"));
						}
						if (element.hasAttribute("maxOccurs"))
						{
							m_mapMaxOccurs.insert(line_number, element.attribute("maxOccurs"));
						}

						if (element.hasAttribute("ref"))
						{
							QString ref = element.attribute("ref");

							node = m_mapElements.value(ref);
							element = node.toElement();

							AddRelations(node, new_class_name, node.lineNumber());
						}

						if (m_mapSimpleTypes.contains(type))
						{
							node = m_mapSimpleTypes.value(type);
							element = node.toElement();
						}
					}
				}
			}
			else if (tag_name.contains(":extension"))
			{
				QString base = element.attribute("base");
				if (m_mapComplexTypes.contains(base))
				{
					node = m_mapComplexTypes.value(base);
				}
			}
			else if (tag_name.contains(":restriction"))
			{
				QString base = element.attribute("base");
				m_mapRestrictionTypes.insert(line_number, base);
			}
			else if (tag_name.contains(":any"))
			{
				m_setAnyClasses.insert(ClassName);
			}
			else if (tag_name.contains(":documentation"))
			{
				QDomNode text_node = element.firstChild();
				QDomText text = text_node.toText();
				QString class_info = text.data();

				QString header_fragment = "header_fragment:";
				QString unit_fragment = "unit_fragment:";
				QString post_read_fragment = "post_read_fragment:";
				QString post_class_info = "post_class_info:";
				QString prefix_class_info = "prefix_class_info:";
				QString condition = "condition:";
				QString patch = "patch:";

				if (class_info.left(header_fragment.size()) == header_fragment)
				{
					class_info = class_info.mid(header_fragment.size());
					m_mapHeaderFragments.insert(new_class_name, class_info);
				}
				else if (class_info.left(unit_fragment.size()) == unit_fragment)
				{
					class_info = class_info.mid(unit_fragment.size());
					m_mapUnitFragments.insert(new_class_name, class_info);
				}
				else if (class_info.left(post_read_fragment.size()) == post_read_fragment)
				{
					class_info = class_info.mid(post_read_fragment.size());
					m_mapPostReadFragments.insert(new_class_name, class_info);
				}
				else if (class_info.left(post_class_info.size()) == post_class_info)
				{
					class_info = class_info.mid(post_class_info.size());
					m_mapPostClassInfo.insert(new_class_name, class_info);
				}
				else if (class_info.left(prefix_class_info.size()) == prefix_class_info)
				{
					class_info = class_info.mid(prefix_class_info.size());
					m_mapPrefixClassInfo.insert(new_class_name, class_info);
				}
				else if (class_info.left(condition.size()) == condition)
				{
					class_info = class_info.mid(condition.size());
					m_mapConditions.insert(line_number, class_info);
				}
				else if (class_info.left(patch.size()) == patch)
				{
					class_info = class_info.mid(patch.size());
					m_mapPatches.insert(line_number, class_info);
				}
				else
				{
					QStringList list = class_info.split(';');
					for(int j = 0; j < list.size(); j++)
					{
						QString str = list.at(j);
						QStringList ls = str.split('=');
						if (ls.size() == 2)
						{
							QString str_key = ls[0];
							QString str_value = ls[1];

							if (str_key == "class")
							{
								m_mapRealClassNames.insert(new_class_name, str_value);
							}
							else if (str_key == "file_name")
							{
								m_mapRealFileNames.insert(new_class_name, str_value);
							}
							else if (str_key == "base_class")
							{
								m_mapBaseClassNames.insert(new_class_name, str_value);
							}
							else if (str_key == "sub_class")
							{
								m_mapSubClassNames.insert(new_class_name, str_value);
							}
							else if (str_key == "extern")
							{
								m_mapExternClassIncludes.insert(new_class_name, str_value);
							}
							else if (str_key == "include")
							{
								m_mapAdditionalIncludes.insert(new_class_name, str_value);
							}
							else if (str_key == "method")
							{
								m_mapMethodNames.insert(line_number, str_value);
							}
							else if (str_key == "virtual" && str_value == "on")
							{
								m_mapVirtualPrefixMethodNames.insert(line_number, str_key + " " );
							}
							else if (str_key == "private_getter" && str_value == "on")
							{
								m_setPrivateGetterMethod.insert(line_number);
							}
							else if (str_key == "private_setter" && str_value == "on")
							{
								m_setPrivateSetterMethod.insert(line_number);
							}
							else if (str_key == "type")
							{
								m_mapTypes.insert(line_number, str_value);
							}
							else if (str_key == "default")
							{
								m_mapComplexDefaults.insert(line_number, str_value);
							}
							else if (str_key == "list")
							{
								m_mapListNames.insert(new_class_name, str_value);
							}
							else if (str_key == "single_fixed" && str_value == "on")
							{
								m_setSingleFixed.insert(new_class_name);
							}
							else if (str_key == "object_id" && str_value == "on")
							{
								m_setObjectId.insert(new_class_name);
							}
							else if (str_key == "attribute")
							{
								m_mapAttributes.insert(line_number, str_value);
							}
							else if (str_key == "method_base_class" )
							{
								// method_base_class=[prefix::]method
								//
								// str_value value should be in the following format: [prefix::]method,
								// where
								// prefix - name of the base class;
								// method - name of the method;
								m_mapMethodBaseClassNames.insert(line_number, str_value);
							}
							else if (str_key == "no_save" && str_value == "on")
							{
								m_mapConditions.insert(line_number, " ! no_save_option ");
							}

							// More parameters
						}
					}
				}
			}
		}

		AddRelations(node, new_class_name, line_number);
	}
}


/**
 * @brief Check complex element or simple
 * @param[in] parent_node Checking node
 * @return boolean value: true - element is complex, false - simple
 */
bool CodeComposer::IsComplexElement(const QDomNode& ParentNode) const
{
	if (!ParentNode.hasChildNodes())
	{
		return false;
	}

	QDomNodeList node_list = ParentNode.childNodes();
	for(int i = 0; i < node_list.size(); i++)
	{
		QDomNode node = node_list.at(i);
		if (node.isElement())
		{
			QDomElement element = node.toElement();

			QString tag_name = element.tagName();
			if (tag_name.contains(":complexType"))
			{
				return true;
			}
		}
	}
	return false;
}


/**
 * @brief Composite project include file
 * @param[in] Path Directory where to save files
 * @return boolean value: true - success, false - fail
 */
bool CodeComposer::CompositeProjectIncludeFile(const QString& Path)
{
	QString content;

	content += QString(PRI_CAPTION).arg(m_qsXSDName);

	content += PRI_GEN_HEADERS;
	foreach(QString qsHeaderFile, m_lstHeaderFiles)
	{
		content += QString(PRI_FILE).arg(qsHeaderFile);
	}
	content += '\n';

	content += PRI_GEN_SOURCES;
	foreach(QString qsSourceFile, m_lstSourceFiles)
	{
		content += QString(PRI_FILE).arg(qsSourceFile);
	}
	content += '\n';

	content += QString(PRI_GEN_PRIFILES).arg(m_qsXSDName);
	content += '\n';

// Save project include file

	QString pri_file_name = Path + '/' + m_qsXSDName + ".pri";
	QFile pri_file(pri_file_name);

	if (!pri_file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		m_qsErrorMessage = QString("Cannot open project header file '%1'!\n").arg(pri_file_name);
		return false;
	}

	pri_file.write(content.toLatin1());

	return true;
}


/**
 * @brief Composite source files
 * @param[in] Path Directory where to save files
 * @return boolean value: true - success, false - fail
 */
bool CodeComposer::CompositeSourceFiles(const QString& Path)
{
	string_set::const_iterator it;
	for(it = m_setClassNames.begin(); it != m_setClassNames.end(); ++it)
	{
		QString class_name = *it;


		if (!m_mapSubClassNames.contains(class_name)
			&& m_mapExternClassIncludes.contains(class_name))
		{
				continue;
		}

		QString real_class_name,real_file_name;
		// Check if file name is present and take it into account the first
		if (m_mapRealFileNames.contains(class_name))
		{
			real_file_name = m_mapRealFileNames.value(class_name);
		}

		if (m_mapSubClassNames.contains(class_name))
		{
			real_class_name = m_mapSubClassNames.value(class_name);
		}
		else
		{
			real_class_name = m_mapRealClassNames.contains(class_name)
								? m_mapRealClassNames.value(class_name) : class_name;
		}

// Header file
		if (real_file_name.isEmpty() || real_file_name.isNull()) real_file_name=real_class_name;

		QString header_file_name = real_file_name + ".h";
		m_lstHeaderFiles += header_file_name;
		header_file_name = Path + '/' + header_file_name;

		QFile header_file(header_file_name);
		QString content = CompositeHeader(class_name, real_class_name);

		if (CheckFileContent(header_file, content))
		{
			if (!header_file.open(QIODevice::WriteOnly | QIODevice::Text))
			{
				m_qsErrorMessage = QString("Cannot open header file '%1'!\n").arg(header_file_name);
				return false;
			}

			header_file.write(content.toLatin1());
		}

// Unit file

		QString unit_file_name = real_file_name + ".cpp";
		m_lstSourceFiles += unit_file_name;
		unit_file_name = Path + '/' + unit_file_name;

		QFile unit_file(unit_file_name);
		content = CompositeUnit(class_name, real_class_name, real_file_name);

		if (CheckFileContent(unit_file, content))
		{
			if (!unit_file.open(QIODevice::WriteOnly | QIODevice::Text))
			{
				m_qsErrorMessage = QString("Cannot open unit file '%1'!\n").arg(unit_file_name);
				return false;
			}

			unit_file.write(content.toLatin1());
		}
	}

	return true;
}


/**
 * @brief Composite header file content
 * @param[in] class_name
 * @param[in] real_class_name
 * @return header file content
 */
QString CodeComposer::CompositeHeader(const QString& ClassName, const QString& RealClassName)
{
	QString content;
// Caption
	content = QString(HDR_CAPTION).arg(RealClassName);
// Pragma
	content += QString(HDR_PRAGMA).arg(RealClassName.toUpper(), RealClassName.toUpper());

// Includes

	content += HDR_INCLUDES;

	string_multimap::const_iterator it;
	for(it = m_mapAdditionalIncludes.lowerBound(ClassName);
		it != m_mapAdditionalIncludes.upperBound(ClassName);
		++it)
	{
		QString include = it.value();
// Additional includes
		content += QString(HDR_INCLUDE_ADDITIONAL).arg(include);
	}

	QString base_classes;
	bool was_base_class = false;
	for(it = m_mapBaseClassNames.lowerBound(ClassName);
		it != m_mapBaseClassNames.upperBound(ClassName);
		++it)
	{
		QString base_class_name = it.value();
		if (!base_class_name.contains('-'))
		{
// Base class includes
			content += QString(HDR_INCLUDE).arg(base_class_name);
		}
		base_class_name.remove('-');

		if (base_class_name.contains('^'))
		{
			base_class_name.remove('^');

			QString first_base_class = QString(HDR_BASE_CLASS).arg(base_class_name);
			first_base_class.remove(HDR_BASE_CLASS_IDENT);

			base_classes = first_base_class
				+ (was_base_class ? (HDR_BASE_CLASS_IDENT + base_classes) : base_classes);
		}
		else
		{
			base_classes += QString(HDR_BASE_CLASS).arg(base_class_name);
			if (!was_base_class)
			{
				base_classes.remove(HDR_BASE_CLASS_IDENT);
			}
		}

		was_base_class = true;
	}

	//content += '\n';
	for(it = m_mapPrefixClassInfo.lowerBound(ClassName);
		it != m_mapPrefixClassInfo.upperBound(ClassName);
		++it)
	{
// Prefix class info
		content += it.value();
	}
	//content += '\n';

	if (base_classes.isEmpty())
	{
// Default base class
		base_classes = IsSignalsEnabled() ? HDR_DEFAULT_SIGNALS_BASE_CLASS : HDR_DEFAULT_BASE_CLASS;
	}

	QString list_members;
	QString single_list_members;

	int i = -1;
	node_vector vector = m_mapLists.value(ClassName);
	for(i = 0; i < vector.size(); i++)
	{
		QDomNode node = vector[i];
		QDomElement element = node.toElement();

		QString list_class_name = element.attribute("name");
		QString list_real_class_name = m_mapRealClassNames.contains(list_class_name)
									? m_mapRealClassNames.value(list_class_name) : list_class_name;

		QString list_name = m_mapListNames.contains(list_class_name)
						? m_mapListNames.value(list_class_name) : list_class_name;
		QString method_name = m_mapMethodNames.contains(node.lineNumber())
						? m_mapMethodNames.value(node.lineNumber()) : list_class_name;

		QString virtual_prefix_method_name = m_mapVirtualPrefixMethodNames.contains(node.lineNumber())
						? m_mapVirtualPrefixMethodNames.value(node.lineNumber()) : QString();


		// Check if this member is marked as the member of base class
		// so exclude membership and get and set methods accordingly
		if ( ! m_mapMethodBaseClassNames.contains(node.lineNumber()) )
		{

			// List class member includes
			if (!m_mapExternClassIncludes.contains(list_class_name))
			{
				content += QString(HDR_INCLUDE).arg(list_real_class_name);
			}
			else
			{
				QString header_file = m_mapExternClassIncludes.value(list_class_name);
				content += QString(HDR_INCLUDE_EXTERN).arg(header_file);
			}


			list_members += QString(HDR_LIST_MEMBER).arg(list_real_class_name, list_name);

			QString min_occurs = element.attribute("minOccurs", "1");
			QString max_occurs = element.attribute("maxOccurs", "1");
			if (max_occurs.toInt() == 1)
			{
				single_list_members += QString(HDR_GET_SINGLE_LIST_MEMBER)
									.arg(virtual_prefix_method_name, list_real_class_name, method_name);
				single_list_members += QString(HDR_SET_SINGLE_LIST_MEMBER)
									.arg(virtual_prefix_method_name, method_name, list_real_class_name, list_name);
				single_list_members += '\n';
			}
		}

	}

// Class
	content += QString(HDR_CLASS).arg(RealClassName);
// Base classes
	content += base_classes;
// Public section
	if (IsSignalsEnabled())
		content += HDR_Q_OBJECT_CLASS_PUBLIC;
	else
		content += HDR_CLASS_PUBLIC;
// Constructors
	content += QString(HDR_CONSTRUCTORS)
				.arg(RealClassName, RealClassName, RealClassName,
					 RealClassName, RealClassName, RealClassName);
// Assign operator
	content += QString(HDR_ASSIGN_OPERATOR)
				.arg(RealClassName, RealClassName);
// Virtual destructor
	content += QString(HDR_VIRTUAL_DESTRUCTOR).arg(RealClassName);

//	content += '\n';
	for(it = m_mapHeaderFragments.lowerBound(ClassName);
		it != m_mapHeaderFragments.upperBound(ClassName);
		++it)
	{
// Custom code fragmnet
		content += it.value();
	}
	content += '\n';

// List members
	content += list_members;
	content += '\n';
	content += single_list_members;

	QString simple_members;
	QString signals_list;
	QString slots_list;
	QString declare_metatypes;
	QString get_method_id_list;

	vector = m_mapMembers.value(ClassName);
	for(i = 0; i < vector.size(); i++)
	{
		QDomNode node = vector[i];
		if (node.isComment())
		{
			continue;
		}
		int line_number = node.lineNumber();

		QString max_occurs = m_mapMaxOccurs.contains(line_number)
							? m_mapMaxOccurs.value(line_number) : QString("1");

		QDomElement element = node.toElement();
		if (element.hasAttribute("ref"))
		{
			QString ref = element.attribute("ref");

			node = m_mapElements.value(ref);
			line_number = node.lineNumber();

			element = node.toElement();
		}

		QString member_name = element.attribute("name");
		QString method_name = m_mapMethodNames.contains(line_number)
						? m_mapMethodNames.value(line_number) : member_name;

		QString virtual_prefix_method_name = m_mapVirtualPrefixMethodNames.contains(line_number)
						? m_mapVirtualPrefixMethodNames.value(line_number) : QString();
		bool private_getter = m_setPrivateGetterMethod.contains(node.lineNumber());
		bool private_setter = m_setPrivateSetterMethod.contains(node.lineNumber());


		QString type = element.attribute("type");
		if (type.isEmpty())
		{
			type = m_mapRestrictionTypes.value(line_number);
			if (type.isEmpty())
			{
				m_qsErrorMessage += QString("Warning: Tag '%1' has no type!\n").arg(member_name);
			}
		}

		QString default_value = element.attribute("default");

		bool is_list = (max_occurs == "unbounded" || max_occurs.toInt() > 1);
		bool is_custom_type = m_mapTypes.contains(line_number);
		if (is_custom_type)
		{
			type = m_mapTypes.value(line_number);
		}

		QString type_prefix;
		QString real_type =
			 ConvertType(type, is_list, is_custom_type, type_prefix, default_value.isEmpty() ? &default_value : 0);

// Methods 'is/get/set'

		// Check if this member is marked as the member of base class
		// so exclude membership and get and set methods accordingly
		if ( ! m_mapMethodBaseClassNames.contains(line_number) )
		{
			if (real_type == "bool")
			{
				if (private_getter)
					content += "\n"HDR_METHOD_PRIVATE_BEGIN;
				content += QString(HDR_IS_METHOD).arg(virtual_prefix_method_name, method_name);
				if (private_getter)
					content += HDR_METHOD_PRIVATE_END;
			}
			else
			{
				if (private_getter)
					content += "\n"HDR_METHOD_PRIVATE_BEGIN;
				content += QString(HDR_GET_METHOD).arg(virtual_prefix_method_name, real_type, method_name);
				if (private_getter)
					content += HDR_METHOD_PRIVATE_END;
			}

			if (private_setter)
				content += HDR_METHOD_PRIVATE_BEGIN"\n";
			content += QString(HDR_SET_METHOD).arg(virtual_prefix_method_name, method_name, real_type, default_value);
			if (private_setter)
				content += HDR_METHOD_PRIVATE_END;

			simple_members += QString(HDR_SIMPLE_MEMBER).arg(real_type, type_prefix, member_name);

			if (IsSignalsEnabled())
			{
				signals_list += QString(HDR_SIGNAL).arg(method_name, real_type);
				slots_list += QString(HDR_SLOT).arg(method_name, real_type);
			}
		}

		if (   (IsSignalsEnabled() || IsChangePropertiesEnabled())
			&& (is_custom_type || real_type.contains("QList<")) )
		{
			declare_metatypes += QString(HDR_DECLARE_METATYPE)
								.arg(qHash(real_type)).arg(real_type);
		}

		get_method_id_list += QString(HDR_GET_METHOD_ID).arg(method_name);
	}
// Commom methods
	content += HDR_DEFAULTS;
	if (IsChangePropertiesEnabled())
	{
		// Change properties
		content += HDR_PROPERTY;
		if ( ! get_method_id_list.isEmpty() )
			content += "\n" + get_method_id_list;
		content += HDR_LIST_IFACE;
	}
	content += HDR_XML;
	if (IsMergeEnabled())
	{
		// Merge
		content += QString(HDR_MERGE).arg(RealClassName);
	}
	if (IsChangePropertiesEnabled())
	{
		// Diff
		content += QString(HDR_DIFF).arg(RealClassName);
	}
// Any node getter
	if (m_setAnyClasses.contains(ClassName))
		content += HEADER_GET_NODE_CODE;

// Init lists
	content += HDR_INIT_LISTS;
// Clear lists
	content += HDR_CLEAR_LISTS;
// Crear lists in read xml
	content += HDR_CLEAR_LISTS_IN_READ_XML;
// Protected section
	content += HDR_CLASS_PROTECTED;
// Protected members
	content += simple_members;
	content += '\n';
// Any node storage
	if (m_setAnyClasses.contains(ClassName))
		content += UNIT_STORE_NODE_DECL;
// Copy
	content += QString(HDR_COPY).arg(RealClassName);
	if ( ! signals_list.isEmpty() )
	{
// Signals
		content += HDR_CLASS_SIGNALS;
		content += signals_list;
	}
	if ( ! slots_list.isEmpty() )
	{
// Slots
		content += HDR_CLASS_SLOTS;
		content += slots_list;
	}
// End class
	content += HDR_END_CLASS;
	if ( ! declare_metatypes.isEmpty() )
	{
// Declare metatypes
		content += declare_metatypes;
	}
	for(it = m_mapPostClassInfo.lowerBound(ClassName);
		it != m_mapPostClassInfo.upperBound(ClassName);
		++it)
	{
// Post class info
		content += it.value();
	}

// End header

	content += HDR_END;

	return content;
}


/**
 * @brief Composite unit file content
 * @param[in] class_name
 * @param[in] real_class_name
 * @return unit file content
 */
QString CodeComposer::CompositeUnit(const QString& ClassName, const QString& RealClassName, const QString& RealFileName)
{
	QString content;
// Caption
	content = QString(UNIT_CAPTION).arg(RealClassName);
// Include
	content += QString(UNIT_INCLUDE).arg(RealFileName);

	string_multimap::const_iterator it;
	QString base_classes;
	bool was_base_class = false;
	for(it = m_mapBaseClassNames.lowerBound(ClassName);
		it != m_mapBaseClassNames.upperBound(ClassName);
		++it)
	{
		QString base_class_name = it.value();
		base_class_name.remove('-');

		if (base_class_name.contains('^'))
		{
			base_class_name.remove('^');

			base_classes = (base_class_name + "()")
				+ (was_base_class ? ", " + base_classes : base_classes);
		}
		else
		{
			if (was_base_class)
			{
				base_classes += ", ";
			}
			base_classes += base_class_name + "()";
		}

		was_base_class = true;
	}

	if (base_classes.isEmpty())
	{
// Default base class
		base_classes = IsSignalsEnabled() ? DEFAULT_SIGNALS_BASE_CLASS"()" : DEFAULT_BASE_CLASS"()";
	}

	QString call_init_lists;
	QString call_clear_lists;
	QString call_clear_lists_in_read_xml;
	QString call_list_set_defaults;
	QString single_list_members;
	QString map_item_ids;
	QString sync_item_ids;
	QString tag_min_occurs_counters;
	QString patch_stamps;
	QString patch_members;
	QString check_min_occurs;
	QStringList list_member_to_dom_elements;
	QString member_sync_item_ids;
	QString list_dom_element_to_members;
	QString member_list_merges;
	QString member_list_diffs;
	QString copy_list_members;
	QString get_property_list_members;
	QString set_property_list_members;
	QString add_list_item_list_members;
	QString delete_list_item_list_members;

	int i = -1;
	node_vector vector = m_mapLists.value(ClassName);
	for(i = 0; i < vector.size(); i++)
	{
		QDomNode node = vector[i];
		int line_number = node.lineNumber();
		QDomElement element = node.toElement();

		QString list_class_name = element.attribute("name");
		QString list_real_class_name = m_mapRealClassNames.contains(list_class_name)
									? m_mapRealClassNames.value(list_class_name) : list_class_name;
		QString list_name = m_mapListNames.contains(list_class_name)
						? m_mapListNames.value(list_class_name) : list_class_name;
		QString method_name = m_mapMethodNames.contains(node.lineNumber())
						? m_mapMethodNames.value(node.lineNumber()) : list_class_name;

		QString min_occurs = element.attribute("minOccurs", "1");
		QString max_occurs = element.attribute("maxOccurs", "1");

		QString qsListName = "m_lst" + list_name;

		if (min_occurs.toInt() > 0)
		{
			call_init_lists += QString(UNIT_CALL_INIT_LIST).arg(list_real_class_name , min_occurs, qsListName);
		}

		call_clear_lists += QString(UNIT_CALL_CLEAR_LIST).arg(list_real_class_name , qsListName);

		if ( ! m_mapMethodBaseClassNames.contains(line_number) )
		{
			if (max_occurs.toInt() == 1)
			{
				single_list_members += QString(UNIT_GET_SINGLE_LIST_MEMBERS)
									.arg(list_real_class_name, RealClassName,
										 method_name, list_name, list_name);
				single_list_members += QString(UNIT_SET_SINGLE_LIST_MEMBERS)
									.arg(RealClassName, method_name, list_real_class_name,
										 list_name, list_real_class_name, list_name,
										 list_name, list_name, list_name);
			}
		}

		QString condition = "true";
		if (m_mapConditions.contains(line_number))
		{
			condition = m_mapConditions.value(line_number);
		}

		QString qsToDynListsAttr;
		QString qsMaxItemId;
		QString qsMaxItemId2;
		QString qsSetItemId;
		QString qsSetItemIdEx;
		QString qsSetItemIdAttr;
		QString qsSyncMaxItemId = "\n";
		QString qsSyncMaxItemId3 = "\n";

		if ( ! m_setSingleFixed.contains(list_class_name) )
		{
			qsToDynListsAttr = QString(UNIT_TO_DYN_LISTS_ATTR).arg(list_class_name);
			qsMaxItemId = QString(UNIT_MAX_ITEM_ID).arg(list_real_class_name, qsListName, list_class_name);
			qsMaxItemId2 = QString(UNIT_MAX_ITEM_ID_2).arg(list_real_class_name, qsListName, list_class_name);
			qsSetItemId = UNIT_SET_ITEM_ID;
			qsSetItemIdEx = UNIT_SET_ITEM_ID_EX;
			qsSetItemIdAttr = UNIT_SET_ITEM_ID_ATTR;
			qsSyncMaxItemId = QString(UNIT_SYNC_MAX_ITEM_ID).arg(list_class_name);
			qsSyncMaxItemId3 = QString(UNIT_SYNC_MAX_ITEM_ID_3).arg(list_class_name);

			map_item_ids += QString(UNIT_READ_XML_MAP_ITEM_ID)
							.arg(list_class_name, list_real_class_name, qsListName);
			sync_item_ids += QString(UNIT_SYNC_MAX_ITEM_ID_2).arg(list_class_name);
		}

		if (IsChangePropertiesEnabled())
		{
			QString qs3dArg;
			if ( m_setSingleFixed.contains(list_class_name) )
			{
				if (min_occurs.toInt() > 1)
					qs3dArg = ", i";
			}
			else
			{
				qs3dArg = ", object->getItemId()";
			}
			qsSetItemIdEx += QString(UNIT_MAKE_FULL_ITEM_ID).arg(list_class_name, qs3dArg);
		}

		list_member_to_dom_elements += qsToDynListsAttr + qsMaxItemId
										+ QString(UNIT_LIST_MEMBER_TO_DOM_ELEMENT)
										.arg(qsListName, condition, list_real_class_name)
										.arg(qsSetItemId, qsSetItemIdAttr)
										+ qsSyncMaxItemId;
		member_sync_item_ids += qsMaxItemId2
								+ QString(UNIT_LIST_MEMBER_SYNC_ITEM_IDS)
									.arg(qsListName, list_real_class_name, qsSetItemIdEx)
								+ qsSyncMaxItemId3;
		tag_min_occurs_counters += QString(UNIT_TAG_MIN_OCCURS_COUNTER).arg(list_name, min_occurs);
		if (   min_occurs.toInt() > 0
			&& !m_setSingleFixed.contains(list_class_name))
		{
			check_min_occurs += QString(UNIT_CHECK_MIN_OCCURS_ERROR)
							.arg(list_name, RealClassName, list_class_name, min_occurs);
		}
		else
		{
			QString unit_template = UNIT_CHECK_MIN_OCCURS_WARNING;
			if (m_setSingleFixed.contains(list_class_name))
			{
				unit_template.replace(">= 0", ">= 1");
			}
			check_min_occurs += QString(unit_template)
							.arg(list_name, RealClassName, list_class_name);
		}

		QString qsMakeFullItemId2;
		if (IsChangePropertiesEnabled())
		{
			QString qs3dArg;
			if ( m_setSingleFixed.contains(list_class_name) )
			{
				if (min_occurs.toInt() > 1)
					qs3dArg = QString(", %1 - %2_count").arg(min_occurs, list_name);
			}
			else
			{
				qs3dArg = ", object->getItemId()";
			}
			qsMakeFullItemId2 = QString(UNIT_MAKE_FULL_ITEM_ID_2).arg(list_class_name, qs3dArg);
		}

		if ( ! m_setSingleFixed.contains(list_class_name) )
		{
			list_dom_element_to_members += QString(UNIT_LIST_DOM_ELEMENT_TO_MEMBER)
									.arg(list_class_name, list_real_class_name, qsListName, list_name,
										qsMakeFullItemId2);

			call_clear_lists_in_read_xml += QString(UNIT_CHECK_KNOWN_LIST).arg(list_class_name)
											+ QString(UNIT_CALL_CLEAR_LIST)
											.arg(list_real_class_name, qsListName);

			if (IsMergeEnabled())
			{
				member_list_merges += QString(UNIT_MERGE_MEMBER_LIST)
								.arg(list_real_class_name, qsListName, list_class_name);
			}

			if (IsChangePropertiesEnabled())
			{
				member_list_diffs += QString(UNIT_DIFF_LIST_MEMBER_DYN)
								.arg(list_real_class_name, qsListName);
			}
		}
		else
		{
			list_dom_element_to_members += QString(UNIT_LIST_DOM_ELEMENT_TO_MEMBER_SINGLE_FIXED)
										.arg(list_class_name, list_name, list_real_class_name, qsListName,
											 min_occurs, qsMakeFullItemId2 );

			call_list_set_defaults += QString(UNIT_CALL_LIST_SET_DEFAULT)
									.arg(qsListName);

			if (IsMergeEnabled())
			{
				member_list_merges += QString(UNIT_MERGE_MEMBER_LIST_SINGLE_FIXED).arg(qsListName);
			}

			if (IsChangePropertiesEnabled())
			{
				member_list_diffs += QString(UNIT_DIFF_LIST_MEMBER_SF)
								.arg(qsListName);
			}
		}

		copy_list_members += QString(UNIT_COPY_LIST_MEMBER)
							.arg(list_real_class_name, qsListName);

		if (IsChangePropertiesEnabled())
		{
			if (min_occurs == max_occurs && min_occurs == "1")
			{
				get_property_list_members += QString(UNIT_GET_PROPERTY_LIST_MEMBER)
										.arg(list_class_name, qsListName);
				set_property_list_members += QString(UNIT_SET_PROPERTY_LIST_MEMBER)
										.arg(list_class_name, qsListName);
				add_list_item_list_members += QString(UNIT_ADD_LIST_ITEM_SF)
										.arg(list_class_name, qsListName);
				delete_list_item_list_members += QString(UNIT_DELETE_LIST_ITEM_SF)
										.arg(list_class_name, qsListName);
			}
			else
			{
				get_property_list_members += QString(UNIT_GET_PROPERTY_DYN_LIST_MEMBER).arg(list_class_name);
				set_property_list_members += QString(UNIT_SET_PROPERTY_DYN_LIST_MEMBER).arg(list_class_name);
				delete_list_item_list_members += QString(UNIT_DELETE_LIST_ITEM_DYN_LIST).arg(list_class_name);

				if ( ! m_setSingleFixed.contains(list_class_name) )
				{
					get_property_list_members += QString(UNIT_GET_PROPERTY_DYN_LIST_MEMBER_IMPL).arg(qsListName);
					get_property_list_members += QString(UNIT_GET_PROPERTY_DYN_LIST_MEMBER_MAX_ITEM_ID)
													.arg(list_class_name, qsListName);
					get_property_list_members += QString(UNIT_GET_PROPERTY_DYN_LIST_MEMBER_LIST_ITEM_IDS)
													.arg(list_class_name, qsListName);
					set_property_list_members += QString(UNIT_SET_PROPERTY_DYN_LIST_MEMBER_IMPL).arg(qsListName);

					add_list_item_list_members += QString(UNIT_ADD_LIST_ITEM_IMPL)
													.arg(list_class_name, qsListName, list_real_class_name);
					add_list_item_list_members += QString(UNIT_ADD_LIST_ITEM_DYN_LIST).arg(list_class_name);
					add_list_item_list_members += QString(UNIT_ADD_LIST_ITEM_DYN_LIST_IMPL).arg(qsListName);
					delete_list_item_list_members += QString(UNIT_DELETE_LIST_ITEM_DYN_LIST_IMPL).arg(qsListName);
				}
				else
				{
					get_property_list_members += QString(UNIT_GET_PROPERTY_DYN_LIST_MEMBER_SF_IMPL).arg(qsListName);
					set_property_list_members += QString(UNIT_SET_PROPERTY_DYN_LIST_MEMBER_SF_IMPL).arg(qsListName);

					add_list_item_list_members += QString(UNIT_ADD_LIST_ITEM_DYN_LIST).arg(list_class_name);
					add_list_item_list_members += QString(UNIT_ADD_LIST_ITEM_DYN_LIST_SF_IMPL).arg(qsListName);
					delete_list_item_list_members += QString(UNIT_DELETE_LIST_ITEM_DYN_LIST_SF_IMPL).arg(qsListName);
				}

				get_property_list_members += UNIT_GET_PROPERTY_DYN_LIST_MEMBER_END;
				set_property_list_members += UNIT_SET_PROPERTY_DYN_LIST_MEMBER_END;
				add_list_item_list_members += UNIT_ADD_LIST_ITEM_DYN_LIST_END;
				delete_list_item_list_members += UNIT_DELETE_LIST_ITEM_DYN_LIST_END;
			}
		}
	}

// Default constructor
	content += QString(UNIT_DEFAULT_CONSTRUCTOR).arg(RealClassName, RealClassName);
// Copy constructor
	content += QString(UNIT_COPY_CONSTRUCTOR).arg(RealClassName, RealClassName, RealClassName, base_classes);
// Pointer constructor
	content += QString(UNIT_POINTER_CONSTRUCTOR).arg(RealClassName, RealClassName, RealClassName);
// Assign operator
	content += QString(UNIT_ASSIGN_OPERATOR).arg(RealClassName, RealClassName, RealClassName);
// Initialize constructor
//	content += QString(UNIT_INITIALIZE_CONSTRUCTOR).arg(RealClassName, RealClassName);
// File constructor
	content += QString(UNIT_FILE_CONSTRUCTOR).arg(RealClassName, RealClassName);
// Virtual destructor
	content += QString(UNIT_VIRTUAL_DESTRUCTOR).arg(RealClassName, RealClassName);

	for(it = m_mapUnitFragments.lowerBound(ClassName);
		it != m_mapUnitFragments.upperBound(ClassName);
		++it)
	{
// Custom code fragmnet
		content += it.value();
	}
	content += '\n';

// Get single list member
	content += single_list_members;

	QString call_set_methods;
	QString register_metatypes;
	QString member_to_dom_elements;
	QString dom_element_to_members;
	QString unit_template;
	QString member_merges;
	QString member_diffs;
	QString copy_members;
	QString get_property_members = UNIT_GET_PROPERTY_ITEM_ID;
	QString set_property_members;
	QString get_method_id_list;
	int list_index = 0;

// Any node storage
	if (m_setAnyClasses.contains(ClassName))
		member_to_dom_elements += UNIT_SAVE_NODE_CODE;

	vector = m_mapMembers.value(ClassName);
	for(i = 0; i < vector.size(); i++)
	{
		QDomNode node = vector[i];
		if (node.isComment())
		{
			QDomComment comment = node.toComment();
			if (comment.data() == "list member")
			{
				member_to_dom_elements += list_member_to_dom_elements[list_index];
				++list_index;
			}
			continue;
		}

		int line_number = node.lineNumber();

		QString min_occurs = m_mapMinOccurs.contains(line_number)
							? m_mapMinOccurs.value(line_number) : QString("1");
		QString max_occurs = m_mapMaxOccurs.contains(line_number)
							? m_mapMaxOccurs.value(line_number) : QString("1");

		QDomElement element = node.toElement();
		if (element.hasAttribute("ref"))
		{
			QString ref = element.attribute("ref");

			node = m_mapElements.value(ref);
			line_number = node.lineNumber();

			element = node.toElement();
		}

		QString member_name = element.attribute("name");
		QString method_name = m_mapMethodNames.contains(line_number)
						? m_mapMethodNames.value(line_number) : member_name;
		QString complex_default = m_mapComplexDefaults.contains(line_number)
						? m_mapComplexDefaults.value(line_number) : QString();
		QString attribute = m_mapAttributes.contains(line_number)
						? m_mapAttributes.value(line_number) : member_name;
		QString type = element.attribute("type");
		if (type.isEmpty())
		{
			type = m_mapRestrictionTypes.value(line_number);
		}

		bool is_list = (max_occurs == "unbounded" || max_occurs.toInt() > 1);
		bool is_custom_type = m_mapTypes.contains(line_number);
		if (is_custom_type)
		{
			type = m_mapTypes.value(line_number);
		}

		QString type_prefix;
		QString convert_value_method;
		QString convert_from_string;
		QString validate;

		QString real_type = ConvertType(type, is_list, is_custom_type, type_prefix,
										0, &convert_value_method, &convert_from_string,
										&validate);

// Methods 'is/get/set'

		// Check if this member is marked as the member of base class
		// so exclude membership and get and set methods accordingly
		if ( ! m_mapMethodBaseClassNames.contains(line_number) )
		{

			if (real_type == "bool")
			{
				content += QString(UNIT_IS_METHOD).arg(RealClassName, method_name, member_name);
			}
			else
			{
				content += QString(UNIT_GET_METHOD)
							.arg(real_type, RealClassName, method_name, type_prefix, member_name);
			}

			if (IsSignalsEnabled())
			{
				content += QString(UNIT_SET_METHOD_WITH_SIGNAL)
									.arg(RealClassName, method_name, real_type,
											type_prefix, member_name,
											method_name,
											m_setObjectId.contains(ClassName) ? ", getObjectId()" : "",
											validate);
				content += QString(UNIT_SLOT_METHOD)
							.arg(RealClassName, method_name, real_type, method_name);
			}
			else
			{
				content += QString(UNIT_SET_METHOD).arg(RealClassName, method_name, real_type,
													type_prefix, member_name, validate);
			}
		}

		QString qsSetMethodName = "set" + method_name;
		QString qsGetMethodName = (real_type == "bool" ? "is" : "get") + method_name;
		QString qsMethodBaseClass = m_mapMethodBaseClassNames.value(line_number);
		if ( ! qsMethodBaseClass.isEmpty() )
			qsMethodBaseClass += "::";

		if (   (IsSignalsEnabled() || IsChangePropertiesEnabled())
			&& (is_custom_type || real_type.contains("QList<")) )
		{
			register_metatypes += QString(UNIT_REGISTER_METATYPE).arg(real_type, real_type);
		}

		if (real_type.contains("QList<"))
		{
			call_set_methods += QString(UNIT_CALL_SET_METHOD_LIST).arg(member_name, qsSetMethodName, complex_default);

			QString qsToDynListsAttr = QString(UNIT_TO_DYN_LISTS_ATTR).arg(member_name);

			unit_template = UNIT_MEMBER_TO_DOM_ELEMENT_LIST;
			if (type.contains(":anyType"))
			{
				unit_template.replace("text_element", "cdata_element");
				unit_template.replace("->createTextNode", "->createCDATASection");
			}
			member_to_dom_elements += qsToDynListsAttr + QString(unit_template)
									.arg(member_name, member_name, member_name)
									.arg(convert_value_method)
									.replace("#", "%1");

			unit_template = UNIT_DOM_ELEMENT_TO_MEMBER_LIST;
			if (type.contains(":anyType"))
			{
				unit_template.replace("text_element", "cdata_element");
				unit_template.replace(".toText", ".toCDATASection");
				unit_template.replace(".createTextNode", ".createCDATASection");
				convert_from_string.replace("text_element", "cdata_element");
			}
			dom_element_to_members += QString(unit_template)
									.arg(member_name).arg(member_name)
									.arg(convert_from_string).arg(member_name);
		}
		else
		{
			call_set_methods += QString(UNIT_CALL_SET_METHOD).arg(member_name, qsSetMethodName, complex_default);

			QString unit_template = UNIT_MEMBER_TO_DOM_ELEMENT;
			if (type.contains(":anyType"))
			{
				unit_template.replace("text_element", "cdata_element");
				unit_template.replace("->createTextNode", "->createCDATASection");
			}

			QString to_dom_element = QString(unit_template)
											.arg(member_name, qsGetMethodName, convert_value_method);

			if (m_mapConditions.contains(line_number))
			{
				to_dom_element.replace("\t", "\t\t");
				QString condition = m_mapConditions.value(line_number);
				to_dom_element = QString(UNIT_CONDITION).arg(condition, to_dom_element);
			}
			member_to_dom_elements += to_dom_element.replace("#", "%1");

			unit_template = UNIT_DOM_ELEMENT_TO_MEMBER;
			if (type.contains(":anyType"))
			{
				unit_template.replace("text_element", "cdata_element");
				unit_template.replace(".toText", ".toCDATASection");
				unit_template.replace(".createTextNode", ".createCDATASection");
				convert_from_string.replace("text_element", "cdata_element");
			}

			dom_element_to_members += QString(unit_template)
									.arg(attribute, member_name, min_occurs,
										 member_name, qsSetMethodName, convert_from_string,
										 member_name);
		}

		if (m_mapPatches.contains(line_number))
		{
			member_to_dom_elements += QString(UNIT_PATCH_TO_DOM_ELEMENT_ATTRIBUTE);
			patch_stamps += QString(UNIT_PATCH_STAMP).arg(member_name);
			patch_members += QString(UNIT_PATCH_MEMBER)
							.arg(m_mapPatches.value(line_number), member_name, member_name);
		}

		tag_min_occurs_counters += QString(UNIT_TAG_MIN_OCCURS_COUNTER).arg(member_name, min_occurs);

		if (min_occurs.toInt() > 0)
		{
			check_min_occurs += QString(UNIT_CHECK_MIN_OCCURS_ERROR)
							.arg(member_name, RealClassName, member_name, min_occurs);

		}
		else
		{
			check_min_occurs += QString(UNIT_CHECK_MIN_OCCURS_WARNING)
							.arg(member_name, RealClassName, member_name);
		}

		if (IsChangePropertiesEnabled())
		{
			if (is_custom_type && convert_value_method.isEmpty()
				&& ! real_type.contains("QList<"))
			{
				get_property_members += QString(UNIT_GET_PROPERTY_PATCH_STAMP)
									.arg(member_name);
				get_property_members += QString(UNIT_GET_PROPERTY_ENUM_MEMBER)
									.arg(member_name, qsGetMethodName, qsMethodBaseClass);
				set_property_members += QString(UNIT_SET_PROPERTY_PATCH_STAMP)
									.arg(member_name);
				set_property_members += QString(UNIT_SET_PROPERTY_ENUM_MEMBER)
									.arg(member_name, qsGetMethodName,
											real_type, qsSetMethodName, qsMethodBaseClass);
			}
			else
			{
				get_property_members += QString(UNIT_GET_PROPERTY_PATCH_STAMP)
									.arg(member_name);
				get_property_members += QString(UNIT_GET_PROPERTY_MEMBER)
									.arg(member_name, qsGetMethodName, qsMethodBaseClass);
				set_property_members += QString(UNIT_SET_PROPERTY_PATCH_STAMP)
									.arg(member_name);
				set_property_members += QString(UNIT_SET_PROPERTY_MEMBER)
									.arg(member_name, qsGetMethodName,
											real_type, qsSetMethodName, qsMethodBaseClass);
			}

			get_method_id_list += QString(UNIT_GET_METHOD_ID).arg(RealClassName, method_name, member_name);
		}

		if (IsMergeEnabled())
		{
			if (element.attribute("fixed") == "read_only_for_merge")
				member_merges += QString(UNIT_MERGE_FIXED_FIELD)
						.arg(qsGetMethodName, qsSetMethodName);
			member_merges += QString(UNIT_MERGE_MEMBER_COPY)
						.arg(qsGetMethodName, qsSetMethodName);
			member_merges += QString(UNIT_MERGE_MEMBER_CONFLICT)
						.arg(qsGetMethodName, member_name);
		}

		if (IsChangePropertiesEnabled())
		{
			member_diffs += QString(UNIT_DIFF_MEMBER).arg(qsGetMethodName, method_name);
		}

		QString qsOrigValue = qsGetMethodName + "()";
		if (real_type == "QUrl")
			qsOrigValue += convert_value_method;

		copy_members += QString(UNIT_COPY_MEMBER)
						.arg(qsSetMethodName, qsOrigValue);
	}
// Set defaults
	content += QString(UNIT_SET_DEFAULTS).arg(RealClassName);
	content += call_set_methods;
	if ( ! register_metatypes.isEmpty() )
	{
		content += UNIT_ONCE_CONDITION;
		content += register_metatypes;
		content += UNIT_END_ONCE_CONDITION;
	}
	content += UNIT_CLOSE_BRACE;
// Change properties
	if ( IsChangePropertiesEnabled() )
	{
		content += QString(UNIT_GET_PROPERTY).arg(RealClassName);
		content += get_property_members;
		content += get_property_list_members;
		content += UNIT_RETURN_GET_PROPERTY;
		content += QString(UNIT_SET_PROPERTY).arg(RealClassName);
		content += set_property_members;
		content += set_property_list_members;
		content += UNIT_RETURN_SET_PROPERTY;

		if ( ! get_method_id_list.isEmpty() )
			content += get_method_id_list + "\n";

		content += QString(UNIT_ADD_LIST_ITEM).arg(RealClassName);
		content += add_list_item_list_members;
		content += UNIT_RETURN_ADD_LIST_ITEM;
		content += QString(UNIT_DELETE_LIST_ITEM).arg(RealClassName);
		content += delete_list_item_list_members;
		content += UNIT_RETURN_DELETE_LIST_ITEM;
	}
// Get XML
	content += QString(UNIT_GET_XML).arg(RealClassName, ClassName);
	content += member_to_dom_elements;
	content += UNIT_GET_XML_RETURN;
	content += UNIT_CLOSE_BRACE;
// Read XML

	content += QString(UNIT_READ_XML).arg(RealClassName, ClassName, RealClassName);
	content += map_item_ids + '\n';
	content += UNIT_READ_XML_CLEAR;

	content += tag_min_occurs_counters;
	content += '\n';
	if (!patch_stamps.isEmpty())
	{
		content += patch_stamps;
		content += '\n';
	}
// Any node storage
	if (m_setAnyClasses.contains(ClassName))
		content += UNIT_STORE_NODE_CODE;
	content += UNIT_READ_XML_LOOP;
	if (!dom_element_to_members.isEmpty())
	{
		content += UNIT_READ_XML_LOOP_INIT;
	}
	content += dom_element_to_members;
	content += list_dom_element_to_members;
	content += UNIT_READ_XML_END;
	content += check_min_occurs;

	content += sync_item_ids + '\n';
	content += patch_members;

	for(it = m_mapPostReadFragments.lowerBound(ClassName);
		it != m_mapPostReadFragments.upperBound(ClassName);
		++it)
	{
// Custom code fragment
		content += it.value();
	}
	content += '\n';

	content += UNIT_READ_XML_RETURN;
	content += UNIT_CLOSE_BRACE;
// Sync item ids
	content += QString(UNIT_SYNC_ITEM_IDS).arg(RealClassName);
	content += member_sync_item_ids;
	content += UNIT_CLOSE_BRACE;

	if (IsMergeEnabled())
	{
		// Merge
		content += QString(UNIT_MERGE).arg(RealClassName, ClassName);
		if (m_setRestrictMergeClasses.contains(ClassName))
			content += UNIT_MERGE_RESTRICT;
		content += member_merges;
		content += member_list_merges;
		content += UNIT_MERGE_RETURN;
		content += UNIT_CLOSE_BRACE;
	}
	if (IsChangePropertiesEnabled())
	{
		// Diff
		content += QString(UNIT_DIFF).arg(RealClassName);
		content += member_diffs;
		content += member_list_diffs;
		content += UNIT_CLOSE_BRACE;
	}
// Copy
	content += QString(UNIT_COPY).arg(RealClassName, RealClassName);
	content += copy_members + "\n";
	content += copy_list_members;
	content += UNIT_COPY_CUSTOM_CALL;
	content += UNIT_CLOSE_BRACE;
// Init lists
	content += QString(UNIT_INIT_LISTS).arg(RealClassName);
	content += call_init_lists;
	content += UNIT_CUSTOM_INIT;
	content += UNIT_CLOSE_BRACE;
// Clear lists
	content += QString(UNIT_CLEAR_LISTS).arg(RealClassName);
	content += call_clear_lists;
	content += UNIT_CLOSE_BRACE;
// Clear list in read Xml
	content += QString(UNIT_CLEAR_LISTS_IN_READ_XML).arg(RealClassName);
	content += call_clear_lists_in_read_xml;
	content += call_list_set_defaults;
	content += UNIT_CLOSE_BRACE;

	return content;
}


/**
 * @brief Convert XSD simple type to real type
 * @param[in] type XSD simple type
 * @param[in] IsList List type
 * @param[in] IsCustomType Custom type
 * @param[out] type_prefix Type prefix
 * @param[out] pdefault_value Default value
 * @param[out] pconvert_value_method Conversion method of value to string
 * @param[out] pconvert_from_string Conversion from string to native type
 * @return real type
 */
QString CodeComposer::ConvertType(const QString& Type,
								  bool IsList,
								  bool IsCustomType,
								  QString& TypePrefix,
								  QString* pDefaultValue,
								  QString* pConvertValueMethod,
								  QString* pConvertFromString,
								  QString* pValidate) const
{
	QString real_type = "QByteArray";
	QString default_value = "QByteArray()";
	TypePrefix = "ba";
	QString convert_value_method = ".toBase64().data()";
	QString convert_from_string = "QByteArray::fromBase64( text_element.data().toLatin1() )";
	QString validate;

	if (IsCustomType)
	{
		real_type = Type;

		if (real_type == "QHostAddress")
		{
			TypePrefix = "ha";
			default_value = "QHostAddress()";
			convert_value_method = ".toString()";
			convert_from_string = "QHostAddress( text_element.data() )";
		}
		else if (real_type == "QDateTime")
		{
			TypePrefix = "dt";
			default_value = "QDateTime::currentDateTime()";
			convert_value_method = ".toUTC().toString(XML_DATETIME_FORMAT)";
			convert_from_string = "toLocalTime( QDateTime::fromString( text_element.data(), XML_DATETIME_FORMAT ) )";
			validate = "\tQTime t = value.time(); t.setHMS(t.hour(), t.minute(), t.second()); value.setTime(t);\n";
		}
		else
		{
			TypePrefix = "ct";
			default_value = "(" + Type + " )0";
			convert_value_method = "";
			convert_from_string = "(" + Type + " )text_element.data().toLongLong()";
		}
	}
	else if (Type.endsWith(":boolean"))
	{
		real_type = "bool";
		TypePrefix = "b";
		default_value = "false";
		convert_value_method = "";
		convert_from_string = "text_element.data().toInt() != 0";
	}
	else if (Type.endsWith(":byte"))
	{
		real_type = "char";
		TypePrefix = "c";
		default_value = "0";
		convert_value_method = "";
		convert_from_string = "(char )text_element.data().toInt()";
	}
	else if (Type.endsWith(":unsignedByte"))
	{
		real_type = "unsigned char";
		TypePrefix = "uc";
		default_value = "0";
		convert_value_method = "";
		convert_from_string = "(unsigned char )text_element.data().toUInt()";
	}
	else if (Type.endsWith(":short"))
	{
		real_type = "short";
		TypePrefix = "s";
		default_value = "0";
		convert_value_method = "";
		convert_from_string = "(short )text_element.data().toInt()";
	}
	else if (Type.endsWith(":unsignedShort"))
	{
		real_type = "unsigned short";
		TypePrefix = "us";
		default_value = "0";
		convert_value_method = "";
		convert_from_string = "(unsigned short )text_element.data().toUInt()";
	}
	else if (   Type.endsWith(":int")
			 || Type.endsWith(":negativeInteger")
			 || Type.endsWith(":nonPositiveInteger"))
	{
		real_type = "int";
		TypePrefix = "i";
		default_value = "0";
		convert_value_method = "";
		convert_from_string = "text_element.data().toInt()";
	}
	else if (   Type.endsWith(":unsignedInt")
			 || Type.endsWith(":integer")
			 || Type.endsWith(":nonNegativeInteger")
			 || Type.endsWith(":positiveInteger"))
	{
		real_type = "unsigned int";
		TypePrefix = "ui";
		default_value = "0";
		convert_value_method = "";
		convert_from_string = "text_element.data().toUInt()";
	}
	else if (Type.endsWith(":long"))
	{
		real_type = "qlonglong";
		TypePrefix = "ll";
		default_value = "0";
		convert_value_method = "";
		convert_from_string = "text_element.data().toLongLong()";
	}
	else if (Type.endsWith(":unsignedLong"))
	{
		real_type = "qulonglong";
		TypePrefix = "ull";
		default_value = "0";
		convert_value_method = "";
		convert_from_string = "text_element.data().toULongLong()";
	}
	else if (   Type.endsWith(":decimal")
			 || Type.endsWith(":double")
			 || Type.endsWith(":float"))
	{
		real_type = "double";
		TypePrefix = "d";
		default_value = "0.0";
		convert_value_method = "";
		convert_from_string = "text_element.data().toDouble()";
	}
	else if (   Type.endsWith(":string")
			 || Type.endsWith(":normalizedString")
			 || Type.endsWith(":token")
			 || Type.endsWith(":language")
			 || Type.endsWith(":anyType"))
	{
		real_type = "QString";
		TypePrefix = "qs";
		default_value = "QString()";
		convert_value_method = "";
		convert_from_string = "text_element.data()";
	}
	else if (Type.endsWith(":anyURI"))
	{
		real_type = "QUrl";
		TypePrefix = "url";
		default_value = "QUrl()";
		convert_value_method = ".toString()";
		convert_from_string = "text_element.data()";
	}
	else if (Type.endsWith(":date"))
	{
		real_type = "QDate";
		TypePrefix = "date";
		default_value = "QDate::currentDate()";
		convert_value_method = ".toString(XML_DATE_FORMAT)";
		convert_from_string = "QDate::fromString( text_element.data(), XML_DATE_FORMAT )";
	}
	else if (Type.endsWith(":time"))
	{
		real_type = "QTime";
		TypePrefix = "time";
		default_value = "QTime::currentTime()";
		convert_value_method = ".toString(XML_TIME_FORMAT)";
		convert_from_string = "QTime::fromString( text_element.data(), XML_TIME_FORMAT )";
		validate = "\tvalue.setHMS(value.hour(), value.minute(), value.second());\n";
	}
	else if (Type.endsWith(":dateTime"))
	{
		real_type = "QDateTime";
		TypePrefix = "dt";
		default_value = "QDateTime::currentDateTime()";
		convert_value_method = ".toUTC().toString(XML_DATETIME_FORMAT)";
		convert_from_string = "toLocalTime( QDateTime::fromString( text_element.data(), XML_DATETIME_FORMAT ) )";
		validate = "\tQTime t = value.time(); t.setHMS(t.hour(), t.minute(), t.second()); value.setTime(t);\n";
	}

	if (IsList)
	{
		real_type = "QList<" + real_type + ">";
		TypePrefix = "lst";
	}

	if (pDefaultValue)
	{
		if (IsList)
		{
			default_value = real_type + "()";
		}
		*pDefaultValue = default_value;
	}

	if (pConvertValueMethod)
	{
		*pConvertValueMethod = convert_value_method;
	}

	if (pConvertFromString)
	{
		*pConvertFromString = convert_from_string;
	}

	if (pValidate)
	{
		*pValidate = validate;
	}

	return real_type;
}


/**
 * @brief Check file content
 * @param[in] File
 * @param[in] Content
 * @return boolean true - write file, false - no write
 */
bool CodeComposer::CheckFileContent(QFile& File, const QString& Content) const
{
	if (!File.open(QIODevice::ReadOnly))
	{
		return true;
	}

	QString old_content = File.readAll();
	old_content.replace("\r\n", "\n");

	File.close();
	return old_content != Content;
}

bool CodeComposer::IsSignalsEnabled() const
{
	return m_qslFlags.contains(QString("--signals")) || m_qslFlags.contains(QString("-s"));
}

bool CodeComposer::IsMergeEnabled() const
{
	return m_qslFlags.contains(QString("--merge")) || m_qslFlags.contains(QString("-m"));
}

bool CodeComposer::IsChangePropertiesEnabled() const
{
	return m_qslFlags.contains(QString("--properties")) || m_qslFlags.contains(QString("-p"));
}

// Set optional flag
void CodeComposer::SetFlag(const QString& qsFlag)
{
	if ( ! m_qslFlags.contains( qsFlag ) ) m_qslFlags.append(qsFlag);
}
