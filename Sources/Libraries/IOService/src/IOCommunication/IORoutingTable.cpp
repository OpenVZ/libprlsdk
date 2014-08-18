/*
 * IORoutingTable.cpp
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


#include "IORoutingTable.h"
#include "Libraries/Logging/Logging.h"

using namespace IOService;

/*****************************************************************************/

IORoutingTable IORoutingTable::NullTable;

/*****************************************************************************/

IORoutingTable::IORoutingTable () :
    m_defaultRouteName(UnknownRoute),
    m_defaultRouteRequirement(OptionalRoute),
    m_onlyDefaultRoute(true)
{}

IORoutingTable::IORoutingTable (
    RouteName defaultRouteName,
    RouteRequirement defaultRouteRequirement ) :
    m_defaultRouteName(defaultRouteName),
    m_defaultRouteRequirement(defaultRouteRequirement),
    m_onlyDefaultRoute(true)
{
    m_routeNameMap[defaultRouteName] = defaultRouteName;
}

IORoutingTable::IORoutingTable (
    RouteName defaultRouteName,
    RouteRequirement defaultRouteRequirement,
    const QList<RouteName>& routeNames ) :
    m_defaultRouteName(defaultRouteName),
    m_defaultRouteRequirement(defaultRouteRequirement),
    m_onlyDefaultRoute(true)
{
    m_routeNameMap[defaultRouteName] = defaultRouteName;
    foreach ( RouteName routeName, routeNames )
        m_routeNameMap[routeName] = routeName;
}

IORoutingTable::IORoutingTable ( const IORoutingTable& table ) :
    m_defaultRouteName(UnknownRoute),
    m_defaultRouteRequirement(OptionalRoute),
    m_onlyDefaultRoute(true)
{
    QReadLocker locker( &table.m_rwLock );

    m_defaultRouteName = table.m_defaultRouteName;
    m_defaultRouteRequirement = table.m_defaultRouteRequirement;
    m_onlyDefaultRoute = table.m_onlyDefaultRoute;
    m_fullRouteMap = table.m_fullRouteMap;
    m_routeNameMap = table.m_routeNameMap;
}

IORoutingTable& IORoutingTable::operator= ( const IORoutingTable& table )
{
    QReadLocker rdLocker( &table.m_rwLock );

    RouteName defaultRouteName = table.m_defaultRouteName;
    RouteRequirement defaultRouteRequirement = table.m_defaultRouteRequirement;
    bool onlyDefaultRoute = table.m_onlyDefaultRoute;
    QHash<TypeRange, RoutePair> fullRouteMap = table.m_fullRouteMap;
    QHash<RouteName, RouteName> routeNameMap = table.m_routeNameMap;

    rdLocker.unlock();

    QWriteLocker wrLocker( &m_rwLock );

    m_defaultRouteName = defaultRouteName;
    m_defaultRouteRequirement = defaultRouteRequirement;
    m_onlyDefaultRoute = onlyDefaultRoute;
    m_fullRouteMap = fullRouteMap;
    m_routeNameMap = routeNameMap;

    return *this;
}

bool IORoutingTable::isNull () const
{
    QReadLocker locker( &m_rwLock );
    return m_defaultRouteName == UnknownRoute;
}

bool IORoutingTable::acceptRoutePair (
    const RoutePair& routePair,
    const RoutePair& in,
    RoutePair& out ) const
{
    // NOTE: caller of this function must be locked for read

    if ( routePair.second == OptionalRoute ) {
        if ( in.second == OptionalRoute ) {
            bool exists = m_routeNameMap.contains( in.first );
            if ( ! exists ) {
                out.first = routePair.first;
                out.second = routePair.second;
            }
            else {
                out.first = in.first;
                out.second = in.second;
            }
        }
        else {
            bool exists = m_routeNameMap.contains( in.first );
            if ( ! exists ) {
                return false;
            }
            else {
                out.first = in.first;
                out.second = in.second;
            }
        }
    }
    else {
        if ( routePair.first == in.first ) {
            out.first = routePair.first;
            out.second = routePair.second;
        }
        else {
            if ( in.second == OptionalRoute ) {
                out.first = routePair.first;
                out.second = routePair.second;
            }
            else {
                return false;
            }
        }
    }

    return true;
}

bool IORoutingTable::accept (
    const IORoutingTable& inT, IORoutingTable& outT ) const
{
    // Make a copy
    IORoutingTable in = inT;
    // New out table
    IORoutingTable out;

    // Lock
    QReadLocker locker( &m_rwLock );

    // Check default route first of all
    {
        RoutePair routePair( m_defaultRouteName,
                             m_defaultRouteRequirement );
        RoutePair inRoutePair( in.m_defaultRouteName,
                               in.m_defaultRouteRequirement );
        RoutePair outRoutePair;

        bool accepted = acceptRoutePair( routePair,
                                         inRoutePair, outRoutePair );
        if ( ! accepted ) {
            return false;
        }
        out.m_defaultRouteName = outRoutePair.first;
        out.m_defaultRouteRequirement = outRoutePair.second;
    }

    // Merge non existent routes
    foreach ( TypeRange typeRange, m_fullRouteMap.keys() ) {
        if ( in.m_fullRouteMap.contains(typeRange) )
            continue;

        RoutePair routePair;

        if ( m_defaultRouteRequirement == IORoutingTable::OptionalRoute ) {
            routePair = RoutePair( out.m_defaultRouteName,
                                   out.m_defaultRouteRequirement );
        }
        else {
            routePair = RoutePair( m_defaultRouteName,
                                   m_defaultRouteRequirement );
        }

        in.m_fullRouteMap[typeRange] = routePair;
    }

    // Check 'in' routes
    foreach ( TypeRange typeRange, in.m_fullRouteMap.keys() ) {
        RoutePair& inRoutePair = in.m_fullRouteMap[typeRange];

        if ( m_fullRouteMap.contains(typeRange) ) {
            const RoutePair routePair = m_fullRouteMap.value(typeRange);
            RoutePair outRoutePair;
            bool accepted = acceptRoutePair( routePair, inRoutePair,
                                             outRoutePair );
            if ( ! accepted ) {
                return false;
            }
            out.m_fullRouteMap[typeRange] = outRoutePair;
        }
        else {
            if ( inRoutePair.second == OptionalRoute ) {
                bool exists = m_routeNameMap.contains( inRoutePair.first );
                if ( ! exists ) {
                    RoutePair defOutPair( out.m_defaultRouteName,
                                          out.m_defaultRouteRequirement );
                    out.m_fullRouteMap[typeRange] = defOutPair;
                }
                else {
                    out.m_fullRouteMap[typeRange] = inRoutePair;
                }
            }
            else {
                bool exists = m_routeNameMap.contains( inRoutePair.first );
                if ( ! exists ) {
                    return false;
                }
                else {
                    out.m_fullRouteMap[typeRange] = inRoutePair;
                }
            }
        }
    }

    // Drop default route flag if we have other routes
    if ( out.m_fullRouteMap.size() > 0 ) {
        out.m_onlyDefaultRoute = false;
    }

    // Save out table
    outT = out;
    return true;
}

bool IORoutingTable::addRoute ( IOPackage::Type pkgType,
                                RouteName routeName,
                                RouteRequirement routeRequirement )
{
    return addRoute( pkgType, pkgType, routeName, routeRequirement );
}

bool IORoutingTable::addRoute ( IOPackage::Type pkgTypeBegin,
                                IOPackage::Type pkgTypeEnd,
                                RouteName routeName,
                                RouteRequirement routeRequirement )
{
    if ( pkgTypeBegin > pkgTypeEnd ) {
        LOG_MESSAGE(DBG_FATAL, "Range error in routing table: begin > end");
        return false;
    }

    TypeRange typeRange( pkgTypeBegin, pkgTypeEnd );

    // Lock
    QWriteLocker locker( &m_rwLock );

    if ( m_fullRouteMap.size() == MaxRoutesNumber )
        return false;

    if ( m_fullRouteMap.contains(typeRange) )
        return false;

    foreach ( TypeRange range, m_fullRouteMap.keys() ) {
        if ( (range.first <= pkgTypeBegin && range.second >= pkgTypeBegin) ||
             (range.first <= pkgTypeEnd && range.second >= pkgTypeEnd) )
            return false;
    }

    m_onlyDefaultRoute = false;
    m_fullRouteMap[typeRange] = QPair<RouteName, RouteRequirement>(
                                            routeName, routeRequirement );
    m_routeNameMap[routeName] = routeName;

    return true;
}

IORoutingTable::RouteName IORoutingTable::findRoute (
    IOPackage::Type pkgType ) const
{
    // Quick getter for single connection
    if ( m_onlyDefaultRoute )
        return m_defaultRouteName;

    TypeRange typeRange( pkgType, pkgType );

    // Direct search
    QReadLocker locker( &m_rwLock );
    if ( m_fullRouteMap.contains(typeRange) )
        return m_fullRouteMap[typeRange].first;

    // Range search
    QHash<TypeRange, RoutePair>::ConstIterator it = m_fullRouteMap.begin();
    while ( it != m_fullRouteMap.end() ) {
        if ( it.key().first <= pkgType && it.key().second >= pkgType )
            return it.value().first;
        ++it;
    }

    // Default route
    if ( m_defaultRouteName )
        return m_defaultRouteName;

    return IORoutingTable::UnknownRoute;
}

/*****************************************************************************/

SmartPtr<char> IORoutingTable::toBuffer ( quint32& outSize ) const
{
    QReadLocker rdLocker( &m_rwLock );

    quint32 routesNumber = m_fullRouteMap.size();
    if ( routesNumber > MaxRoutesNumber )
        return SmartPtr<char>();

    // Save size
    outSize = sizeof(IORoutingTable::Header) +
              sizeof(IORoutingTable::DirectRangeRoute) * routesNumber;

    // Create table buff
    SmartPtr<char> rtBuff( new(std::nothrow) char[outSize], SmartPtrPolicy::ArrayStorage );
    if ( ! rtBuff.isValid() )
        return SmartPtr<char>();

    IORoutingTable::Header* header =
        reinterpret_cast<IORoutingTable::Header*>(rtBuff.getImpl());
    header->defaultRouteName = m_defaultRouteName;
    header->defaultRouteReq = m_defaultRouteRequirement;
    header->routesNumber = routesNumber;

    IORoutingTable::DirectRangeRoute* routes =
        reinterpret_cast<IORoutingTable::DirectRangeRoute*>(header + 1);
    // Iterate over routes
    QHash<TypeRange, RoutePair>::ConstIterator it = m_fullRouteMap.begin();
    for ( quint32 i = 0; it != m_fullRouteMap.end(); ++it, ++i ) {
        routes[i].pkgTypeBegin = it.key().first;
        routes[i].pkgTypeEnd = it.key().second;
        routes[i].routeName = it.value().first;
        routes[i].routeReq = it.value().second;
    }

    return rtBuff;
}

IORoutingTable IORoutingTable::fromBuffer (
    const SmartPtr<char>& rtBuff,
    quint32 size,
    bool& res )
{
    // Set to false;
    res = false;

    if ( size < sizeof(IORoutingTable::Header) )
        return NullTable;

    const IORoutingTable::Header* header =
        reinterpret_cast<const IORoutingTable::Header*>(rtBuff.getImpl());

    quint32 routesNumber = header->routesNumber;
    if ( routesNumber > MaxRoutesNumber )
        return NullTable;

    // Full size
    quint32 fullSize = sizeof(IORoutingTable::Header) +
                       sizeof(IORoutingTable::DirectRangeRoute) * routesNumber;
    if ( size != fullSize )
        return NullTable;

    const IORoutingTable::DirectRangeRoute* routes =
        reinterpret_cast<const IORoutingTable::DirectRangeRoute*>(header + 1);
    IORoutingTable routingTable( header->defaultRouteName,
                                 header->defaultRouteReq );
    for ( quint32 i = 0; i < routesNumber; ++i ) {
        bool res = routingTable.addRoute( routes[i].pkgTypeBegin,
                                          routes[i].pkgTypeEnd,
                                          routes[i].routeName,
                                          routes[i].routeReq );
        if ( ! res )
            return NullTable;
    }

    // Set to true
    res = true;

    return routingTable;
}

quint32 IORoutingTable::validateHeader ( const IORoutingTable::Header& header )
{
    quint32 routesNumber = header.routesNumber;
    if ( routesNumber > MaxRoutesNumber )
        return 0;

    // Full size
    quint32 fullSize = sizeof(IORoutingTable::Header) +
                       sizeof(IORoutingTable::DirectRangeRoute) * routesNumber;

    return fullSize;
}

/*****************************************************************************/
