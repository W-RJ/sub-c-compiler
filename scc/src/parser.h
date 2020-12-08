#pragma once

#ifndef _SCC_PARSER_H_
#define _SCC_PARSER_H_

#include "lexer.h"

namespace scc
{
    class Parser
    {
    private:

        Lexer* lexer;

    public:

        Parser();

        virtual ~Parser();

        void setLexer(Lexer* lexer);

        virtual void parse() = 0;
    };
}

#endif // _SCC_PARSER_H_
