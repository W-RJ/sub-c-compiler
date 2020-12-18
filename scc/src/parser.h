#pragma once

#ifndef _SCC_PARSER_H_
#define _SCC_PARSER_H_

#include <cstdio>

#include "lexer.h"
#include "trie"

namespace scc
{
    enum class VarType
    {
        NONE,
        VOID,
        CHAR,
        INT,
    };

    struct Var
    {
        static const int SINGLE = -1;

        bool writable = true;
        int size = SINGLE;
        VarType type = VarType::NONE;
    };

    struct Fun
    {
        VarType returnType = VarType::NONE;
        std::vector<VarType> paramTypes;
    };

    class Parser
    {
    protected:

        Lexer* lexer;

        static const int CACHE_MAX = 8;

        Word buffer[CACHE_MAX];

        int h, size;

        FILE *lexFp, *parserFp;

        bool global;

        Trie<Var, L'0', L'z'> globalTrie;

        Trie<Var, L'0', L'z'> localTrie;

        Trie<Fun, L'0', L'z'> funTrie;

        static bool EXPRESSION_SELECT[static_cast<unsigned>(WordType::END)];

        static bool STATEMENT_SELECT[static_cast<unsigned>(WordType::END)];

        static bool hasInited;

        static void init();

        void nextWord(bool accept = true);

        void rollback(unsigned n);

        Word& preWord(unsigned n);

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
