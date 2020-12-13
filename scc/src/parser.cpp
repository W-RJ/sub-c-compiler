#include <cassert>

#include "lexer.h"
#include "parser.h"
#include "exception.h"

namespace scc
{
    // class Parser

    Parser::Parser() : lexer(nullptr), lexFp(nullptr), parserFp(nullptr)
    {
    }

    Parser::~Parser()
    {
        if (lexFp != nullptr)
        {
            fclose(lexFp);
        }
        if (parserFp != nullptr && parserFp != lexFp)
        {
            fclose(parserFp);
        }
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


    void Parser::open(const char* lexFileName, const char* parserFileName)
    {
        if (lexFileName != nullptr)
        {
            if (strcmp(lexFileName, "-") == 0)
            {
                lexFp = stdout;
            }
            else
            {
                lexFp = fopen(lexFileName, "w");
            }
            if (lexFp == nullptr)
            {
                throw FileError(lexFileName, L"lexical analysis result");
            }
        }

        if (parserFileName != nullptr)
        {
            if (strcmp(parserFileName, "-") == 0)
            {
                parserFp = stdout;
            }
            else if (strcmp(parserFileName, "&") == 0)
            {
                parserFp = lexFp;
            }
            else
            {
                parserFp = fopen(parserFileName, "w");
            }
            if (parserFp == nullptr)
            {
                throw FileError(parserFileName, L"parsing result");
            }
        }
    }

    void Parser::close()
    {
        if (lexFp != nullptr)
        {
            fclose(lexFp);
        }
        if (parserFp != nullptr && parserFp != lexFp)
        {
            fclose(parserFp);
        }
        lexFp = nullptr;
        parserFp = nullptr;
    }

    // class RecursiveParser

    void RecursiveParser::str()
    {
    }

    void RecursiveParser::constBlock()
    {
    }

    void RecursiveParser::constDef()
    {
    }

    void RecursiveParser::uinteger()
    {
    }

    void RecursiveParser::integer()
    {
    }

    void RecursiveParser::declareHead()
    {
    }

    void RecursiveParser::varBlock()
    {
    }

    void RecursiveParser::varDef()
    {
    }

    void RecursiveParser::funDef()
    {
    }

    void RecursiveParser::voidFunDef()
    {
    }

    void RecursiveParser::compoundSt()
    {
    }

    void RecursiveParser::param()
    {
    }

    void RecursiveParser::mainFun()
    {
    }

    void RecursiveParser::expression()
    {
    }

    void RecursiveParser::item()
    {
    }

    void RecursiveParser::factor()
    {
    }

    void RecursiveParser::statement()
    {
    }

    void RecursiveParser::assignSt()
    {
    }

    void RecursiveParser::conditionSt()
    {
    }

    void RecursiveParser::condition()
    {
    }

    void RecursiveParser::loopSt()
    {
    }

    void RecursiveParser::step()
    {
    }

    void RecursiveParser::funCall()
    {
    }

    void RecursiveParser::voidFunCall()
    {
    }

    void RecursiveParser::paramVal()
    {
    }

    void RecursiveParser::statementBlock()
    {
    }

    void RecursiveParser::readSt()
    {
    }

    void RecursiveParser::writeSt()
    {
    }

    void RecursiveParser::returnSt()
    {
    }

    void RecursiveParser::parse()
    {
        assert(lexer != nullptr);

        // TODO
    }
}
