#include <cstdio>

#include "../../common/src/exception.h"
#include "interpreter.h"

void runBin(const char* fileName)
{
    sci::BInterpreter interpreter;

    interpreter.read(fileName);

    interpreter.run();
}

int imain(int argc, char** argv)
{
    RuntimeError::setCmdName(argv[0]);

    char* fileName = nullptr;

    for (int i = 1; i < argc; i++)
    {
        fileName = argv[i]; // TODO
    }

    if (fileName == nullptr)
    {
        InvalidArgumentError("no input file", nullptr).print(stderr);
    }
    else
    {
        try
        {
            runBin(fileName); // TODO
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
