/*
    Handling the configuration of SCC.
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

#include "config.h"
#include "define.h"

#include "../../common/src/exception.h"

#include <cstdio>
#include <cstring>

FILE* popen(const void*, const void*)
{
    return nullptr;
}

void pclose(const void*)
{
}

// class Config

const char* Config::cmdName = "scc";

const char* const Config::VERSION = "0.1.0";

const char *const Config::HELP =
    "Usage: scc <input file> [options]\n"
    "Options:\n"
    "  -O        --optimize        Optimize. (default)\n"
    "  -P        --no-optimize     Do not optimize.\n"
    "  -G <file> --lang <file>     Use specific lang file.\n"
    "  -E <file> --lex-only <file> Lexical analysis only and place result into <file>.\n"
    "  -e <file> --lex <file>      Place lexical analysis result into <file>.\n"
    "  -p <file> --parser <file>   Place parsing result into <file>.\n"
    "  -o <file> --object <file>   Place pcode into <file>.\n"
    "  -b        --bin             Generate binary pcode. (default)\n"
    "  -t        --text            Generate textual pcode instead of binary one.\n"
    "  -h        --help            Display this infomation.\n"
    "  -v        --version         Display compiler version information.\n"
    "\n"
    "   Set <file> to '-' to read from stdin / write to stdout.\n"
    "   Set parser file to '@' to write to the same file as lexer.\n"
    "\n"
    "Examples:\n"
    "\n"
    "  scc testfile.txt -E output.txt\n"
    "  Lexical analysis only and place result into output.txt.\n"
    "\n"
    "  scc testfile.txt -E -\n"
    "  Lexical analysis only and place result into stdin.\n"
    "\n"
    "  scc testfile.txt -e output.txt -p @\n"
    "  Place lexical analysis result and parsing result into output.txt.\n"
    "\n";

char* Config::splitPath(char* path)
{
    char* p = strrchr(path, '/');
    if (p != nullptr)
    {
        return p;
    }
    else
    {
        return strrchr(path, '\\');
    }
}

const char* Config::splitPath(const char* path)
{
    const char* p = strrchr(path, '/');
    if (p != nullptr)
    {
        return p;
    }
    else
    {
        return strrchr(path, '\\');
    }
}

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

        bin(true),

        objectFileName(nullptr),

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

    RuntimeError::setCmdName(cmdName);

    char* buffer = new char[FILENAME_MAX];
    buffer[0] = '\0';
    int PREFIX_MAX = FILENAME_MAX - strlen(langFileName);

    char* p = splitPath(argv[0]);
    if (p != nullptr)
    {
        strcpy(buffer, cmdName);
        p += buffer - cmdName;
    }
    else
    {
        FILE *fp = popen("where scc", "r");
        if (fp != nullptr)
        {
            fgets(buffer, PREFIX_MAX, fp);
            pclose(fp);
        }
        p = splitPath(buffer);
        if (p == nullptr)
        {
            fp = popen("which scc", "r");
            if (fp != nullptr)
            {
                fgets(buffer, PREFIX_MAX, fp);
                pclose(fp);
            }
            p = splitPath(buffer);
        }
    }
    if (p != nullptr)
    {
        strcpy(p + 1, langFileName);
        langFileName = buffer;
    }

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
                throw InvalidArgumentError("except option before", argv[i]); // TODO
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
                    optimize = false;
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

                case 'o':
                    fileName = &objectFileName;
                    more = true;
                    break;

                case 'b':
                    bin = true;
                    break;

                case 't':
                    bin = false;
                    break;

                case 'h':
                    printf("%s", HELP);
                    exit(0);
                    break;

                case 'v':
                    printf("scc version %s\n", VERSION);
                    exit(0);
                    break;

                default:
                    argv[i][j - 1] = '-';
                    argv[i][j + 1] = '\0';
                    throw InvalidArgumentError("unrecognized option", argv[i] + j - 1, HELP);
                }

                if (more)
                {
                    argv[i][1] = argv[i][j];
                    argv[i][2] = argv[i][j + 1];
                    if (argv[i][2] != '\0')
                    {
                        argv[i][2] = '\0';
                        throw InvalidArgumentError("missing filename after", argv[i]);
                    }
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
            else if (strcmp(argv[i] + 2, "object") == 0)
            {
                fileName = &parserFileName;
            }
            else if (strcmp(argv[i] + 2, "bin") == 0)
            {
                bin = true;
            }
            else if (strcmp(argv[i] + 2, "text") == 0)
            {
                bin = false;
            }
            else if (strcmp(argv[i] + 2, "help") == 0)
            {
                printf("%s", HELP);
                exit(0);
            }
            else if (strcmp(argv[i] + 2, "version") == 0)
            {
                printf("scc version %s\n", VERSION);
                exit(0);
            }
            else if (argv[i][2] == '\0')
            {
                // TODO
            }
            else
            {
                throw InvalidArgumentError("unrecognized option", argv[i], HELP);
            }
        }

        if (fileName != nullptr)
        {
            i++;
            if (i >= argc)
            {
                throw InvalidArgumentError("missing filename after", argv[i - 1]);
            }
            if (strcmp(argv[i], "--") == 0)
            {
                i++;
            }
            if (i >= argc)
            {
                throw InvalidArgumentError("missing filename after", argv[i - 2]);
            }
            *fileName = argv[i];
        }

        // TODO
    }

    if (inputFileName == nullptr)
    {
        throw InvalidArgumentError("no input file", nullptr, HELP);
    }

#endif

#if !defined(CG) || CG == 4

    if (objectFileName == nullptr)
    {
        const char* ext = bin ? ".bpc" : ".tpc";
        int n = strlen(inputFileName);
        buffer = new char[n + 5];
        strcpy(buffer, inputFileName);
        p = strrchr(buffer, '.');
        if (p != nullptr)
        {
            strcpy(p, ext);
        }
        else
        {
            strcpy(buffer + n, ext);
        }
        objectFileName = buffer;
    }

#endif

}
