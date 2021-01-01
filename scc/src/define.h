/*
    Macros for build options.
    Copyright (C) 2020-2021 Renjian Wang

    This file is part of SCC.

    SCC is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SCC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SCC.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef _SCC_DEFINE_H_
#define _SCC_DEFINE_H_

#if !defined(CG) && !defined(NCG)
#define CG 1
#endif

#if !defined(DEBUG) && !defined(NDEBUG)
#define NDEBUG
#endif

#endif // _SCC_DEFINE_H_
