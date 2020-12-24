#include "interpreter.h"

void runBin(const char* fileName)
{
    sci::BInterpreter interpreter;

    interpreter.read(fileName);

    interpreter.run();
}

int imain(int argc, char** argv)
{

    return 0;
}
