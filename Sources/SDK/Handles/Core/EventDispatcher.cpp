/*
 * EventDispatcher.cpp
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


#include <QList>
#include <QMutex>
#include "PrlHandleBase.h"
#include "EventDispatcher.h"

class EventDispatcher::Handler
{
public:
	Handler(void *const owner,
			const PRL_EVENT_HANDLER_PTR callback,
			void *const data):
		m_owner(owner), m_callback(callback), m_data(data)
	{}
	void *owner() const { return m_owner; }
	PRL_EVENT_HANDLER_PTR callback() const { return m_callback; }
	void *data() const { return m_data; }
	void call(PrlHandleBase *const evt) const;
	bool operator==(const Handler &other) const;
private:
	void *m_owner;
	PRL_EVENT_HANDLER_PTR m_callback;
	void *m_data;
};

void EventDispatcher::Handler::call(PrlHandleBase *const event) const
{
	event->AddRef();
	m_callback(event->GetHandle(), m_data);
}

bool EventDispatcher::Handler::operator==(const Handler &other) const
{
	return (m_owner == other.m_owner &&
			m_callback == other.m_callback &&
			m_data == other.m_data);
}

class EventDispatcher::Private
{
public:
	Private();
	void regHandler(void *const onwer,
					const PRL_EVENT_HANDLER_PTR handler,
					void *const data);
	void unregHandler(void *const onwer,
					  const PRL_EVENT_HANDLER_PTR handler,
					  void *const data);
	void unregHandlers(void *const onwer);
	void callHandlers(PrlHandleBase *const event);
private:
	Private(const Private &);
	const Private &operator=(const Private &);
	QMutex m_handlersLock;
	QList<Handler> m_handlers;
};

EventDispatcher::Private::Private():
	m_handlersLock(QMutex::Recursive)
{
}

void EventDispatcher::Private::regHandler(void *const owner,
										  const PRL_EVENT_HANDLER_PTR handler,
										  void *const data)
{
	QMutexLocker handlersLocker(&m_handlersLock);
	m_handlers.append(Handler(owner, handler, data));
}

void EventDispatcher::Private::unregHandler(void *const owner,
											const PRL_EVENT_HANDLER_PTR handler,
											void *const data)
{
	QMutexLocker handlersLocker(&m_handlersLock);
	m_handlers.removeOne(Handler(owner, handler, data));
}

void EventDispatcher::Private::unregHandlers(void *const owner)
{
	QMutexLocker handlersLocker(&m_handlersLock);
	for (int i = m_handlers.size(); 0 < i--;)
	{
		if (m_handlers[i].owner() == owner)
		{
			m_handlers.removeAt(i);
		}
	}
}

void EventDispatcher::Private::callHandlers(PrlHandleBase *const event)
{
	QMutexLocker handlersLocker(&m_handlersLock);
	const QList<Handler> handlers = m_handlers;
	for (int i = 0, c = handlers.size(); c > i; ++i)
	{
		handlers[i].call(event);
	}
}

EventDispatcher::EventDispatcher():
	m_p(new Private)
{
}

EventDispatcher::~EventDispatcher()
{
}

void EventDispatcher::regHandler(void *const owner,
								 const PRL_EVENT_HANDLER_PTR handler,
								 void *const data)
{
	m_p->regHandler(owner, handler, data);
}

void EventDispatcher::unregHandler(void *const owner,
								   const PRL_EVENT_HANDLER_PTR handler,
								   void *const data)
{
	m_p->unregHandler(owner, handler, data);
}

void EventDispatcher::unregHandlers(void *const owner)
{
	m_p->unregHandlers(owner);
}

void EventDispatcher::callHandlers(PrlHandleBase *const event)
{
	m_p->callHandlers(event);
}
