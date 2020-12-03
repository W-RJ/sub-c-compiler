#include <cstdio>

#include "lexer.h"
#include "exception.h"

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

    void Lexer::open(const char *fileName)
    {
        fp = fopen(fileName, "r, ccs=utf-8");
        if (fp == nullptr)
        {
            throw NoSuchFileError(L"Cannot open input file");
        }
    }

    void Lexer::close()
    {
        if (fp != nullptr)
        {
            fclose(fp);
            fp = nullptr;
        }
    }

    // class DFALexer
    Word DFALexer::nextWord()
    {
        Word word;
        if (fp != nullptr)
        {
            // TODO: DFA
        }
        return word;
    }
}
