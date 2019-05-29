/*
 * PrlQuestionsList.h
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


#ifndef __PARALLELS_QUESTIONS_LIST_H__
#define __PARALLELS_QUESTIONS_LIST_H__


#include <QList>
#include <QMap>
#include <QMutex>
#include "PrlHandleBase.h"

/**
 * This class is implementation of synchronized questions list container.
 */
class PrlQuestionsList
{
public:
	/**
	 * Default class constructor
	 */
	PrlQuestionsList();

	/**
	 * Registries question handle at questions list
	 * @param registering question handle
	 */
	void RegisterQuestionObject(const PrlHandleVmEventPtr &pQuestion);

	/**
	 * Unregistries question handle(s) from questions list by specified initial request id
	 * @param initial request id
	 */
	void UnregisterQuestionObject(const QString &sRequestId);

	/**
	 * Returns all questions handles list
	 * @return all available questions list
	 */
	QList<PrlHandleBasePtr> GetQuestionsList();

	/**
	 * Returns questions handles list for specified issuer ID
	 * @param issue ID which questions list requesting
	 * @return questions list for corresponding issuer ID
	 */
	QList<PrlHandleBasePtr> GetQuestionsByIssuerId(const QString &sIssuerId);

private:
	/**
	 * Wrapping questions list
	 */
	QMap<QString, PrlHandleVmEventPtr> m_QuestionsList;

	/**
	 * Questions list access synchronization object
	 */
	QMutex m_QuestionsListMutex;
};

#endif // __PARALLELS_QUESTIONS_LIST_H__
