/*
 * PrlQuestionsList.cpp
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019, Virtuozzo International GmbH, All rights reserved
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


#include "PrlQuestionsList.h"
#include "PrlHandleVmEvent.h"
#include <prlcommon/Std/PrlAssert.h>

PrlQuestionsList::PrlQuestionsList()
: m_QuestionsListMutex(QMutex::Recursive)
{}

void PrlQuestionsList::RegisterQuestionObject(const PrlHandleVmEventPtr &pQuestion)
{
	QMutexLocker _lock(&m_QuestionsListMutex);
	if (pQuestion->GetEventObject().getInitRequestId().isEmpty())
		pQuestion->GetEventObject().setInitRequestId(Uuid::createUuid().toString());
	PRL_ASSERT(m_QuestionsList.find(pQuestion->GetEventObject().getInitRequestId()) == m_QuestionsList.end());
	m_QuestionsList[pQuestion->GetEventObject().getInitRequestId()] = pQuestion;
}

void PrlQuestionsList::UnregisterQuestionObject(const QString &sRequestId)
{
	QMutexLocker _lock(&m_QuestionsListMutex);
	QMap<QString, PrlHandleVmEventPtr>::iterator it = m_QuestionsList.find(sRequestId);
	if (it != m_QuestionsList.end())
		m_QuestionsList.erase(it);
}

QList<PrlHandleBasePtr> PrlQuestionsList::GetQuestionsList()
{
	QMutexLocker _lock(&m_QuestionsListMutex);
	QList<PrlHandleBasePtr> lstResult;
	QList<PrlHandleVmEventPtr> lstAllQuestions = m_QuestionsList.values();
	foreach(PrlHandleVmEventPtr pQuestion, lstAllQuestions)
		lstResult.append(PrlHandleBasePtr(pQuestion.getHandle()));
	return (lstResult);
}

QList<PrlHandleBasePtr> PrlQuestionsList::GetQuestionsByIssuerId(const QString &sIssuerId)
{
	QMutexLocker _lock(&m_QuestionsListMutex);
	QList<PrlHandleBasePtr> lstResult;
	QList<PrlHandleVmEventPtr> lstAllQuestions = m_QuestionsList.values();
	foreach(PrlHandleVmEventPtr pQuestion, lstAllQuestions)
		if (pQuestion->GetEventObject().getEventIssuerId() == sIssuerId)
			lstResult.append(PrlHandleBasePtr(pQuestion.getHandle()));
	return (lstResult);
}
