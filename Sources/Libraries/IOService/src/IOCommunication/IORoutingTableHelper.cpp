/*
 * IORoutingTableHelper.cpp
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

#include "IORoutingTableHelper.h"
#include <boost/mpl/pair.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/integral_c.hpp>

using namespace IOService;

namespace
{
namespace mpl = boost::mpl;

///////////////////////////////////////////////////////////////////////////////
// struct Visitor

struct Visitor
{
	Visitor(): m_result
		(IORoutingTable::PlainRoute, IORoutingTable::RequiredRoute)
	{
	}

	template<IOPackage::Type I>
	void operator()(const mpl::integral_c<IOPackage::Type, I>& )
	{
		bool x = m_result.addRoute(I, IORoutingTable::SSLRoute,
				IORoutingTable::RequiredRoute);
		Q_ASSERT(x);
		Q_UNUSED(x);
	}
	template<IOPackage::Type A, IOPackage::Type B>
	void operator()(const mpl::pair<
				mpl::integral_c<IOPackage::Type, A>,
				mpl::integral_c<IOPackage::Type, B> >& )
	{
		bool x = m_result.addRoute(A, B, IORoutingTable::SSLRoute,
				IORoutingTable::RequiredRoute);
		Q_ASSERT(x);
		Q_UNUSED(x);
	}
	const IORoutingTable& getResult() const
	{
		return m_result;
	}

private:
	IORoutingTable m_result;
};
typedef mpl::vector<
		mpl::integral_c<IOPackage::Type, PET_IO_CLI_KEYBOARD_SCANCODE>,
		mpl::integral_c<IOPackage::Type, PET_IO_CLI_TOOLS_CLIPBOARD_DATA>,
		mpl::integral_c<IOPackage::Type, PET_IO_CLI_AUTHENTICATE_SESSION>,
		mpl::integral_c<IOPackage::Type, PET_IO_CLI_ATTACH_TO_VM>,
		mpl::pair<
			mpl::integral_c<IOPackage::Type, PVE::DspClientToVmCommandRangeStart>,
			mpl::integral_c<IOPackage::Type, PVE::DspClientToVmCommandRangeEnd> >,
		mpl::pair<
			mpl::integral_c<IOPackage::Type, PVE::DspClientToDispatcherCommandRangeStart>,
			mpl::integral_c<IOPackage::Type, PVE::DspClientToDispatcherCommandRangeEnd> >,
		mpl::pair<
			mpl::integral_c<IOPackage::Type, PVE::DspVmToClientCommandRangeStart>,
			mpl::integral_c<IOPackage::Type, PVE::DspVmToClientCommandRangeEnd> >,
		mpl::pair<
			mpl::integral_c<IOPackage::Type, PVE::DspCtlCommandRangeStart>,
			mpl::integral_c<IOPackage::Type, PVE::DspCtlCommandRangeEnd> >,
		mpl::pair<
			mpl::integral_c<IOPackage::Type, PVE::DspWsCommandRangeStart>,
			mpl::integral_c<IOPackage::Type, PVE::DspWsCommandRangeEnd> >,
		mpl::pair<
			mpl::integral_c<IOPackage::Type, PVE::DispToDispRangeStart>,
			mpl::integral_c<IOPackage::Type, PVE::DispToDispRangeEnd> >
	>::type normalRouteList_type;

} // namespace

/*****************************************************************************/

const IORoutingTable IORoutingTableHelper::GetClientRoutingTable (
    PRL_SECURITY_LEVEL security)
{
	if (PSL_HIGH_SECURITY == security)
	{
		return IORoutingTable(IORoutingTable::SSLRoute,
			IORoutingTable::RequiredRoute);
	}
	Visitor v;
	if (PSL_NORMAL_SECURITY == security)
		mpl::for_each<normalRouteList_type>(boost::ref(v));
	else
		Q_ASSERT(PSL_LOW_SECURITY == security);
	
	return v.getResult();
}

/*****************************************************************************/
