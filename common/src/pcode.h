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
