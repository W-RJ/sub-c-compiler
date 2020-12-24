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

        VarType type;
        bool global;
        bool writable;
        int addr;
        int size;

        explicit Var(VarType type);

        Var(VarType type, bool global, int addr, bool writable);

        Var(VarType type, bool global, int addr, int size);
    };

    struct Fun
    {
        int addr;
        VarType returnType;
        std::vector<VarType> paramTypes;

        Fun(VarType returnType, int addr);
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

        FILE *errorFp;

        int ip;

        std::vector<ExCode> codes;

        bool optimize;

        bool global;

        int globalSize;

        int strSize;

        Trie<int, '0', 'z'> globalTrie;

        Trie<int, '0', 'z'> localTrie;

        Trie<int, '0', 'z'> funTrie;

        Trie<int> strTrie;

        std::vector<Var> globalVector;

        std::vector<Var> localVector;

        std::vector<Fun> funVector;

        std::vector<std::pair<std::string, int> > strVector;

        static bool EXPRESSION_SELECT[static_cast<unsigned>(WordType::END)];

        static bool STATEMENT_SELECT[static_cast<unsigned>(WordType::END)];

        static bool hasInited;

        static void init();

        void nextWord(bool accept = true);

        void rollback(unsigned n);

        Word& preWord(unsigned n);

        void print(const char* name);

        void findVar(Var*& var);

        void loadVar(Var* var);

        void loadElement(Var* var);

        void storeVar(Var* var);

        void storeElement(Var* var);

        void allocAddr(int codesH);

    public:

        explicit Parser(bool optimize);

        virtual ~Parser();

        void setLexer(Lexer* lexer);

        void open(const char* lexFileName, const char* parserFileName, const char* errorFileName);

        void close();

        virtual void parse() = 0;

        void write(const char* fileName);

        void writeText(const char* fileName);
    };

    class RecursiveParser : public Parser
    {
    protected:

        int str();

        void constBlock();

        void constDef();

        int uinteger();

        int integer();

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

        void condition(bool inv = false);

        void loopSt();

        int step();

        void funCall(bool remain = true);

        void voidFunCall();

        void paramVal(const Fun& fun);

        void statementBlock();

        void readSt();

        void writeSt();

        void returnSt();

    public:

        using Parser::Parser;

        virtual void parse() override;

    };
}

#endif // _SCC_PARSER_H_
