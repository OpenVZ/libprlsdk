/*
 * IORoutingTable.h
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


#ifndef IOROUTINGTABLE_H
#define IOROUTINGTABLE_H

#include <QPair>
#include <QReadWriteLock>
#include <QHash>

#include "IOProtocol.h"

namespace IOService {

/**
 * Routing table.
 * Is used for quick routes searching by package type.
 */
class IORoutingTable
{
public:
    /** Route requirements */
    enum RouteRequirement  {
        OptionalRoute = 0,   /**< Route is suggested, but is not required */
        RequiredRoute = 1    /**< Route is required. */
    };

    /** Route name */
    enum RouteName {
        UnknownRoute = 0, /**< Unknown route name */
        PlainRoute   = 1, /**< Plain data route */
        SSLRoute     = 2  /**< SSL data route */
    };

    /** Declare some max constants */
    enum MaxNumbers { MaxRoutesNumber = 20 };

#include <Interfaces/packed.h>
    /** Routing table header */
    struct Header
    {
        RouteName        defaultRouteName;    /**< Default route name */
        RouteRequirement defaultRouteReq;     /**< Default route req. */
        quint8           routesNumber;        /**< Direct or range routes
                                                   number.
                                                  Use #DirectRoute structure.
                                                  This number must be <=
                                                       #MaxRoutesNumber.*/
    } PACKED;
#include <Interfaces/unpacked.h>

#include <Interfaces/packed.h>
    /** Direct or range routes. */
    struct DirectRangeRoute
    {
        IOPackage::Type  pkgTypeBegin; /**< Package begin type */
        IOPackage::Type  pkgTypeEnd;   /**< Package end type */
        RouteName        routeName;    /**< Route name */
        RouteRequirement routeReq;     /**< Route requirement */

    } PACKED;
#include <Interfaces/unpacked.h>

    /**
     * Default constructor.
     * Table will be null.
     * @see #isNull
     */
    IORoutingTable ();

    /**
     * Constructs routing table with default route.
     */
    IORoutingTable ( RouteName defaultRouteName,
                     RouteRequirement defaultRouteRequirement );

    /**
     * Constructs routing table with default route and possible other routes.
     */
    IORoutingTable ( RouteName defaultRouteName,
                     RouteRequirement defaultRouteRequirement,
                     const QList<RouteName>& );


    /**
     * Copy constructor
     */
    IORoutingTable ( const IORoutingTable& table );

    /**
     * Alignment operator
     */
    IORoutingTable& operator= ( const IORoutingTable& table );

    /**
     * If table was constructed with default constructor,
     * or default route name is empty, true will be returned.
     */
    bool isNull () const;

    /**
     * Try to accept 'in' table.
     * While comparison new table will be created and saved to 'out'
     * @return true will be returnrf if accepting successfully done,
     *         'out' will contain new accepted table
     *         false -- if 'in' table can't be accepted.
     */
    bool accept ( const IORoutingTable& in, IORoutingTable& out ) const;

    /**
     * Adds route to routing table.
     *
     * @param pkgType type of package to be routed.
     * @param routeName route name.
     * @param routeRequirement route requirement.
     * @return true if route has been successfully added, false otherwise.
     */
    bool addRoute ( IOPackage::Type pkgType,
                    RouteName routeName,
                    RouteRequirement routeRequirement );

    /**
     * Adds route to routing table.
     * Adds range of types to be routed.
     *
     * @param pkgTypeBegin begin type of package to be routed.
     * @param pkgTypeEnd end type of package to be routed.
     * @param routeName route name.
     * @param routeRequirement route requirement.
     * @return true if route has been successfully added, false otherwise.
     */
    bool addRoute ( IOPackage::Type pkgTypeBegin,
                    IOPackage::Type pkgTypeEnd,
                    RouteName routeName,
                    RouteRequirement routeRequirement );

    /**
     * Finds route by specified package type.
     * Return #UnknownRoute if route was not found.
     */
    RouteName findRoute ( IOPackage::Type pkgType ) const;

    /**
     * Returns binary buffer, which describes this routing table.
     * If routing table is null (see #isNull), buffer will contain
     * routing table header, which describes this situation.
     *
     * @param size [out] size of buffer, which will be returned.
     */
    SmartPtr<char> toBuffer ( quint32& size ) const;

    /**
     * Returns routing table, which has been created from binary buffer.
     * If error has been occured, res param will be false and null table
     * will be returned (see #isNull).
     *
     * @param buff [in] buffer, from routing table will be created.
     * @param size [in] size of buffer.
     * @param res  [out] if parse succeeded, 'res' will be set to true.
     */
    static IORoutingTable fromBuffer ( const SmartPtr<char>& buff,
                                       quint32 size, bool& res );

    /**
     * Validates routing table header and returns full size of routing table.
     * 0 will be returned, if error occurred.
     */
    static quint32 validateHeader ( const IORoutingTable::Header& );


private:
    typedef QPair<IOPackage::Type, IOPackage::Type> TypeRange;
    typedef QPair<RouteName, RouteRequirement> RoutePair;

    /**
     * Returns true if route pair has been accepted.
     * @note Beware: caller must be locked for read before call.
     */
    bool acceptRoutePair ( const RoutePair&,
                           const RoutePair&, RoutePair& ) const;

public:
    /** Static null table */
    static IORoutingTable __attribute__((visibility("hidden"))) NullTable;

private:
    mutable QReadWriteLock m_rwLock;
    RouteName m_defaultRouteName;
    RouteRequirement m_defaultRouteRequirement;
    bool m_onlyDefaultRoute;
    QHash<TypeRange, RoutePair> m_fullRouteMap;
    QHash<RouteName, RouteName> m_routeNameMap;
};

} //namespace IOService

#endif //IOROUTINGTABLE_H
