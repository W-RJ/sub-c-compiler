/*
    Tool for converting token to source code.
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

#define CMD_NAME "token2input: "
#define ERROR_PREFIX CMD_NAME "error: "
#define FATAL_ERROR_PREFIX CMD_NAME "fatal error: "

int token2input(int argc, char** argv)
{
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
    char typeName[256], val[4096];
    int n;
    bool newline = true, space = false;
    int level = 0;
    while (fscanf(ifp, "%255s ", typeName) >= 1)
    {
        fgets(val, 4096, ifp);
        n = strlen(val);
        if (val[n - 1] == '\n')
        {
            val[n - 1] = '\0';
        }
        if (strcmp(typeName, "CHARCON") == 0)
        {
            if (newline)
            {
                for (int i = 0; i < level; i++)
                {
                    fprintf(ofp, "    ");
                }
            }
            fprintf(ofp, "'%s'", val);
            space = true;
            newline = false;
        }
        else if (strcmp(typeName, "STRCON") == 0)
        {
            if (newline)
            {
                for (int i = 0; i < level; i++)
                {
                    fprintf(ofp, "    ");
                }
            }
            fprintf(ofp, "\"%s\"", val);
            space = true;
            newline = false;
        }
        else if (strcmp(typeName, "LBRACE") == 0)
        {
            if (newline)
            {
                for (int i = 0; i < level; i++)
                {
                    fprintf(ofp, "    ");
                }
            }
            fprintf(ofp, "%s\n", val);
            space = false;
            newline = true;
            level++;
        }
        else if (strcmp(typeName, "RBRACE") == 0)
        {
            if (newline)
            {
                for (int i = 0; i < level - 1; i++)
                {
                    fprintf(ofp, "    ");
                }
            }
            if (!newline)
            {
                fprintf(ofp, "\n");
            }
            fprintf(ofp, "%s\n", val);
            space = false;
            newline = true;
            level--;
        }
        else if (strcmp(typeName, "SEMICN") == 0)
        {
            if (newline)
            {
                for (int i = 0; i < level; i++)
                {
                    fprintf(ofp, "    ");
                }
            }
            fprintf(ofp, "%s\n", val);
            space = false;
            newline = true;
        }
        else if (strcmp(typeName, "LPARENT") == 0 || strcmp(typeName, "RPARENT") == 0)
        {
            if (newline)
            {
                for (int i = 0; i < level; i++)
                {
                    fprintf(ofp, "    ");
                }
            }
            fprintf(ofp, "%s", val);
            space = false;
            newline = false;
        }
        else if (strcmp(typeName, "COMMA") == 0)
        {
            if (newline)
            {
                for (int i = 0; i < level; i++)
                {
                    fprintf(ofp, "    ");
                }
            }
            fprintf(ofp, "%s", val);
            space = true;
            newline = false;
        }
        else
        {
            if (newline)
            {
                for (int i = 0; i < level; i++)
                {
                    fprintf(ofp, "    ");
                }
            }
            if (space)
            {
                fprintf(ofp, " ");
            }
            fprintf(ofp, "%s", val);
            space = true;
            newline = false;
        }
        
    }
    fclose(ofp);
    fclose(ifp);
    return 0;
}
