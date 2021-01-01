/*
    Tool for converting text to C style string.
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
    while ((ch = fgetc(ifp)) != static_cast<char>(EOF) && ch != '\t')
    {
        switch (ch)
        {
        case '\n':
            fprintf(ofp, "\\n\\\n");
            break;
        case '\\':
        case '"':
            fputc('\\', ofp);
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
