/*
 * IOProtocol.cpp
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


#include <QBuffer>

#include "IOProtocol.h"
#include "../Build/Current.ver"

#include <time.h>
#include <stdlib.h>

#ifndef _WIN_
  #include <sys/time.h>
#endif

#include "Socket/Socket_p.h"
#include "Libraries/Logging/Logging.h"

#include "Interfaces/ParallelsAlloc.h"
#include "Libraries/Std/AtomicOps.h"

using namespace IOService;

/*****************************************************************************/

/** Define IOPackage static members */
const SmartPtr<IOPackage> IOPackage::Null;

/** Define invalid handle */
const IOSender::Handle IOSender::InvalidHandle;

/** Define IO transport protocol version */
const IOCommunication::ProtocolVersion IOService::IOProtocolVersion =
{
    {'P','R','L','T'}, // Never changes, our 'PRLT' magic string
    6, 8,              // Protocol version: MAJOR, MINOR
    VER_FILEVERSION_STR " (" VER_SPECIAL_BUILD_STR ")" // Build description
};

/** Defined loopback (localhost) addr. */
const char IOService::LoopbackAddr[] = "A40B1FAE-EBF5-4FA7-A748-40388FCFF47D";

/** Defined any (wildcard) addr. */
const char IOService::AnyAddr[] = "";

/*****************************************************************************/

quint64 IOService::msecsFromEpoch ()
{
#ifdef _WIN_
    quint64 msecs = 0;
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);
    msecs |= ft.dwHighDateTime;
    msecs <<= 32;
    msecs |= ft.dwLowDateTime;
    // Convert file time to Unix epoch
    msecs /= 10000; // to msecs
    msecs -= 11644473600000ULL; // diff between 1601 and 1970
    return msecs;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (((quint64)tv.tv_sec * 1000) + ((quint64)tv.tv_usec / 1000));
#endif
}

void IOService::timeMark ( IOService::TimeMark& tm )
{
#ifdef _WIN_
    tm = ::GetTickCount();
#elif defined(_LIN_)
    timespec ts = {0, 0};
    ::clock_gettime(CLOCK_MONOTONIC, &ts);
    tm = (((quint64)ts.tv_sec * 1000) + ((quint64)ts.tv_nsec / 1000000));
#else
#error Unsupported platform
#endif
}

quint32 IOService::msecsDiffTimeMark ( const IOService::TimeMark& tm1,
                                       const IOService::TimeMark& tm2 )
{
#ifdef _WIN_
    // Overflow check, see MSDN of ::GetTickCount for details.
    if ( tm1 > tm2 )
        return (UINT_MAX - (quint32)tm1) + tm2;
    return tm2 - tm1;
#elif defined(_LIN_)
    if ( tm1 > tm2 )
        return 0;
    return tm2 - tm1;
#else
#error Unsupported platform
#endif
}

/*****************************************************************************/

enum {
    ATTACH_BUFFERS_NUMBER = 5
};

SmartPtr<IOPackage> IOCommunication::createAttachClientPackage (
    const IOCommunication::DetachedClient& detachedClient,
    const SmartPtr<IOPackage>& request )
{
    if ( ! detachedClient.isValid() ) {
        return SmartPtr<IOPackage>();
    }

    DetachedClientState* state =
        reinterpret_cast<DetachedClientState*>(detachedClient->m_state);

    SmartPtr<IOPackage> p = IOPackage::createInstance(
                                  IOCommunicationMngPackage::AttachClient,
                                  ATTACH_BUFFERS_NUMBER,
                                  request );
    if ( ! p.isValid() ) {
        return SmartPtr<IOPackage>();
    }

    bool res = false;

    res = p->fillBuffer( 0, IOPackage::RawEncoding,
                         reinterpret_cast<const char*>(&state->header),
                         sizeof(state->header) );
    if ( ! res ) {
        WRITE_TRACE(DBG_FATAL, "Can't fill buffer!");
        return SmartPtr<IOPackage>();
    }

    res = p->setBuffer( 1, IOPackage::RawEncoding,
                        state->data.routingTableBuff,
                        state->header.routingTableBuffSize );
    if ( ! res ) {
        WRITE_TRACE(DBG_FATAL, "Can't set buffer!");
        return SmartPtr<IOPackage>();
    }

    res = p->setBuffer( 2, IOPackage::RawEncoding,
                        state->data.sslSession,
                        state->header.sslSessionSize );
    if ( ! res ) {
        WRITE_TRACE(DBG_FATAL, "Can't set buffer!");
        return SmartPtr<IOPackage>();
    }

    if ( state->data.additionalPkg.isValid() ) {
        quint32 size = 0;
        SmartPtr<char> pkgBuff = state->data.additionalPkg->toBuffer( size );
        if ( ! pkgBuff.isValid() ) {
            WRITE_TRACE(DBG_FATAL, "Can't convert package to buffer! "
                        "Out buffer is invalid!");
            return SmartPtr<IOPackage>();
        }
        res = p->setBuffer( 3, IOPackage::RawEncoding, pkgBuff, size );
        if ( ! res ) {
            WRITE_TRACE(DBG_FATAL, "Can't set buffer!");
            return SmartPtr<IOPackage>();
        }
    }
    if ( state->data.readAhead.isValid() ) {
        p->setBuffer( 4, IOPackage::RawEncoding,
                      state->data.readAhead,
                      state->header.readAheadSize );
    }
    return p;
}

IOCommunication::DetachedClient IOCommunication::parseAttachClientPackage (
    const SmartPtr<IOPackage>& p,
    int unixSocketHandle )
{
    if ( ! p.isValid() ||
         p->header.type != IOCommunicationMngPackage::AttachClient ) {
        LOG_MESSAGE(DBG_FATAL, "Attach client package is invalid: "
                               "wrong type!");
        return IOCommunication::DetachedClient();
    }

    if ( p->header.buffersNumber != ATTACH_BUFFERS_NUMBER ) {
        LOG_MESSAGE(DBG_FATAL, "Attach client package is invalid: "
                               "wrong buffers number!");
        return IOCommunication::DetachedClient();
    }

    DetachedClientState* state = new(std::nothrow) DetachedClientState;
    if ( state == 0 ) {
        WRITE_TRACE(DBG_FATAL, "Can't allocate memory!");
        return IOCommunication::DetachedClient();
    }

    DetachedClientPrivate* detachedClientPri =
        new(std::nothrow) DetachedClientPrivate(state);
    if ( detachedClientPri == 0 ) {
        delete state;
        WRITE_TRACE(DBG_FATAL, "Can't allocate memory!");
        return IOCommunication::DetachedClient();
    }

    DetachedClient detachedClient( detachedClientPri );
    if ( ! detachedClient.isValid() ) {
        WRITE_TRACE(DBG_FATAL, "Can't allocate memory!");
        return IOCommunication::DetachedClient();
    }

    IOPackage::EncodingType type;
    SmartPtr<char> data;
    quint32 size = 0;
    bool res = false;

    // Get header
    {
        res = p->getBuffer( 0, type, data, size );
        if ( ! res ) {
            LOG_MESSAGE(DBG_FATAL, "Attach client package is invalid: "
                        "can't get buffer!");
            return IOCommunication::DetachedClient();
        }

        if ( size != sizeof(state->header) ) {
            LOG_MESSAGE(DBG_FATAL, "Attach client package is invalid: "
                        "wrong header size!");
            return IOCommunication::DetachedClient();
        }
        // Save header
        ::memcpy( &state->header, data.getImpl(), sizeof(state->header) );
    }

    // Get routing table buffer
    {
        res = p->getBuffer( 1, type, data, size );
        if ( ! res ) {
            LOG_MESSAGE(DBG_FATAL, "Attach client package is invalid: "
                        "can't get buffer!");
            return IOCommunication::DetachedClient();
        }

        // Create test rt
        IORoutingTable rt = IORoutingTable::fromBuffer( data, size, res );
        if ( ! res || rt.isNull() ) {
            LOG_MESSAGE(DBG_FATAL, "Attach client package is invalid: "
                        "wrong routing table!");
            return IOCommunication::DetachedClient();
        }
        // Save routing table
        state->data.routingTableBuff = data;
        state->header.routingTableBuffSize = size;
    }

    // Get SSL session
    {
        res = p->getBuffer( 2, type, data, size );
        if ( ! res ) {
            LOG_MESSAGE(DBG_FATAL, "Attach client package is invalid: "
                        "can't get buffer!");
            return IOCommunication::DetachedClient();
        }


        // Create test ssl session
        SSL_SESSION* sslSession = 0;
        const unsigned char* pp =
            reinterpret_cast<const unsigned char*>(data.getImpl());
        sslSession = d2i_SSL_SESSION( &sslSession, &pp, size );
        if ( sslSession == 0 ) {
            LOG_MESSAGE(DBG_FATAL, "Attach client package is invalid: "
                        "SSL session is wrong!");
            return IOCommunication::DetachedClient();
        }
        SSL_SESSION_free(sslSession);
        // Save SSL session
        state->data.sslSession = data;
        state->header.sslSessionSize = size;
    }

    // Get additional package
    {
        res = p->getBuffer( 3, type, data, size );
        if ( ! res ) {
            LOG_MESSAGE(DBG_FATAL, "Attach client package is invalid: "
                        "can't get buffer");
            return IOCommunication::DetachedClient();
        }

        if ( size != 0 ) {
            SmartPtr<IOPackage> additionalPkg;
            try { additionalPkg = IOPackage::createInstance( data, size ); }
            catch ( IOPackage::MalformedPackageException& ) {
                LOG_MESSAGE(DBG_FATAL, "Attach client package is invalid: "
                            "wrong additional packge!");
                return IOCommunication::DetachedClient();
            }
            if ( ! additionalPkg.isValid() ) {
                LOG_MESSAGE(DBG_FATAL, "Attach client package is invalid: "
                            "wrong additional packge!");
                return IOCommunication::DetachedClient();
            }

            // Save additional package
            state->data.additionalPkg = additionalPkg;
        }
    }
    // Get read ahead buffer
    {
         p->getBuffer( 4, type, data, size );
         state->header.readAheadSize = size;
         state->data.readAhead = data;
    }

#ifndef _WIN_
    // Save socket handle for UNIX
    state->header.clientSocket = unixSocketHandle;
#else
    Q_UNUSED(unixSocketHandle);
#endif

    return detachedClient;
}

#ifndef _WIN_
int IOCommunication::getUnixfdFromAttachClientPackage (
    const SmartPtr<IOPackage>& p )
{
    // Unix

    int unixfd = -1;

    if ( ! p.isValid() ||
         p->header.type != IOCommunicationMngPackage::AttachClient ) {
        LOG_MESSAGE(DBG_FATAL, "Attach client package is invalid: "
                               "wrong type!");
        return -1;
    }

    if ( p->header.buffersNumber != ATTACH_BUFFERS_NUMBER ) {
        LOG_MESSAGE(DBG_FATAL, "Attach client package is invalid: "
                               "wrong buffers number!");
        return -1;
    }

    IOPackage::EncodingType type;
    SmartPtr<char> data;
    quint32 size = 0;
    bool res = false;

    // Get header
    {
        res = p->getBuffer( 0, type, data, size );
        if ( ! res ) {
            WRITE_TRACE(DBG_FATAL, "Can't get buffer!");
            return -1;
        }

        const DetachedClientState::PODHeader* header = 0;

        if ( size != sizeof(*header) ) {
            LOG_MESSAGE(DBG_FATAL, "Attach client package is invalid: "
                        "wrong header size!");
            return -1;
        }
        header = reinterpret_cast<const DetachedClientState::PODHeader*>(
                                                             data.getImpl());

        unixfd = header->clientSocket;
    }

    return unixfd;
}
#endif

/*****************************************************************************/

#ifdef _WIN_

// Compilation size check
namespace {
#define COMPILE_TIME_ASSERT(expr) struct x { unsigned int bf : expr; }
typedef IOCommunication::SocketDuplicatedState SockDupSt;
COMPILE_TIME_ASSERT( sizeof(WSAPROTOCOL_INFO) <
                     sizeof(reinterpret_cast<SockDupSt*>(0)->m_protocolInfo) );
}

void* IOCommunication::SocketDuplicatedState::getStateBuffer ( quint32& size )
{
    size = sizeof(WSAPROTOCOL_INFO);
    return &m_protocolInfo;
}

bool IOCommunication::SocketDuplicatedState::toSocket (
    SocketHandle& sock )
{
    WSAPROTOCOL_INFO* wsaInfo =
        reinterpret_cast<WSAPROTOCOL_INFO*>(m_protocolInfo);
    int sockH = ::WSASocket( FROM_PROTOCOL_INFO,
                             FROM_PROTOCOL_INFO,
                             FROM_PROTOCOL_INFO,
                             wsaInfo, 0, WSA_FLAG_OVERLAPPED );
    if ( sockH == INVALID_SOCKET)
        return false;

    SocketHandlePrivate* ptr = new(std::nothrow) SocketHandlePrivate(sockH);
    if ( ptr == 0 ) {
        ::closesocket( sockH );
        return false;
    }
    sock = SmartPtr<SocketHandlePrivate>(ptr);
    return sock.isValid();
}

#endif

/*****************************************************************************/

IOCommunication::SocketHandlePrivate::SocketHandlePrivate (
    int socket ) :
    m_sockHandle(socket)
{}

IOCommunication::SocketHandlePrivate::~SocketHandlePrivate ()
{
    int sockHandle = takeSocketHandle();
    if ( sockHandle != -1 ) {
#ifdef _WIN_
        ::closesocket(sockHandle);
#else
        ::close(sockHandle);
#endif
    }
}

QString IOCommunication::SocketHandlePrivate::socketToString () const
{
    int sockHandle = AtomicRead( &m_sockHandle );
    return QString("%1").arg(sockHandle);
}

#ifdef _WIN_
bool IOCommunication::SocketHandlePrivate::duplicateSocket (
    quint32 pid,
    SocketDuplicatedState& dupState ) const
{
    int sockHandle = AtomicRead( &m_sockHandle );
    if ( sockHandle == -1 )
        return false;

    WSAPROTOCOL_INFO* wsaInfo =
        reinterpret_cast<WSAPROTOCOL_INFO*>(dupState.m_protocolInfo);
    if ( 0 != ::WSADuplicateSocket(sockHandle, pid, wsaInfo) ) {
        return false;
    }
    return true;
}
#endif

int IOCommunication::SocketHandlePrivate::takeSocketHandle ()
{
    return AtomicSwap( &m_sockHandle, -1 );
}

/*****************************************************************************/

IOCommunication::DetachedClientPrivate::DetachedClientPrivate ( void* state, bool server) :
    m_state(state),
    m_attached(false),
    m_server(server)
{
    Q_ASSERT(m_state);
}

IOCommunication::DetachedClientPrivate::~DetachedClientPrivate ()
{
    DetachedClientState* state =
        reinterpret_cast<DetachedClientState*>(m_state);
#ifndef _WIN_
    int sockHandle = takeSocketHandle();
    if ( sockHandle != -1 )
        ::close(sockHandle);
#endif
    delete state;
}

bool IOCommunication::DetachedClientPrivate::attach () const
{
    if ( 0 == AtomicCompareSwap(&m_attached, 0, 1) ) {
        return true;
    }
    return false;
}

#ifndef _WIN_
int IOCommunication::DetachedClientPrivate::takeSocketHandle ()
{
    DetachedClientState* state =
        reinterpret_cast<DetachedClientState*>(m_state);
    return AtomicSwap( &state->header.clientSocket, -1 );
}
#endif

/*****************************************************************************/

IOPackage* IOPackage::allocatePackage ( quint32 buffNum )
{
    return reinterpret_cast<IOPackage*>( ::malloc(IOPACKAGESIZE(buffNum)) );
}

void IOPackage::freePackage ( IOPackage* pkg )
{
    if ( pkg ) {
        ::free( pkg );
    }
}

void IOPackage::destroyPackage ( IOPackage* pkg )
{
    if ( pkg ) {
        pkg->~IOPackage();
        IOPackage::freePackage( pkg );
    }
}

/*****************************************************************************/

SmartPtr<IOPackage> IOPackage::duplicateHeaderInstance (
    const SmartPtr<IOPackage>& pkg,
    quint32 buffNum )
{
    SmartPtr<IOPackage> newPkg =
        IOPackage::createInstance( pkg->header.type, buffNum );

    if ( ! newPkg.isValid() ) {
        return SmartPtr<IOPackage>();
    }

    // Copy main uuids

    ::memcpy( newPkg->header.uuid,
              pkg->header.uuid, sizeof(Uuid_t) );

    ::memcpy( newPkg->header.parentUuid,
              pkg->header.parentUuid, sizeof(Uuid_t) );

    ::memcpy( newPkg->header.senderUuid,
              pkg->header.senderUuid, sizeof(Uuid_t) );

    ::memcpy( newPkg->header.receiverUuid,
              pkg->header.receiverUuid, sizeof(Uuid_t) );

    return newPkg;
}

SmartPtr<IOPackage> IOPackage::duplicateInstance (
    const SmartPtr<IOPackage>& srcPkg,
    bool deepCopy )
{
    SmartPtr<IOPackage> newPkg( new (IOPackage::allocatePackage(srcPkg->header.buffersNumber))
                                    IOPackage(*srcPkg.getImpl()),
                                IOPackage::destroyPackage );
    if ( ! newPkg.isValid() ) {
        WRITE_TRACE(DBG_FATAL, "Can't allocate memory!");
        return SmartPtr<IOPackage>();
    }

    // Deep copy
    if ( deepCopy && newPkg->header.buffersNumber > 0 ) {
        PODData* ioData = IODATAMEMBER(newPkg);

        // Fill buffer
        for ( quint32 i = 0; i < newPkg->header.buffersNumber; ++i ) {
            quint32 buffSize = ioData[i].bufferSize;
            EncodingType enc = ioData[i].bufferEncoding;

            // Fill
            bool res = newPkg->fillBuffer(i, enc, srcPkg->buffers[i].getImpl(),
                                          buffSize);
            if ( ! res ) {
                WRITE_TRACE(DBG_FATAL, "Can't fill buffer!");
                return SmartPtr<IOPackage>();
            }
        }
    }

    return newPkg;
}

SmartPtr<IOPackage> IOPackage::createInstance (
    IOPackage::Type type,
    quint32 buffNum,
    const SmartPtr<IOPackage>& parent,
    bool broadcastResponse )
{
    SmartPtr<IOPackage> newPkg( new (IOPackage::allocatePackage(buffNum))
                                    IOPackage(type, buffNum),
                                IOPackage::destroyPackage );
    if ( ! newPkg.isValid() ) {
        WRITE_TRACE(DBG_FATAL, "Can't allocate memory!");
        return SmartPtr<IOPackage>();
    }

    if ( parent.isValid() ) {
        if ( broadcastResponse )
            newPkg->makeBroadcastResponse( parent );
        else
            newPkg->makeDirectResponse( parent );
    }
    return newPkg;
}

SmartPtr<IOPackage> IOPackage::createInstance (
    IOPackage::Type type,
    IOPackage::EncodingType encType,
    const void* data,
    quint32 size,
    const SmartPtr<IOPackage>& parent,
    bool broadcastResponse )
{
    SmartPtr<IOPackage> package = createInstance( type, !!size, parent,
                                                  broadcastResponse );
    if ( package.isValid() && size ) {
        SmartPtr<char> buffer( new(std::nothrow) char[size], SmartPtrPolicy::ArrayStorage );
        if ( ! buffer.isValid() ) {
            WRITE_TRACE(DBG_FATAL, "Can't allocate memory!");
            return SmartPtr<IOPackage>();
        }
        ::memcpy( buffer.getImpl(), data, size );
        bool res = package->setBuffer( 0, encType, buffer, size );
        if ( ! res ) {
            WRITE_TRACE(DBG_FATAL, "Can't set buffer!");
            return SmartPtr<IOPackage>();
        }
    }
    return package;
}

SmartPtr<IOPackage> IOPackage::createInstance (
    IOPackage::Type type,
    IOPackage::EncodingType encType,
    const SmartPtr<char>& buffer,
    quint32 size,
    const SmartPtr<IOPackage>& parent,
    bool broadcastResponse )
{
    SmartPtr<IOPackage> package = createInstance( type, !!size, parent,
                                                  broadcastResponse );
    if ( package.isValid() && size && buffer.isValid() ) {
        bool res = package->setBuffer( 0, encType, buffer, size );
        if ( ! res ) {
            WRITE_TRACE(DBG_FATAL, "Can't set buffer!");
            return SmartPtr<IOPackage>();
        }
    }
    return package;
}

SmartPtr<IOPackage> IOPackage::createInstance (
    QDataStream& in,
    const SmartPtr<IOPackage>& parent,
    bool broadcastResponse )
{
    Q_ASSERT(in.device());
    qint64 pos = in.device()->pos();

    char tmpPkgHeader[ sizeof(IOPackage::PODHeader) ];

    // Read header
    uint read = in.readRawData( tmpPkgHeader, sizeof(IOPackage::PODHeader) );
    if ( read != sizeof(IOPackage::PODHeader) ) {
        in.device()->seek( pos );
        WRITE_TRACE(DBG_FATAL, "Read from Qt stream failed!");
        return SmartPtr<IOPackage>();
    }

    // Safe cast
    IOPackage* tmpPkg = reinterpret_cast<IOPackage*>(tmpPkgHeader);

    // Number of buffers for correct allocation
    const quint32 buffNum = tmpPkg->header.buffersNumber;

    // Seek to begin
    // NOTE: we can do nothing if seek has been failed!
    in.device()->seek( pos );

    IOPackage* pkgArena = IOPackage::allocatePackage( buffNum );
    if ( pkgArena == 0 ) {
        WRITE_TRACE(DBG_FATAL, "Can't allocate memory!");
        return SmartPtr<IOPackage>();
    }

    SmartPtr<IOPackage> newPkg;
    try {
        newPkg = SmartPtr<IOPackage>( new (pkgArena) IOPackage(in),
                                      IOPackage::destroyPackage );
    }
    catch ( IOPackage::MalformedPackageException& ) {
        IOPackage::freePackage( pkgArena );
        WRITE_TRACE(DBG_FATAL, "Can't create package from Qt stream!");
        return SmartPtr<IOPackage>();
    }

    if ( parent.isValid() ) {
        if ( broadcastResponse )
            newPkg->makeBroadcastResponse( parent );
        else
            newPkg->makeDirectResponse( parent );
    }
    return newPkg;
}

SmartPtr<IOPackage> IOPackage::createInstance (
    const SmartPtr<char>& buff,
    quint32 size,
    const SmartPtr<IOPackage>& parent,
    bool broadcastResponse )
{
    QBuffer buffer;
    QByteArray data = QByteArray::fromRawData( buff.getImpl(), size );
    buffer.setBuffer( &data );
    if ( ! buffer.open(QIODevice::ReadOnly) ) {
        WRITE_TRACE(DBG_FATAL, "Can't open Qt buffer for reading!");
        return SmartPtr<IOPackage>();
    }

    QDataStream in( &buffer );
    in.setVersion(QDataStream::Qt_4_0);

    return IOPackage::createInstance( in, parent, broadcastResponse );
}

quint16 IOPackage::headerChecksumCRC16 ( const IOPackage::PODHeader& header )
{
    return qChecksum( reinterpret_cast<const char*>(&header),
                      sizeof(header) - sizeof(header.crc16) );
}

/*****************************************************************************/

IOPackage::IOPackage ( IOPackage::Type type, quint32 buffNum )
{
    // Zero header
    ::memset( &header, 0, sizeof(header) );

    // Zero callbacks
    ::memset( &callback, 0, sizeof(callback) );

    // Fill some header members
    header.type = type;
    header.buffersNumber = buffNum;

    quint32 initBuffNum = (buffNum > 1 ? buffNum : 1);
    PODData* initData = IODATAMEMBER(this);

    for ( quint32 i = 0; i < initBuffNum; ++i ) {
        // Zero data
        initData[i].bufferEncoding = RawEncoding;
        initData[i].bufferSize = 0;

        // First buffer is already created by compiler
        if ( i >= 1 ) {
            // Place new smart ptr in allocated array
            new( buffers + i ) SmartPtr<char>;
        }
    }
}

IOPackage::IOPackage ( const IOPackage& p )
{
    const quint32 buffNum = p.header.buffersNumber;

    // Copy header
    ::memcpy( &header, &p.header, sizeof(IOPackage::PODHeader) );

    // Zero callback
    ::memset( &callback, 0, sizeof(callback) );

    // Copy data and buffers
    if ( buffNum > 0 ) {
        ::memcpy( IODATAMEMBER(this), IODATAMEMBERCONST(&p), IODATASIZE(&p) );

        for ( quint32 i = 0; i < buffNum; ++i ) {
            // First buffer is already created by compiler
            if ( i >= 1 )
                // Call default constructor and place
                // new smart ptr in allocated array
                new( buffers + i ) SmartPtr<char>;

            // Call assignment operator
            buffers[i] = p.buffers[i];
        }
    }
    else {
        // Zero first data elements
        data[0].bufferEncoding = RawEncoding;
        data[0].bufferSize = 0;
    }
}

IOPackage::IOPackage ( QDataStream& in )
    /*throws (MalformedPackageException)*/
{
    // Get current pos
    Q_ASSERT(in.device());
    qint64 pos = in.device()->pos();

    // Zero callback
    ::memset( &callback, 0, sizeof(callback) );

    // Read header
    uint read = in.readRawData( reinterpret_cast<char*>(&header),
                                sizeof(IOPackage::PODHeader) );
    if ( read != sizeof(IOPackage::PODHeader) ) {
        in.device()->seek( pos );

        throw MalformedPackageException();
    }

    // Read data and buffers
    if ( header.buffersNumber > 0 ) {
        PODData* ioData = IODATAMEMBER(this);

        // Read to data
        read = in.readRawData( reinterpret_cast<char*>(ioData),
                               IODATASIZE(this) );
        if ( read != IODATASIZE(this) ) {
            in.device()->seek( pos );

            throw MalformedPackageException();
        }

        // Create buffers
        for ( quint32 i = 0; i < header.buffersNumber; ++i ) {
            quint32 buffSize = ioData[i].bufferSize;

            // Create and read to buffer
            SmartPtr<char> buff( new char[buffSize],
                                 SmartPtrPolicy::ArrayStorage );
            read = in.readRawData( buff.getImpl(), buffSize );
            if ( read != buffSize ) {
                in.device()->seek( pos );

                throw MalformedPackageException();
            }

            // First buffer is already created by compiler
            if ( i >= 1 )
                // Call default constructor and place
                // new smart ptr in allocated array
                new( buffers + i ) SmartPtr<char>;

            // Create smart ptr with array policy
            buffers[i] = buff;
        }
    }
}

IOPackage::~IOPackage ()
{
    // Destructor callback
    if ( callback.destructorCall )
        callback.destructorCall( callback.destructorContext );

    if ( header.buffersNumber > 1 ) {
        // Reverse order
        for ( quint32 i = header.buffersNumber - 1; i > 0 ; --i ) {
            buffers[i].~SmartPtr<char>();
        }
    }
}

void IOPackage::makeBroadcastResponse ( const SmartPtr<IOPackage>& parent )
{
    if ( ! parent.isValid() )
        return;
    ::memcpy( header.parentUuid, parent->header.uuid, sizeof(Uuid_t) );
}

void IOPackage::makeDirectResponse ( const SmartPtr<IOPackage>& parent )
{
    if ( ! parent.isValid() )
        return;
    ::memcpy( header.parentUuid, parent->header.uuid, sizeof(Uuid_t) );
    ::memcpy( header.receiverUuid, parent->header.senderUuid, sizeof(Uuid_t) );
}

void IOPackage::makeForwardRequest ( const SmartPtr<IOPackage>& request )
{
    if ( ! request.isValid() )
        return;
    ::memcpy( header.senderUuid, request->header.senderUuid, sizeof(Uuid_t) );
}

bool IOPackage::isResponsePackage () const
{
    return ! Uuid::toUuid(header.parentUuid).isNull();
}

bool IOPackage::getBuffer ( quint32 index, EncodingType& enc,
                            SmartPtr<char>& buff, quint32& size ) const
{
    if ( index >= header.buffersNumber ) {
        return false;
    }

    const PODData* ioData = IODATAMEMBERCONST(this);

    buff = buffers[index];
    enc = ioData[index].bufferEncoding;
    size = ioData[index].bufferSize;

    return true;
}

bool IOPackage::setBuffer ( quint32 index, EncodingType enc,
                            IODataBuffer& buffer )
{
    qint32 size = static_cast<qint32>( buffer.size() );
    char* buff = buffer.takeBuffer();
    return setBuffer( index, enc,
                      SmartPtr<char>(buff, SmartPtrPolicy::ArrayStorage),
                      size );
}

bool IOPackage::setBuffer ( quint32 index, EncodingType enc,
                            const SmartPtr<char>& buff, quint32 size )
{
    if ( index >= header.buffersNumber ) {
        return false;
    }

    PODData* ioData = IODATAMEMBER(this);

    buffers[index] = buff;
    ioData[index].bufferEncoding = enc;
    ioData[index].bufferSize = size;

    return true;
}

bool IOPackage::fillBuffer ( quint32 index, EncodingType enc,
                             const void* buffSrc, quint32 size )
{
    if ( index >= header.buffersNumber ) {
        return false;
    }

    SmartPtr<char> smartBuff;

    if ( size != 0 ) {
        smartBuff = SmartPtr<char>(new(std::nothrow) char[size],
                                   SmartPtrPolicy::ArrayStorage);
        if ( ! smartBuff.isValid() ) {
            WRITE_TRACE(DBG_FATAL, "Can't allocate memory!");
            return false;
        }

        ::memcpy( smartBuff.getImpl(), buffSrc, size );
    }

    // Delegate to set variant
    return setBuffer( index, enc, smartBuff, size );
}

quint32 IOPackage::buffersSize () const
{
    if ( header.buffersNumber == 0 )
        return 0;

    quint32 size = 0;
    const PODData* ioData = IODATAMEMBERCONST(this);
    for ( quint32 i = 0; i < header.buffersNumber; ++i ) {
        size += ioData[i].bufferSize;

    }

    return size;
}

quint32 IOPackage::dataSize () const
{
    return IODATASIZE(this);
}

quint32 IOPackage::fullPackageSize () const
{
    return (header.buffersNumber == 0 ? sizeof(IOPackage::PODHeader) :
            sizeof(IOPackage::PODHeader) + IODATASIZE(this) + buffersSize());
}

quint16 IOPackage::headerChecksumCRC16 () const
{
    return IOPackage::headerChecksumCRC16( header );
}

SmartPtr<char> IOPackage::toBuffer ( quint32& size ) const
{
    size = fullPackageSize();

    bool res = false;
    IODataBuffer buffer;
    res = buffer.allocate( size );
    if ( ! res ) {
        return SmartPtr<char>();
    }

    // Always returns true
    res = buffer.open( QIODevice::ReadWrite );
    Q_ASSERT(res);

    QDataStream out( &buffer );
    out.setVersion(QDataStream::Qt_4_0);

    writeToStream( out );
    if ( size != buffer.size() ) {
        WRITE_TRACE(DBG_FATAL, "Buffer size is wrong!");
        return SmartPtr<char>();
    }
    return SmartPtr<char>( buffer.takeBuffer(), SmartPtrPolicy::ArrayStorage );
}

void IOPackage::writeToStream ( QDataStream& out ) const
{
    // Write header
    out.writeRawData( reinterpret_cast<const char*>(&header),
                      sizeof(IOPackage::PODHeader) );

    // Write data and buffers
    if ( header.buffersNumber > 0 ) {
        const PODData* ioData = IODATAMEMBERCONST(this);

        // Write data
        out.writeRawData( reinterpret_cast<const char*>(ioData),
                          IODATASIZE(this) );

        // Write buffers
        for ( quint32 i = 0; i < header.buffersNumber; ++i ) {
            quint32 buffSize = ioData[i].bufferSize;
            out.writeRawData( buffers[i].getImpl(), buffSize );
        }
    }
}

/** Event numeric identifier - must be global counter for all events */
static unsigned long long g_eventId = 1;

/** Generate numeric identifier for a package by incrementing the global counter */
void IOPackage::generateNumericId ()
{
	if ( header.numericId == 0 )
		header.numericId = AtomicInc64U(&g_eventId);
}

/*
* Here is custom free routine POD data allocated through vallocPOD routine
*/
static void vfreePOD(char* pointee)
{
	if (pointee != NULL)
		prl_vfree(pointee);
}

static SmartPtr<char> vallocPOD(quint32 buffSize)
{
	return SmartPtr<char> ( static_cast<char*>( prl_valloc(buffSize) ), vfreePOD );
}

// type Package header type
SmartPtr<char> IOPackage::allocPODBuffer ( const PODData& desc )
{
    if ( desc.bufferEncoding == RawEncodingAlligned )
        return vallocPOD(desc.bufferSize);

    return SmartPtr<char>( new(std::nothrow) char[ desc.bufferSize ],
                           SmartPtrPolicy::ArrayStorage );
}

/*****************************************************************************/
