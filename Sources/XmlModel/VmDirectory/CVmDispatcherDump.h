/*
 * CVmDispatcherDump.h
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


#ifndef CVMDISPATCHERDUMP_H
#define CVMDISPATCHERDUMP_H

#include <QHash>
#include <QPair>
#include <QFile>
#include <QProcess>
#include <QDomDocument>
#include <QMutex>

class CVmDispatcherDump
{
public:
    /** Object uuid, VM pid */
    struct DumpSet
    {
        DumpSet () :
            vmPid(0)
        {}

        DumpSet ( const QString& objUuid_, qint64 vmPid_ ) :
            objUuid(objUuid_), vmPid(vmPid_)
        {}

        DumpSet ( const DumpSet& ds ) :
            objUuid(ds.objUuid), vmPid(ds.vmPid)
        {}

        DumpSet& operator= ( const DumpSet& ds )
        {
            objUuid = ds.objUuid;
            vmPid = ds.vmPid;
            return *this;
        }

        QString objUuid;
        qint64 vmPid;
    };

    /** Hash of VM uuids as keys and dump sets as values */
    typedef QHash<QString, DumpSet> DumpHash;

    /** Default constructor  */
    CVmDispatcherDump ();
    /** Reads and associates inner structure with file  */
    CVmDispatcherDump ( const QString& fileName );
    ~CVmDispatcherDump ();

    /** Reads and associates inner structure with file  */
    bool setDumpFile ( const QString& fileName );

    /** Returns true if dump file was successfully open and parsed */
    bool isOpen () const;

    /** Returns hash of dump sets */
    DumpHash getDump () const;

    /**
     * Checks if dump contains dump set.
     * Also compares pid of this process and pid of process,
     * who saved this set. Pids should be different, because
     * we should get dump sets obly from previous save.
     */
    bool contains ( const QString& ) const;

    /** Returns dump file as XML string */
    QString toString () const;

    /** Adds new set to hash, if key doesn't exist, or makes changes */
    void dumpSet ( const QString&, const DumpSet& );
    /** Tries to remove set by VM uuid, returns true if succeed */
    bool removeDumpSet ( const QString& vmUuid );

    /** Cleans dump sets, which dispatcher pid != current pid */
    void cleanOldDumpSets ();

private:
    /** Parses dump file. Often called from constructor */
    void parseDumpFile ();

    /** Flushes last changes to associated file */
    void flush ();

    CVmDispatcherDump ( const CVmDispatcherDump& );
    CVmDispatcherDump& operator= ( const CVmDispatcherDump& );


private:
    typedef QHash<QString, QDomElement> DumpElements;
    DumpHash dumpHash;
    DumpElements dumpElements;
    QFile dumpFile;
    QDomDocument dumpDoc;
    mutable QMutex mutex;
};

#endif //CVMDISPATCHERDUMP_H
