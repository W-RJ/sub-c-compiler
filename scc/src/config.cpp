#include <cstring>

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
        parserFileName("@"),
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

    bool hasInputFile = false;
    for (int i = 1; i < argc; i++)
    {
        const char** fileName = nullptr;
        // TODO: help
        if (argv[i][0] != '-' || argv[i][1] == '\0')
        {
            // TODO
            if (!hasInputFile)
            {
                fileName = &inputFileName;
                hasInputFile = true;
                i--;
            }
            else
            {
                throw InvalidArgumentError(L"except option before", argv[i]); // TODO
            }
        }
        else if (argv[i][1] != '-')
        {
            // TODO
            for (int j = 1; argv[i][j] != '\0'; j++)
            {
                bool more = false;

                switch (argv[i][j])
                {
                case 'O':
                    optimize = true;
                    break;

                case 'P':
                    optimize = true;
                    break;

                case 'G':
                    fileName = &langFileName;
                    more = true;
                    break;

                case 'E':
                    lexOnly = true;
                    fileName = &lexFileName;
                    more = true;
                    break;

                case 'e':
                    fileName = &lexFileName;
                    more = true;
                    break;

                case 'p':
                    fileName = &parserFileName;
                    more = true;
                    break;

                default:
                    argv[i][j - 1] = '-';
                    argv[i][j + 1] = '\0';
                    throw InvalidArgumentError(L"unrecognized option", argv[i] + j - 1);
                }

                if (more)
                {
                    if (argv[i][j + 1] != '\0')
                    {
                        argv[i][j - 1] = '-';
                        argv[i][j + 1] = '\0';
                        throw InvalidArgumentError(L"missing filename after", argv[i] + j - 1);
                    }
                    argv[i][1] = argv[i][j];
                    argv[i][2] = '\0';
                    break;
                }
            }
        }
        else
        {
            if (strcmp(argv[i] + 2, "optimize") == 0)
            {
                optimize = true;
            }
            else if (strcmp(argv[i] + 2, "no-optimize") == 0)
            {
                optimize = false;
            }
            else if (strcmp(argv[i] + 2, "lang") == 0)
            {
                fileName = &langFileName;
            }
            else if (strcmp(argv[i] + 2, "lex-only") == 0)
            {
                lexOnly = true;
                fileName = &lexFileName;
            }
            else if (strcmp(argv[i] + 2, "lex") == 0)
            {
                fileName = &lexFileName;
            }
            else if (strcmp(argv[i] + 2, "parser") == 0)
            {
                fileName = &parserFileName;
            }
            else if (argv[i][2] == '\0')
            {
                // TODO
            }
            else
            {
                throw InvalidArgumentError(L"unrecognized option", argv[i]);
            }
        }

        if (fileName != nullptr)
        {
            i++;
            if (i >= argc)
            {
                throw InvalidArgumentError(L"missing filename after", argv[i - 1]);
            }
            *fileName = argv[i];
        }

        // TODO
    }

    if (inputFileName == nullptr)
    {
        throw InvalidArgumentError(L"no input file", nullptr);
    }

#endif

}
