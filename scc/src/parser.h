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

        static const int CACHE_MAX = 8;

        Word buffer[CACHE_MAX];

        int h, size;

        FILE *lexFp, *parserFp;

        static bool EXPRESSION_SELECT[static_cast<unsigned>(WordType::END)];

        static bool STATEMENT_SELECT[static_cast<unsigned>(WordType::END)];

        static bool hasInited;

        static void init();

        void nextWord(bool accept = true);

        void rollback(unsigned n);

        void print(const wchar_t* name);

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

        void str();

        void constBlock();

        void constDef();

        void uinteger();

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

        void step();

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
