#pragma once

#ifndef _SCC_PARSER_H_
#define _SCC_PARSER_H_

#include "lexer.h"

namespace scc
{
    class Parser
    {
    protected:

        Lexer* lexer;

        Word word;

    public:

        Parser();

        virtual ~Parser();

        void setLexer(Lexer* lexer);

        virtual void parse() = 0;
    };

    class RecursiveParser : public Parser
    {
    public:
        virtual void parse() override;
    };
}

#endif // _SCC_PARSER_H_
