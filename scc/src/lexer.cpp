#include <cstdio>

#include "lexer.h"

namespace scc
{
    // class Lexer
    Lexer::Lexer() : fp(nullptr)
    {
    }

    Lexer::~Lexer()
    {
        if (fp != nullptr)
        {
            fclose(fp);
        }
    }

    bool Lexer::open(const char *fileName)
    {
        fp = fopen(fileName, "r, ccs=utf-8");
        return fp != nullptr;
    }

    void Lexer::close()
    {
        if (fp != nullptr)
        {
            fclose(fp);
            fp = nullptr;
        }
    }
}
