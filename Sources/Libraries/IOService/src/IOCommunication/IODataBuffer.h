/*
 * IODataBuffer.h
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


#ifndef IODATABUFFER_H
#define IODATABUFFER_H

#include <QIODevice>

namespace IOService {

class IODataBuffer : public QIODevice
{
public:
    IODataBuffer ();
    ~IODataBuffer ();

    bool allocate ( quint32 size );

    virtual bool atEnd () const;
    virtual bool canReadLine () const;
    virtual void close ();
    virtual bool open ( OpenMode flags );
    virtual qint64 pos () const;
    virtual bool seek ( qint64 pos );
    virtual qint64 size () const;

    char* takeBuffer ();

protected:
    virtual qint64 readData ( char * data, qint64 len );
    virtual qint64 writeData ( const char * data, qint64 len );

private:
    void resizeBuffer ( qint64 newSize );

private:
    char* m_buffer;
    qint64 m_buffSize;
    qint64 m_buffPos;
    qint64 m_buffCap;
};

} //namespace IOService

#endif //IODATABUFFER_H
