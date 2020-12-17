#include <cstdio>
#include <cwchar>
#include <cassert>

#include "lexer.h"
#include "parser.h"
#include "exception.h"

namespace scc
{
    // class Parser

    bool Parser::EXPRESSION_SELECT[unsigned(WordType::END)] = {false};
    bool Parser::STATEMENT_SELECT[unsigned(WordType::END)] = {false};

    bool Parser::hasInited = false;

    void Parser::init()
    {
        EXPRESSION_SELECT[unsigned(WordType::PLUS)] = true;
        EXPRESSION_SELECT[unsigned(WordType::MINU)] = true;
        EXPRESSION_SELECT[unsigned(WordType::IDENFR)] = true;
        EXPRESSION_SELECT[unsigned(WordType::LPARENT)] = true;
        EXPRESSION_SELECT[unsigned(WordType::INTCON)] = true;
        EXPRESSION_SELECT[unsigned(WordType::CHARCON)] = true;

        STATEMENT_SELECT[unsigned(WordType::IFTK)] = true;
        STATEMENT_SELECT[unsigned(WordType::WHILETK)] = true;
        STATEMENT_SELECT[unsigned(WordType::DOTK)] = true;
        STATEMENT_SELECT[unsigned(WordType::FORTK)] = true;
        STATEMENT_SELECT[unsigned(WordType::LBRACE)] = true;
        STATEMENT_SELECT[unsigned(WordType::IDENFR)] = true;
        STATEMENT_SELECT[unsigned(WordType::SCANFTK)] = true;
        STATEMENT_SELECT[unsigned(WordType::PRINTFTK)] = true;
        STATEMENT_SELECT[unsigned(WordType::SEMICN)] = true;
        STATEMENT_SELECT[unsigned(WordType::RETURNTK)] = true;
    }

    Parser::Parser() : lexer(nullptr), h(0), size(0), lexFp(nullptr), parserFp(nullptr)
    {
        if (!hasInited)
        {
            hasInited = true;
            init();
        }
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
            else if (strcmp(parserFileName, "@") == 0)
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

    void Parser::nextWord(bool accept)
    {
        if (accept && lexFp != nullptr)
        {
            fwprintf(lexFp, L"%ls %ls\n", scc::typeName[unsigned(buffer[h].type)], buffer[h].val.c_str());
        }
        ++h %= CACHE_MAX;
        if (size > 0)
        {
            --size;
        }
        else
        {
            buffer[h].type = WordType::NONE;
            buffer[h].val.clear();
            lexer->nextWord(buffer[h]);
        }
    }

    void Parser::rollback(unsigned n)
    {
        h = (h - n + CACHE_MAX) % CACHE_MAX;
        size += n;
    }

    void Parser::print(const wchar_t* name)
    {
        if (parserFp != nullptr)
        {
            fputws(name, parserFp);
        }
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
