#include <cassert>

#include "lexer.h"
#include "parser.h"
#include "exception.h"

namespace scc
{
    // class Parser

    Parser::Parser() : lexer(nullptr)
    {
    }

    Parser::~Parser()
    {
        // TODO
    }

    void Parser::setLexer(Lexer* lexer)
    {
        if (lexer == nullptr)
        {
            throw NullPointerError(L"setLexer: lexer is null");
        }
        this->lexer = lexer;
    }

    // class RecursiveParser

    void RecursiveParser::parse()
    {
        assert(lexer != nullptr);

        // TODO
    }
}
