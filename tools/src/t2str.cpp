#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#define CMD_NAME "t2str: "
#define ERROR_PREFIX CMD_NAME "error: "
#define FATAL_ERROR_PREFIX CMD_NAME "fatal error: "

int t2str(int argc, char** argv)
{
    char ch;
    FILE *ifp = stdin, *ofp = stdout;
    if (argc == 3)
    {
        ifp = fopen(argv[1], "r");
        if (ifp == nullptr)
        {
            fprintf(stderr, ERROR_PREFIX "%s: %s\n", argv[1], strerror(errno));
            fprintf(stderr, FATAL_ERROR_PREFIX "Unable to open input file\n");
            exit(1);
        }
        ofp = fopen(argv[2], "w");
        if (ofp == nullptr)
        {
            fprintf(stderr, ERROR_PREFIX "%s: %s\n", argv[2], strerror(errno));
            fprintf(stderr, FATAL_ERROR_PREFIX "Unable to open output file\n");
            exit(1);
        }
    }
    else if (argc != 1)
    {
        fprintf(stderr, FATAL_ERROR_PREFIX "invalid arguments: 0 or 2 arguments are required\n");
        exit(1);
    }
    while ((ch = fgetc(ifp)) != static_cast<char>(EOF))
    {
        switch (ch)
        {
        case L'\n':
            fprintf(ofp, "\\n\\\n");
            break;
        case L'\\':
        case L'"':
            fputc(L'\\', ofp);
            fputc(ch, ofp);
            break;
        default:
            fputc(ch, ofp);
            break;
        }
    }
    fclose(ofp);
    fclose(ifp);
    return 0;
}
