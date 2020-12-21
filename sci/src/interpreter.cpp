#include <cstdio>
#include <cstring>

#include "../../common/src/bpcode.h"
#include "../../common/src/exception.h"
#include "interpreter.h"

namespace sci
{
    // class Interpreter

    Interpreter::Interpreter() : ip(-1), top(-1), sp(0)
    {
    }

    // class BInterpreter

    BInterpreter::BInterpreter() : codes(nullptr), del(false)
    {
    }

    void BInterpreter::set(BPcode* codes)
    {
        this->codes = codes;
        del = false;
    }

    void BInterpreter::read(const char* fileName)
    {
        FILE* fp;
        if (strcmp(fileName, "-") == 0)
        {
            fp = stdin;
        }
        else
        {
            fp = fopen(fileName, "r");
        }
        if (fp == nullptr)
        {
            throw FileError(fileName, "input");
        }
        del = true;
        // TODO
    }

    void BInterpreter::run()
    {
        // TODO
    }

    BInterpreter::~BInterpreter()
    {
        if (del)
        {
            delete[] codes;
        }
    }

} // namespace sci
