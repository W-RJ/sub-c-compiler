#include <cstdio>
#include <cstring>

#include "config.h"
#include "exception.h"
#include "define.h"

// class Config

const char* Config::cmdName = "scc";

const char* const Config::VERSION = "1.0";

const char* const Config::HELP =
        "Usage: scc <input file> [options]\n"
        "Options:\n"
        "  -O        --optimize        Optimize.\n"
        "  -P        --no-optimize     Do not optimize.\n"
        "  -G <file> --lang <file>     Use specific lang file.\n"
        "  -E <file> --lex-only <file> Lexical analysis only and place result into <file>.\n"
        "  -e <file> --lex <file>      Place lexical analysis result into <file>.\n"
        "  -p <file> --parser <file>   Place parsing result into <file>.\n"
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
        FILE* fp = popen("which scc", "r");
        if (fp != nullptr)
        {
            fgets(buffer, PREFIX_MAX, fp);
            pclose(fp);
        }
        p = splitPath(buffer);
        if (p == nullptr)
        {
            fp = popen("where scc", "r");
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

}
