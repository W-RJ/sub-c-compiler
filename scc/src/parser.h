#pragma once

#ifndef _SCC_PARSER_H_
#define _SCC_PARSER_H_

#include <cstdio>
#include <string>
#include <vector>
#include <utility>

#include "lexer.h"
#include "trie"
#include "../../common/src/pcode.h"

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

    struct ExCode
    {
        sci::BPcode code;
        int id;

        explicit ExCode(unsigned f);

        ExCode(unsigned f, int a);
    };

    class Parser
    {
    protected:

        Lexer* lexer;

        static const int CACHE_MAX = 8;

        Word buffer[CACHE_MAX];

        int h, size;

        FILE *lexFp;

        FILE *parserFp;

        int ip;

        std::vector<ExCode> codes;

        bool global;

        Trie<Var, '0', 'z'> globalTrie;

        int globalSize;

        int strSize;

        Trie<Var, '0', 'z'> localTrie;

        Trie<Fun, '0', 'z'> funTrie;

        Trie<int> strTrie;

        std::vector<std::pair<std::string, int> > strVector;

        static bool EXPRESSION_SELECT[static_cast<unsigned>(WordType::END)];

        static bool STATEMENT_SELECT[static_cast<unsigned>(WordType::END)];

        static bool hasInited;

        static void init();

        void nextWord(bool accept = true);

        void rollback(unsigned n);

        Word& preWord(unsigned n);

        void print(const char* name);

    public:

        Parser();

        virtual ~Parser();

        void setLexer(Lexer* lexer);

        void open(const char* lexFileName, const char* parserFileName);

        void close();

        virtual void parse() = 0;

        void write(const char* fileName);

        void writeText(const char* fileName);
    };

    class RecursiveParser : public Parser
    {
    protected:

        void str();

        void constBlock();

        void constDef();

        int uinteger();

        void integer();

        Fun* declareHead();

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
