/*
 * IODataBuffer.cpp
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


#include "IODataBuffer.h"

#include <QtGlobal>

#include "Libraries/Logging/Logging.h"

using IOService::IODataBuffer;

/*****************************************************************************/

IODataBuffer::IODataBuffer () :
    m_buffer(0),
    m_buffSize(0),
    m_buffPos(0),
    m_buffCap(0)
{}

IODataBuffer::~IODataBuffer ()
{
    delete [] m_buffer;
}

bool IODataBuffer::allocate ( quint32 size )
{
    if ( m_buffer )
        return false;

    char* newBuff = new(std::nothrow) char [ size ];
    if ( newBuff == 0 ) {
        WRITE_TRACE(DBG_FATAL, "Can't allocate memory for buffer!");
        return false;
    }

    m_buffer = newBuff;
    m_buffSize = 0;
    m_buffPos = 0;
    m_buffCap = size;

    return true;
}

bool IODataBuffer::atEnd () const
{
    return QIODevice::atEnd();
}

bool IODataBuffer::canReadLine () const
{
    Q_ASSERT(0);
    return true;
}

void IODataBuffer::close ()
{
    QIODevice::close();
}

bool IODataBuffer::open ( OpenMode flags )
{
    if ((flags & Append) == Append)
        flags |= WriteOnly;
    setOpenMode(flags);
    return true;
}

qint64 IODataBuffer::pos () const
{
    return QIODevice::pos();
}

bool IODataBuffer::seek ( qint64 pos )
{
    if (pos < 0 || pos >= m_buffSize + 1) {
        LOG_MESSAGE(DBG_FATAL, "Invalid pos: %d", int(pos));
        return false;
    }
    m_buffPos = pos;
    return QIODevice::seek(pos);
}

qint64 IODataBuffer::size () const
{
    return m_buffSize;
}

qint64 IODataBuffer::readData ( char * data, qint64 len )
{
    if ((len = qMin(len, m_buffSize - m_buffPos)) <= 0)
        return qint64(0);
    ::memcpy(data, m_buffer + m_buffPos, len);
    m_buffPos += len;
    return len;
}

qint64 IODataBuffer::writeData ( const char * data, qint64 len )
{
    qint64 extraBytes = m_buffPos + len - m_buffSize;
    if ( extraBytes > 0 ) { // overflow
        qint64 newSize = m_buffSize + extraBytes;
        if ( newSize > m_buffCap ) {
            resizeBuffer(newSize);
            if ( m_buffSize != newSize ) { // could not resize
                WRITE_TRACE(DBG_FATAL, "Can't allocate memory for buffer!");
                return -1;
            }
        }
        else
            m_buffSize = newSize;
    }
    ::memcpy(m_buffer + m_buffPos, (uchar *)data, int(len));
    m_buffPos += int(len);

    return len;
}

char* IODataBuffer::takeBuffer ()
{
    char* buff = m_buffer;
    m_buffer = 0;
    m_buffSize = 0;
    m_buffPos = 0;
    m_buffCap = 0;
    return buff;
}

void IODataBuffer::resizeBuffer ( qint64 newSize )
{
    char* newBuff = new(std::nothrow) char [ newSize ];
    if ( newBuff == 0 )
        return;
    if ( m_buffSize )
        ::memcpy( newBuff, m_buffer, qMin(m_buffSize, newSize) );

    delete [] m_buffer;

    m_buffer = newBuff;
    m_buffSize = newSize;
    m_buffCap = newSize;
}

/*****************************************************************************/
