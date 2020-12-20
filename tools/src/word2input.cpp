#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cwchar>

#define CMD_NAME L"word2input: "
#define ERROR_PREFIX CMD_NAME L"error: "
#define FATAL_ERROR_PREFIX CMD_NAME L"fatal error: "

int word2input(int argc, char** argv)
{
    FILE *ifp = stdin, *ofp = stdout;
    if (argc == 3)
    {
        ifp = fopen(argv[1], "r");
        if (ifp == nullptr)
        {
            fwprintf(stderr, ERROR_PREFIX L"%s: %s\n", argv[1], strerror(errno));
            fwprintf(stderr, FATAL_ERROR_PREFIX L"Unable to open input file\n");
            exit(1);
        }
        ofp = fopen(argv[2], "w");
        if (ofp == nullptr)
        {
            fwprintf(stderr, ERROR_PREFIX L"%s: %s\n", argv[2], strerror(errno));
            fwprintf(stderr, FATAL_ERROR_PREFIX L"Unable to open output file\n");
            exit(1);
        }
    }
    else if (argc != 1)
    {
        fwprintf(stderr, FATAL_ERROR_PREFIX L"invalid arguments: 0 or 2 arguments are required\n");
        exit(1);
    }
    wchar_t typeName[256], val[4096];
    int n;
    bool newline = true, space = false;
    int level = 0;
    while (fwscanf(ifp, L"%255ls ", typeName) >= 1)
    {
        fgetws(val, 4096, ifp);
        n = wcslen(val);
        if (val[n - 1] == L'\n')
        {
            val[n - 1] = L'\0';
        }
        if (wcscmp(typeName, L"CHARCON") == 0)
        {
            if (newline)
            {
                for (int i = 0; i < level; i++)
                {
                    fwprintf(ofp, L"    ");
                }
            }
            fwprintf(ofp, L"'%ls'", val);
            space = true;
            newline = false;
        }
        else if (wcscmp(typeName, L"STRCON") == 0)
        {
            if (newline)
            {
                for (int i = 0; i < level; i++)
                {
                    fwprintf(ofp, L"    ");
                }
            }
            fwprintf(ofp, L"\"%ls\"", val);
            space = true;
            newline = false;
        }
        else if (wcscmp(typeName, L"LBRACE") == 0)
        {
            if (newline)
            {
                for (int i = 0; i < level; i++)
                {
                    fwprintf(ofp, L"    ");
                }
            }
            fwprintf(ofp, L"%ls\n", val);
            space = false;
            newline = true;
            level++;
        }
        else if (wcscmp(typeName, L"RBRACE") == 0)
        {
            if (newline)
            {
                for (int i = 0; i < level - 1; i++)
                {
                    fwprintf(ofp, L"    ");
                }
            }
            if (!newline)
            {
                fwprintf(ofp, L"\n");
            }
            fwprintf(ofp, L"%ls\n", val);
            space = false;
            newline = true;
            level--;
        }
        else if (wcscmp(typeName, L"SEMICN") == 0)
        {
            if (newline)
            {
                for (int i = 0; i < level; i++)
                {
                    fwprintf(ofp, L"    ");
                }
            }
            fwprintf(ofp, L"%ls\n", val);
            space = false;
            newline = true;
        }
        else if (wcscmp(typeName, L"LPARENT") == 0 || wcscmp(typeName, L"RPARENT") == 0)
        {
            if (newline)
            {
                for (int i = 0; i < level; i++)
                {
                    fwprintf(ofp, L"    ");
                }
            }
            fwprintf(ofp, L"%ls", val);
            space = false;
            newline = false;
        }
        else if (wcscmp(typeName, L"COMMA") == 0)
        {
            if (newline)
            {
                for (int i = 0; i < level; i++)
                {
                    fwprintf(ofp, L"    ");
                }
            }
            fwprintf(ofp, L"%ls", val);
            space = true;
            newline = false;
        }
        else
        {
            if (newline)
            {
                for (int i = 0; i < level; i++)
                {
                    fwprintf(ofp, L"    ");
                }
            }
            if (space)
            {
                fwprintf(ofp, L" ");
            }
            fwprintf(ofp, L"%ls", val);
            space = true;
            newline = false;
        }
        
    }
    fclose(ofp);
    fclose(ifp);
    return 0;
}
