/*
 * PrlHandleCtTemplate.h
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

#ifndef __VIRTUOZZO_HANDLE_CT_TEMPLATE_H__
#define __VIRTUOZZO_HANDLE_CT_TEMPLATE_H__


#include "PrlHandleBase.h"
#include <prlxmlmodel/CtTemplate/CtTemplate.h>


/**
 * Handle object represents Container template
 */
class PrlHandleCtTemplate: public PrlHandleBase
{
public:

	/**
	 * Class default constructor.
	 */
	PrlHandleCtTemplate();

	/**
	 * Class initialize constructor.
	 */
	PrlHandleCtTemplate(QString sTemplate);

	/**
	 * Initialize object from XML string
	 */
	PRL_RESULT fromString(PRL_CONST_STR sXml);

	/**
	 * Returns object as XML string
	 */
	QString toString();

	/**
	 * Get template name
	 */
        PRL_RESULT GetName(PRL_STR sName, PRL_UINT32_PTR pnNameLength);

	/**
	 * Get template type
	 */
	PRL_RESULT GetTemplType(PRL_CT_TEMPLATE_TYPE_PTR pnType);

	/**
	 * Get template description
	 */
	PRL_RESULT GetDescription(PRL_STR sDescription, PRL_UINT32_PTR pnDescriptionBufLength);

	/**
	 * Get template version
	 */
	PRL_RESULT GetVersion(PRL_STR sVersion, PRL_UINT32_PTR pnVersionBufLength);

	/**
	 * Get template operating system type
	 */
	PRL_RESULT GetOsType(PRL_UINT32_PTR pnOsType);

	/**
	 * Get template operating system version
	 */
	PRL_RESULT GetOsVersion(PRL_UINT32_PTR pnOsVersion);

	/**
	 * Get template cpu mode (32bit or 64bit)
	 */
	PRL_RESULT GetCpuMode(PRL_CPU_MODE_PTR pnCpuMode);

	/**
	 * Get template cached or not
	 */
	PRL_RESULT IsCached(PRL_BOOL_PTR pbCached);

	/**
	 * Get Os template name for which this application template is
	 */
	PRL_RESULT GetOsTemplate(PRL_STR sOsTemplate, PRL_UINT32_PTR pnOsTemplateLength);

private:

	CtTemplate m_CtTemplate;
};


typedef PrlHandleSmartPtr<PrlHandleCtTemplate> PrlHandleCtTemplatePtr;


#endif	// __VIRTUOZZO_HANDLE_CT_TEMPLATE_H__
