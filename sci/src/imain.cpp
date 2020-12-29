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
