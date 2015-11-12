/*
 * IOProtocol.h
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


#ifndef IOPROTOCOL_H
#define IOPROTOCOL_H

#include <QtGlobal>
#include <QDataStream>
#include <QPair>
#include <QHash>
#include <QString>

#include "IODataBuffer.h"

#include "Libraries/Std/SmartPtr.h"
#include "Libraries/PrlUuid/Uuid.h"
#include "Interfaces/ParallelsTypes.h"
#include "SDK/Include/PrlIOStructs.h"
#include "IOSenderHandle.h"
#include "IOProtocolCommon.h"

namespace IOService {

    /**
     * Use this type for timeout estimation
     * @see #msecsDiffTimeMark
     * @see #timeMark
     */
    typedef quint64 TimeMark;

    /**
     * Returns msecs since '1970-01-01T00:00:00'
     *  NOTE: do not use this function for timeout estimation
     *        use #timeMark and #msecsDiffTimeMark
     * @see #msecsDiffTimeMark
     * @see #timeMark
     */
    quint64 msecsFromEpoch ();

    /**
     * Returns platform dependent time mark.
     * Use this function for timeout estimation.
     * NOTE: Never tries just to subtract start
     *       from end mark. Use #msecsDiffTimeMark
     *       instead.
     * @see #msecsDiffTimeMark
     */
    void timeMark ( TimeMark& );

    /**
     * Returns msecs difference betweeen time marks arguments.
     * Use this function for timeout estimation.
     * @see #timeMark
     */
    quint32 msecsDiffTimeMark ( const TimeMark&, const TimeMark& );

    // Binding information by IP version
    typedef QHash<IPVersion, QPair<QString, quint16> > IPvBindingInfo;

    /** Common sender namespace */
    namespace IOSender
    {
        // IO client and IO server states
        enum State {
            // Common states for IO client and IO server

            Disconnected = 0, /**< client/server: disconnected from peer */
            Connected,        /**< client/server: connected to peer */
        };

        // IO client and IO server states
        enum Error {
            //
            // IO common errors
            //

            UnknownError = 0,        /**< Nothing has happened,
                                          or really unknown error */

            ConnectionTimeout,       /**< This happens when specified for connect
                                          operation timeout is exhausted */

            HeartBeatTimeoutError,   /**< Peer has not been sent heart beat
                                          #IOHeartBeatReceiveTimeout msecs. */

            //
            // IO client handshake errors
            //

            HandshakeError,          /**< Common error. This happens,
                                          when other side is not a
                                          parallels product,
                                          or is a absolutely incompatible
                                          version. */

            ProtocolVersionError,    /**< This happens, when other side
                                          is a parallels product, but
                                          protocol version differs. */

            SSLHandshakeError,       /**< SSL handshake failed. */

            RoutingTableAcceptError, /**< Routing table has not been
                                          accepted by server, client was
                                          disconnected. */

            HostnameResolveError,    /**< hostname or servname not provided,
                                          or not known. */
        };


        // Sender type
        enum Type {
            UnknownType          = 0,
            Vm                   = 1,
            Dispatcher           = 2,
            Client               = 3,
            IOClient             = 4,
            FileTransfer         = 5,
            PTAgent              = 6,
            PTClient             = 7,
            ProxyManager         = 8,
            IOCtClient           = 9,

            // Type bound
            EndTypeBound
        };

        // Sender connection mode
        enum ConnectionMode {
            DirectConnectionMode = 0, /**< Generic direct connection */
        };
		// Sender connection security mode
        enum SecurityMode {
			UnknownMode = 0,
            UntrustedConnection	= 1,     /**< Connection without TSL options */
            SelfSignedConnection  = 2,   /**< Connection using self generated keys */
			TrustedChannelConnection = 3 /**< Connection using trusted channels */
        };

        struct Statistics
        {
            qint64 sentPackages;      /**< Number of sent packages */
            qint64 receivedPackages;  /**< Number of received packages */
            qint64 sentBytes;         /**< Number of written to network,
                                           (i.e. encrypted) bytes */
            qint64 receivedBytes;     /**< Number of read from network,
                                           (i.e. encrypted) bytes */
            Statistics () :
                sentPackages(0),
                receivedPackages(0),
                sentBytes(0),
                receivedBytes(0)
            {}
        }
#if defined(_ARM_)
		__attribute__((__aligned__(8)))
#endif //__ARM__
		;

    } //namespace IOSender

    /**
     * Class describes IO package.
     */
    class IOPackage {
    public:

        /** Common package type */
        typedef quint32 Type;

        /**
         * Package callback which is called before or after
         * this package has been sent
         */
        typedef void (*SendCallback) (
                                   bool afterSend, /**< true if after send,
                                                        false if before */
                                   void* context,
                                   const IOSender::Handle&, /**< sender */
                                   const IOSender::Handle&, /**< receiver */
                                   quint32 sendJobResult,
                                   const SmartPtr<IOPackage>& );

        /** Package callback which is called just before package destruction */
        typedef void (*DestructorCallback) ( void* context );


        enum EncodingType {
            // Unchangeable encodings
            RawEncoding     = 0,
            // Raw encoding with page alligned buffers
            RawEncodingAlligned = 8,
            // Final bound
            EncodingFinalBound
        };

        // Package exceptions

        /** Occurs when package is not valid */
        class MalformedPackageException {};

    private:
        // All constructors are private to oblige user to use
        // static factory methods #createInstance, which return
        // wrapped object to prevent careless management.

        /** Package constructors */
        IOPackage ( IOPackage::Type, quint32 buffNum );

        /**
         * Copy constructor.
         * @note Buffers are not deeply copied!
         */
        IOPackage ( const IOPackage& );

        /**
         * Package constructor: reads data from stream
         * @throw MalformedPackageException occurs when we can't create package
         *        from stream
         */
        IOPackage ( QDataStream& );
            /*throw (MalformedPackageException)*/;

    public:
        /** Defines null IO package for easy declarations */
        static const SmartPtr<IOPackage> Null;

        /**
         * Create a copy of header.
         * @param pkg package to duplicate
         * @param buffNum number of buffers in new package
         */
        static SmartPtr<IOPackage> duplicateHeaderInstance (
                                               const SmartPtr<IOPackage>& pkg,
                                               quint32 buffNum );

        /**
         * Create a copy of package instance.
         * @param pkg package to duplicate
         * @param deepCopy if true, all buffers are deeply copied to new pkg
         */
        static SmartPtr<IOPackage> duplicateInstance (
                                               const SmartPtr<IOPackage>& pkg,
                                               bool deepCopy = false );

        /**
         * Create package instance.
         * @param type of new package
         * @param buffNum number of bufers of new package
         * @param parent is an optional parameter to make a response
         *        on this pkg.
         * @param broadcastResponse is an optional param to make
         *        broadcast response or direct response.
         *        Direct response is a copy of 'parent.header.uuid' to
         *        'newPkg.header.parentUuid'.
         *        Broadcast response is copy of 'parent.header.uuid' to
         *        'newPkg.header.parentUuid' and 'parent.header.senderUuid' to
         *        'newPkg.header.receiverUuid'.
         *
         * @see #makeDirectResponse
         * @see #makeBroadcastResponse
         */
        static SmartPtr<IOPackage> createInstance (
                   IOPackage::Type type,
                   quint32 buffNum,
                   const SmartPtr<IOPackage>& parent = IOPackage::Null,
                   bool broadcastResponse = true );

        /**
         * Create package instance with a single buffer.
         * @param type of new package
         * @param encType type of encoding of buffer
         * @param data is a data buffer which will be copied
         * @param parent is an optional parameter to make a response
         *        on this pkg.
         * @param broadcastResponse is an optional param to make
         *        broadcast response or direct response.
         *        Direct response is a copy of 'parent.header.uuid' to
         *        'newPkg.header.parentUuid'.
         *        Broadcast response is copy of 'parent.header.uuid' to
         *        'newPkg.header.parentUuid' and 'parent.header.senderUuid' to
         *        'newPkg.header.receiverUuid'.
         *
         * @see #makeDirectResponse
         * @see #makeBroadcastResponse
         */
        static SmartPtr<IOPackage> createInstance (
                   IOPackage::Type type,
                   IOPackage::EncodingType encType,
                   const void* data,
                   quint32 size,
                   const SmartPtr<IOPackage>& parent = IOPackage::Null,
                   bool broadcastResponse = true );

        /**
         * Create package instance with a single buffer.
         * @param type of new package
         * @param encType type of encoding of buffer
         * @param data is a data buffer which will set with call #setBuffer
         * @param parent is an optional parameter to make a response
         *        on this pkg.
         * @param broadcastResponse is an optional param to make
         *        broadcast response or direct response.
         *        Direct response is a copy of 'parent.header.uuid' to
         *        'newPkg.header.parentUuid'.
         *        Broadcast response is copy of 'parent.header.uuid' to
         *        'newPkg.header.parentUuid' and 'parent.header.senderUuid' to
         *        'newPkg.header.receiverUuid'.
         *
         * @see #makeDirectResponse
         * @see #makeBroadcastResponse
         * @see #setBuffer
         */
        static SmartPtr<IOPackage> createInstance (
                   IOPackage::Type type,
                   IOPackage::EncodingType encType,
                   const SmartPtr<char>& data,
                   quint32 size,
                   const SmartPtr<IOPackage>& parent = IOPackage::Null,
                   bool broadcastResponse = true );

        /**
         * Create package instance.
         * @param stream from which data should be read
         * @param parent is an optional parameter to make a response
         *        on this pkg.
         * @param broadcastResponse is an optional param to make
         *        broadcast response or direct response.
         *        Direct response is a copy of 'parent.header.uuid' to
         *        'newPkg.header.parentUuid'.
         *        Broadcast response is copy of 'parent.header.uuid' to
         *        'newPkg.header.parentUuid' and 'parent.header.senderUuid' to
         *        'newPkg.header.receiverUuid'.
         *
         * @see #makeDirectResponse
         * @see #makeBroadcastResponse
         */
        static SmartPtr<IOPackage> createInstance (
                   QDataStream& stream,
                   const SmartPtr<IOPackage>& parent = IOPackage::Null,
                   bool broadcastResponse = true );

        /**
         * Create package instance.
         * @param size of the input buffer
         * @param buffer from which data should be read
         * @param parent is an optional parameter to make a response
         *        on this pkg.
         * @param broadcastResponse is an optional param to make
         *        broadcast response or direct response.
         *        Direct response is a copy of 'parent.header.uuid' to
         *        'newPkg.header.parentUuid'.
         *        Broadcast response is copy of 'parent.header.uuid' to
         *        'newPkg.header.parentUuid' and 'parent.header.senderUuid' to
         *        'newPkg.header.receiverUuid'.
         *
         * @see #makeDirectResponse
         * @see #makeBroadcastResponse
         * @see #toBuffer
         */
        static SmartPtr<IOPackage> createInstance (
                   const SmartPtr<char>& buffer,
                   quint32 size,
                   const SmartPtr<IOPackage>& parent = IOPackage::Null,
                   bool broadcastResponse = true );

        /** Package destructor */
        ~IOPackage ();

        /**
         * Makes broadcast response on param package.
         * Copies 'parent.header.uuid' to 'this.header.parentUuid',
         * nothing is told about receiver, so package will be sent
         * to all possible receivers on server side.
         */
        void makeBroadcastResponse ( const SmartPtr<IOPackage>& parent );

        /**
         * Makes direct response on param package.
         * Copies 'parent.header.uuid' to 'this.header.parentUuid',
         * and 'parent.header.senderUuid' to 'this.header.receiverUuid',
         * so server side will try to find receiver by 'receiverUuid'
         * and send package to this receiver directly.
         */
        void makeDirectResponse ( const SmartPtr<IOPackage>& parent );

        /**
         * Makes request on param package.
         * Copies 'request.header.senderUuid' to 'this.header.senderUuid',
         * so new package is being forwarded with new sender.
         */
        void makeForwardRequest ( const SmartPtr<IOPackage>& request );

        /**
         * Returns true if this package created as response,
         * i.e. its parent uuid is valid.
         *
         * @see #createResponseInstance
         */
        bool isResponsePackage () const;

        /**
         * Returns encoding type, inner buffer and its size by index as params.
         * If index is out of bounds false will be returned, true otherwise.
         */
        bool getBuffer ( quint32 index, EncodingType&,
                         SmartPtr<char>&, quint32& size ) const;

        /**
         * Replace inner data buffer with argument IO data buffer.
         * Inner char buffer will be taken from argument and wrapped with
         * smart ptr, which will have #SmartPtrPolicy::ArrayStorage policy,
         * i.e. buffer will be freed like 'delete [] buff'.
         * Return false if index is out of bounds.
         */
        bool setBuffer ( quint32 index, EncodingType, IODataBuffer& );

        /**
         * Replace inner data buffer with argument smart ptr buffer.
         * Same as above, except that you can create your own smart ptr
         * with any storage policy you like, e.g.:
         *    if you want IOPackage not to free wrapped buffer, you can create
         *    smart ptr with #SmartPtrPolicy::DoNotReleasePointee policy
         *    and then manage this buffer independently.
         * Returns false when index is out of bounds.
         */
        bool setBuffer ( quint32 index, EncodingType,
                         const SmartPtr<char>&, quint32 size );

        /**
         * Create inner smart ptr buffer with #SmartPtrPolicy::ArrayStorage
         * policy and copy argument buffer to it.
         * Returns false when index is out of bounds.
         */
        bool fillBuffer ( quint32 index, EncodingType, const void*,
                          quint32 size );

        /** Returns size of all buffers */
        quint32 buffersSize () const;

        /**
         * Returns size of data: data size * header.buffersNumber
         */
        quint32 dataSize () const;

        /**
         * Returns full package size: header size + dataSize() + buffersSize()
         */
        quint32 fullPackageSize () const;

        /**
         * Returns msecs elapsed from package creation time stamp:
         * @return result = now - (package_timestamp + client_time_sync).
         *         if result is 0, package time stamp is not valid.
         */
        quint32 msecsElapsed ( qint32 clientTimeSync = 0 ) const;

        /**
         * Returns CRC16 number of header.
         * (except 'crc16' member, of course)
         */
        quint16 headerChecksumCRC16 () const;

        /**
         * Sets current time stamp in msecs format.
         * @see #msecsFromEpoch
         */
        void setCurrentCreationTimeStamp ();

        /**
         * Converts this package to plain data buffer.
         * @see #IOPackage::createInstance
         */
        SmartPtr<char> toBuffer ( quint32& size ) const;

        /** Writes this package to stream */
        void writeToStream ( QDataStream& ) const;

        /** Generates numeric ID and saves it in packet header */
        void generateNumericId ();
    public:
        // Package members

#include <Interfaces/packed.h>
        // POD header
        struct PODHeader {
            Uuid_t         uuid;          /**< Package uuid */
            Uuid_t         parentUuid;    /**< Parent package uuid */
            Uuid_t         senderUuid;    /**< Sender uuid */
            Uuid_t         receiverUuid;  /**< Receiver uuid */
            Type           type;          /**< Package header type */
            quint64        numericId;     /**< Numeric identifier
                                               (used for events ordering ). */
            quint32        buffersNumber; /**< Number of allocated buffers */
            quint16        crc16;         /**< CRC16 header checksum.
                                              (CRC16 member must be the last)*/
        } PACKED header;
#include <Interfaces/unpacked.h>

        // POD callback
        struct PODCallback {
            SendCallback beforeSendCall;  /** Called before native send */
            SendCallback afterSendCall;   /** Called after native send */
            void* sendContext;            /** Send context pointer */
            DestructorCallback destructorCall; /** Called before package
                                                   destruction */
            void* destructorContext;           /** Destructor context */

        } callback;

        //
        // NOTE! dynamic buffers and data arrays must be at the end
        //

        // Use #getBuffer, #setBuffer or #fillBuffer instead of direct access
        // to array data and buffer members.

        SmartPtr<char> buffers[1];        /**< Package buffers */

#include <Interfaces/packed.h>
        struct PODData {
            EncodingType   bufferEncoding; /**< Package buffer encodings */
            quint32        bufferSize;     /**< Package buffer sizes */
        } PACKED data[1];
#include <Interfaces/unpacked.h>

    public:
        /**
         * Static variant of #IOPackage::headerChecksumCRC16
         * @see #IOPackage::headerChecksumCRC16
         */
        static quint16 headerChecksumCRC16 ( const IOPackage::PODHeader& );

        static SmartPtr<char> allocPODBuffer(const IOPackage::PODData& desc);

    private: // Special memory alloc/dealloc functions

        // Only allocates memory
        static IOPackage* allocatePackage ( quint32 buffsNumber );
        // Only frees memory
        static void freePackage ( IOPackage* );
        // Frees With destructor call
        static void destroyPackage ( IOPackage* );

    private: // Hidden not implemented methods

        /** Assignment operator can't be implemented because of flat buffers */
        IOPackage& operator= ( const IOPackage& );

    };

    /** IO communication manager namespace */
    namespace IOCommunicationMngPackage
    {
        // IO communication manager package types
        enum Type {
            IOCommunicationMngTypeBoundBegin = 1,

            // Alias macros for looooong word
            // 'IOCommunicationMngTypeBoundBegin'
#define B IOCommunicationMngTypeBoundBegin

            // Traffic report types
            StartTrafficReport  = B+2, /**< Request to start traffic report */
            StopTrafficReport   = B+3, /**< Request to stop traffic report */
            TrafficReport       = B+4, /**< Traffic report */
            TimeSync            = B+5, /**< Time sync package (for remote) */

            // Inner type
            AttachClient         = B+6, /**< Package to attach client */
            DetachClientRequest  = B+7, /**< Package to client to sync
                                             detaching on server */
            DetachClientResponse = B+8, /**< Package to server to sync
                                             detaching */
            HeartBeat            = B+9, /**< Package to keep connection alive */
            DetachBothSidesRequest = B+10, /**< Package to client to detach both
                                             sides of the connection */

            // Useless for now
#undef B
            IOCommunicationMngTypeBoundEnd
        };
    } //namespace IOCommunicationMngPackage

    /** Common IO communication namespace */
    namespace IOCommunication {

        /**
         * Common consts
         */
        enum Consts {
            IOGracefulShutdownTimeout = 5000, // msecs
            IOHeartBeatSendTimeout    = 10000, // msecs
            IOHeartBeatReceiveTimeout = IOHeartBeatSendTimeout * 3, // msecs
            IOCallbackPossibleTime    = 10000, // msecs
        };

        /** First declaration */
        typedef SmartPtr<struct SocketHandlePrivate> SocketHandle;
        typedef SmartPtr<struct DetachedClientPrivate> DetachedClient;

#ifdef _WIN_
        /**
         * Wrapper to contain WSAPROTOCOL_INFO structure
         */
        struct SocketDuplicatedState
        {
            /** Returns pointer on inner buffer and it's size */
            void* getStateBuffer ( quint32& size );

            /** Converts to socket handle from duplicated state */
            bool toSocket ( SocketHandle& );

            char m_protocolInfo[1024];
        };
#endif

        /**
         * Socket handle.
         * Must be used only as #SocketHandle
         */
        struct SocketHandlePrivate
        {
            int m_sockHandle;

            /** Constructor with socket */
            SocketHandlePrivate ( int socket );

            /** Destructor */
            ~SocketHandlePrivate ();

            /** Converts socket handle to string */
            QString socketToString () const;

#ifdef _WIN_
            /** Duplicates socket */
            bool duplicateSocket ( quint32 pid, SocketDuplicatedState& ) const;
#endif

            /**
             * Atomically takes socket handle and replaces
             * old value with invalid socket handle (-1).
             */
            int takeSocketHandle ();
        };

       /**
         * Detached client private structure
         * Must be used only as #DetachedClient
         */
        struct DetachedClientPrivate
        {
            void* m_state;
            mutable qint32 m_attached;
            bool m_server;

            /** Constructor */
            DetachedClientPrivate ( void* , bool server = true);
            /** Destructor */
            ~DetachedClientPrivate ();

            /**
             * Atomically mark this client as attached.
             * Return true if success, false otherwise.
             */
            bool attach () const;

#ifndef _WIN_
            /**
             * Atomically takes socket handle from state and replaces
             * old value with invalid socket handle (-1).
             */
            int takeSocketHandle ();
#endif
        };



#include <Interfaces/packed.h>
        /** Handshake header struct, which describes other side. */
        struct HandshakeHeader
        {
            IOSender::Type   senderType;
            Uuid_t           connectionUuid;

        } PACKED;
#include <Interfaces/unpacked.h>


        /**
         * Create package from detached client with type
         * #IOCommunicationMngPackage::AttachClient.
         * Specify request package to make response package on it.
         */
        SmartPtr<IOPackage> createAttachClientPackage (
                                      const IOCommunication::DetachedClient&,
                                      const SmartPtr<IOPackage>& request =
                                          IOPackage::Null );

        /**
         * Create detached client from package.
         * If can't, invalid #DetachedClient will be returned.
         * (see #isValid)
         */
        IOCommunication::DetachedClient parseAttachClientPackage (
                                      const SmartPtr<IOPackage>&,
                                      int unixSocketHandle );

#ifndef _WIN_
        /**
         * On UNIX file descriptor will be returned from
         * #IOCommunicationMngPackage::AttachClient package.
         * If package is not an #AttachClient, -1 will be returned.
         */
        int getUnixfdFromAttachClientPackage ( const SmartPtr<IOPackage>& );
#endif

    } //namespace IOCommunication

    /** Declare IO transport protocol version */
    extern const IOCommunication::ProtocolVersion IOProtocolVersion;

    /**
     * Default loopback (localhost) addr for client and server.
     * (actually, just an string with a single \s character.)
     */
    extern const char LoopbackAddr[];

    /**
     * Any (wildcard) addr for server.
     * (actually, just an empty string.)
     */
    extern const char AnyAddr[];

/**
 * IO protocol features table:
 *
 *   MAJOR | MINOR | FEATURE
 *   ------|-------|-----------------------
 *     <6  |  XXX  | old IO protocol
 *   ------|-------|-----------------------
 *     6   |   0   | released protocol
 *   ------|-------|-----------------------
 *     6   |   1   | heart beat support
 *   ------|-------|-----------------------
 *     6   |   2   | binary response
 *   ------|-------|-----------------------
 *     6   |   3   | runtime IO encodings
 *   ------|-------|-----------------------
 *     6   |   4   | attach to VM flags
 *   ------|-------|-----------------------
 *     6   |   5   | KeyEvent replaced with KeyEventEx
 *   ------|-------|-----------------------
 *     6   |   6   | Remote audio support
 *   ------|-------|-----------------------
 *     6   |   7   | New extended display encodings (YV support)
 *   ------|-------|-----------------------
 *     6   |   8   | Event identifier in PODHeader
 *   ------|-------|-----------------------
 */

/**
 * Return integer representation of IO protocol two numbers
 */
#define IOPROTOCOL_VERSION(major, minor) \
    ( ((major) & 0xffff) << 16 | ((minor) & 0xffff) )

/**
 * Returns true if protocol version supports heart beat.
 */
#define IOPROTOCOL_HEART_BEAT_SUPPORT(ver) \
    ( (ver).majorNumber > 6 || ((ver).majorNumber == 6 && (ver).minorNumber >= 1) )

/**
 * Returns true if protocol version supports binary response.
 */
#define IOPROTOCOL_BINARY_RESPONSE_SUPPORT(ver) \
    ( (ver).majorNumber > 6 || ((ver).majorNumber == 6 && (ver).minorNumber >= 2) )

/**
 * Returns true if protocol version supports extended display encodings.
 */
#define IOPROTOCOL_EXT_IO_DISPLAY_ENCODINGS_SUPPORT(ver) \
    ( (ver).majorNumber > 6 || ((ver).majorNumber == 6 && (ver).minorNumber >= 3) )

/**
 * Returns true if protocol version supports KeyEventEx.
 */
#define IOPROTOCOL_KEY_EVENT_EX_SUPPORT(ver) \
    ( (ver).majorNumber > 6 || ((ver).majorNumber == 6 && (ver).minorNumber >= 5) )

/**
 * Returns true if protocol version supports extended audio encodings.
 */
#define IOPROTOCOL_IO_AUDIO_ENCODINGS_SUPPORT(ver) \
    ( (ver).majorNumber > 6 || ((ver).majorNumber == 6 && (ver).minorNumber >= 6) )

/**
 * Returns true if protocol version supports new extended display encodings.
 */
#define IOPROTOCOL_NEW_EXT_IO_DISPLAY_ENCODINGS_SUPPORT(ver) \
    ( (ver).majorNumber > 6 || ((ver).majorNumber == 6 && (ver).minorNumber >= 7) )

/**
 * Returns true if protocol version supports numeric identifiers for events.
 */
#define IOPROTOCOL_EVENT_NUMERIC_ID_SUPPORT(ver) \
	( (ver).majorNumber > 6 || ((ver).majorNumber == 6 && (ver).minorNumber >= 8) )

/**
 * IO protocol internal macroses
 */

/**
 * Offsetof helper macros,
 * (we use 1 (not NULL) because gcc does not want to access NULL object)
 */
#define IOOFFSETOF(pkg, member) \
        ( ((size_t)&((pkg*)(1))->member) - 1 )

/**
 * Macros for calculating IO package size by buffers number.
 * @note Used only for memory allocation.
 */
#define IOPACKAGESIZE(buffNum) \
  ( buffNum > 1 ? \
    IOOFFSETOF(IOService::IOPackage, buffers) + \
      (sizeof(IOService::IOPackage) - \
       IOOFFSETOF(IOService::IOPackage, buffers)) * buffNum \
    : \
    sizeof(IOService::IOPackage) )

/**
 * Count data size according to buffers size
 * @note Used only for memory allocation.
 */
#define IODATASIZE(pkg) \
  (  (pkg)->header.buffersNumber > 1 ? \
     sizeof(IOService::IOPackage::PODData) * (pkg)->header.buffersNumber : \
     sizeof(IOService::IOPackage::PODData) )

/**
 * Macros for correct data addressing.
 */
#define IODATAMEMBER(pkg) \
  reinterpret_cast<IOService::IOPackage::PODData*>( \
      ((pkg)->header.buffersNumber > 1 ? \
          (pkg)->buffers + (pkg)->header.buffersNumber : \
          (pkg)->buffers + 1) )

/**
 * Same as above but const variant.
 */
#define IODATAMEMBERCONST(pkg) \
  reinterpret_cast<const IOService::IOPackage::PODData*>( \
      ((pkg)->header.buffersNumber > 1 ? \
          (pkg)->buffers + (pkg)->header.buffersNumber : \
          (pkg)->buffers + 1) )

} //namespace IOService

#endif //IOPROTOCOL_H
