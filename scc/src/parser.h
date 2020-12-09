#pragma once

#ifndef _SCC_PARSER_H_
#define _SCC_PARSER_H_

#include <cstdio>

#include "lexer.h"

namespace scc
{
    class Parser
    {
    protected:

        Lexer* lexer;

        Word word;

        FILE *lexFp, *parserFp;

    public:

        Parser();

        virtual ~Parser();

        void setLexer(Lexer* lexer);

        void open(const char* lexFileName, const char* parserFileName);

        void close();

        virtual void parse() = 0;
    };

    class RecursiveParser : public Parser
    {
    public:
        virtual void parse() override;
    };
}

#endif // _SCC_PARSER_H_
