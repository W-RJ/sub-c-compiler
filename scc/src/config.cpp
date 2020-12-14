#include "config.h"
#include "exception.h"
#include "define.h"

// class Config

const char* Config::cmdName = "scc";

Config::Config() :

        langFileName("sc.lang"),

#ifdef CG
        inputFileName("testfile.txt"),
#else
        inputFileName(nullptr),
#endif

#if defined(CG) && CG == 1
        lexOnly(true),
#else
        lexOnly(false),
#endif

#if defined(CG) && (CG == 1 || CG == 2)
        lexFileName("output.txt"),
#else
        lexFileName(nullptr),
#endif

#if defined(CG) && CG == 2
        parserFileName("&"),
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
}

void Config::set(int argc, char **argv)
{
    cmdName = argv[0];

#ifndef CG
    for (int i = 1; i < argc; i++)
    {
        // TODO
    }
#endif

}
