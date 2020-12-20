#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cwchar>

#define CMD_NAME L"t2wcs: "
#define ERROR_PREFIX CMD_NAME L"error: "
#define FATAL_ERROR_PREFIX CMD_NAME L"fatal error: "

int t2wcs(int argc, char** argv)
{
    wchar_t ch;
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
    while ((ch = fgetwc(ifp)) != static_cast<wchar_t>(WEOF))
    {
        switch (ch)
        {
        case L'\n':
            fwprintf(ofp, L"\\n\\\n");
            break;
        case L'\\':
        case L'"':
            fputwc(L'\\', ofp);
            fputwc(ch, ofp);
            break;
        default:
            fputwc(ch, ofp);
            break;
        }
    }
    fclose(ofp);
    fclose(ifp);
    return 0;
}
