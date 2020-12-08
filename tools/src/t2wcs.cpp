#include <cstdio>
#include <cstdlib>
#include <cwchar>

#define ERROR_PREFIX L"t2wcs: fatal error: "

int t2wcs(int argc, char** argv)
{
    wchar_t ch;
    FILE *ifp = stdin, *ofp = stdout;
    if (argc == 3)
    {
        ifp = fopen(argv[1], "r");
        ofp = fopen(argv[2], "w");
    }
    else if (argc != 1)
    {
        fwprintf(stderr, ERROR_PREFIX L"invalid arguments: 0 or 2 arguments are required\n");
        exit(1);
    }
    if (ifp == nullptr)
    {
        fwprintf(stderr, ERROR_PREFIX L"%s: cannot open input file\n", argv[1]);
        exit(1);
    }
    if (ofp == nullptr)
    {
        fwprintf(stderr, ERROR_PREFIX L"%s: cannot open output file\n", argv[2]);
        exit(1);
    }
    while ((ch = fgetwc(ifp)) != wchar_t(EOF))
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
