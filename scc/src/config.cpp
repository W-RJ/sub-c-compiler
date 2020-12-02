#include "config.h"

// class Config
Config::Config(int argc, char **argv) :

#ifdef CG
        inputFileName("testfile.txt"),
#else
        inputFileName(nullptr),
#endif

#if defined(CG) && CG == 1
        lexOnly(true),
        lexFileName("output.txt"),
#else
        lexOnly(false),
        lexFileName(nullptr),
#endif

#if defined(CG) && CG == 2
        parserFileName("output.txt"),
#else
        parserFileName(nullptr),
#endif

#if defined(CG) && CG == 3
        errFileName("error.txt"),
#else
        errFileName("-"),
#endif

#if defined(CG) && CG == 5
        mipsFileName("mips.txt"),
#else
        mipsFileName(nullptr),
#endif

        optimize(true)
{
#ifndef CG
    for (int i = 1; i < argc; i++)
    {
        // TODO
    }
#endif
}
