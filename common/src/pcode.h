/*
    Definition of PCODE of SCC.
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

#ifndef _SCI_BPCODE_H_
#define _SCI_BPCODE_H_

#include <cstdint>

namespace sci
{

    struct BPcode
    {
        unsigned f;
        int a;
    };

    union PcodeF
    {
        char name[4];
        uint32_t id;
    };

    struct TPcode
    {
        PcodeF f;
        unsigned l;
        int a;
    };

    const char BPCODE_PREFIX[] = "\200BPCODE";

    const char TPCODE_DATA[] = ".data";

    const char TPCODE_CODE[] = ".code";

    const PcodeF fs[] =
    {
        {.name = "POP"},
        {.name = "LIT"},
        {.name = "LOD"},
        {.name = "STO"},
        {.name = "CAL"},
        {.name = "INT"},
        {.name = "JMP"},
        {.name = "JPC"},
        {.name = "OPR"},
        {.name = "LDA"},
        {.name = "STA"},
        {.name = "STR"},
    };

} // namespace sci


#endif // _SCI_BPCODE_H_
