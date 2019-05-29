/*
 * PP.h: Preprocessor (PP) helpers
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019 Virtuozzo International GmbH. All rights reserved.
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


#ifndef PARALLELS_TRANSPORTER_AGENT_API_PP_H
#define PARALLELS_TRANSPORTER_AGENT_API_PP_H

/// Preprocessor helpers

# define PT_PP_BOOL_0 0
# define PT_PP_BOOL_1 1
# define PT_PP_BOOL_2 1
# define PT_PP_BOOL_3 1
# define PT_PP_BOOL_4 1
# define PT_PP_BOOL_5 1
# define PT_PP_BOOL_6 1
# define PT_PP_BOOL_7 1
# define PT_PP_BOOL_8 1
# define PT_PP_BOOL_9 1
# define PT_PP_BOOL_10 1
# define PT_PP_BOOL_11 1
# define PT_PP_BOOL_12 1

# define PT_PP_COMMA() ,
# define PT_PP_EMPTY()

#define PT_PP_BOOL(x) PT_PP_BOOL_I(x)
#define PT_PP_BOOL_I(x) PT_PP_BOOL_ ## x

/////////////

# define PT_PP_IIF_0(t, f) f
# define PT_PP_IIF_1(t, f) t

#define PT_PP_IIF(bit, t, f) PT_PP_IIF_I(bit, t, f)
#define PT_PP_IIF_I(bit, t, f) PT_PP_IIF_II(PT_PP_IIF_ ## bit(t, f))
#define PT_PP_IIF_II(id) id

#define PT_PP_IF(cond, t, f) PT_PP_IF_I(cond, t, f)
#define PT_PP_IF_I(cond, t, f) PT_PP_IIF(PT_PP_BOOL(cond), t, f)

/////////////

#define PT_PP_COMMA_IF(cond) PT_PP_COMMA_IF_I(cond)
#define PT_PP_COMMA_IF_I(cond) PT_PP_IF(cond, PT_PP_COMMA, PT_PP_EMPTY)()

///////////////

#define PP_MAKE_0(m, d)
#define PP_MAKE_1(m, d) m(0, d)
#define PP_MAKE_2(m, d) m(0, d) m(1, d)
#define PP_MAKE_3(m, d) m(0, d) m(1, d) m(2, d)
#define PP_MAKE_4(m, d) m(0, d) m(1, d) m(2, d) m(3, d)
#define PP_MAKE_5(m, d) m(0, d) m(1, d) m(2, d) m(3, d) m(4, d)
#define PP_MAKE_6(m, d) m(0, d) m(1, d) m(2, d) m(3, d) m(4, d) m(5, d)
#define PP_MAKE_7(m, d) m(0, d) m(1, d) m(2, d) m(3, d) m(4, d) m(5, d) m(6, d)
#define PP_MAKE_8(m, d) m(0, d) m(1, d) m(2, d) m(3, d) m(4, d) m(5, d) m(6, d) m(7, d)
#define PP_MAKE_9(m, d) m(0, d) m(1, d) m(2, d) m(3, d) m(4, d) m(5, d) m(6, d) m(7, d) m(8, d)
#define PP_MAKE_10(m, d) m(0, d) m(1, d) m(2, d) m(3, d) m(4, d) m(5, d) m(6, d) m(7, d) m(8, d) m(9, d)
#define PP_MAKE_11(m, d) m(0, d) m(1, d) m(2, d) m(3, d) m(4, d) m(5, d) m(6, d) m(7, d) m(8, d) m(9, d) m(10, d)
#define PP_MAKE_12(m, d) m(0, d) m(1, d) m(2, d) m(3, d) m(4, d) m(5, d) m(6, d) m(7, d) m(8, d) m(9, d) m(10, d) m(11, d)


#endif // PARALLELS_TRANSPORTER_AGENT_API_PP_H
