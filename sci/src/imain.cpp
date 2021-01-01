/*
    Entrance of SC Interpreter.
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

#include "../../common/src/exception.h"
#include "interpreter.h"

void runBin(const char* fileName)
{
    sci::BInterpreter interpreter;

    interpreter.read(fileName);

    interpreter.run();
}

void runText(const char* fileName)
{
    sci::TInterpreter interpreter;

    interpreter.read(fileName);

    interpreter.run();
}

int imain(int argc, char** argv)
{
    RuntimeError::setCmdName(argv[0]);

    bool binary = true;

    char* fileName = nullptr;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-' || argv[i][1] == '\0')
        {
            fileName = argv[i]; // TODO
        }
        else if (argv[i][1] == 't')
        {
            binary = false;
        }
        else if (argv[i][1] == 'b')
        {
            binary = true;
        }
    }

    if (fileName == nullptr)
    {
        InvalidArgumentError("no input file", nullptr).print(stderr);
    }
    else
    {
        try
        {
            if (binary)
            {
                runBin(fileName); // TODO
            }
            else
            {
                runText(fileName);
            }
        }
        catch (const FileError& e)
        {
            e.print(stderr);
            return 1;
        }
        catch (const InvalidFormatError& e)
        {
            e.print(stderr);
            return 1;
        }
        catch (const InstructionError& e)
        {
            e.print(stderr);
            return 1;
        }
    }

    return 0;
}
