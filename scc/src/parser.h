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
    protected:

        void constBlock();

        void constDef();

        void integer();

        void declareHead();

        void varBlock();

        void varDef();

        void funDef();

        void voidFunDef();

        void compoundSt();

        void param();

        void mainFun();

        void expression();

        void item();

        void factor();

        void statement();

        void assignSt();

        void conditionSt();

        void condition();

        void loopSt();

        void funCall();

        void voidFunCall();

        void paramVal();

        void statementBlock();

        void readSt();

        void writeSt();

        void returnSt();

    public:

        virtual void parse() override;

    };
}

#endif // _SCC_PARSER_H_
