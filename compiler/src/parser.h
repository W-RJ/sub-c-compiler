/*
    Parser & Optimizer & PCODE generator of SCC.
    Copyright (C) 2020-2021 Renjian Wang

    This file is part of SCC.

    SCC is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SCC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SCC.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef _SCC_PARSER_H_
#define _SCC_PARSER_H_

#include "lexer.h"
#include "trie"

#include "../../common/src/pcode.h"

#include <cstdio>
#include <cstdarg>

#include <string>
#include <vector>
#include <list>
#include <utility>

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

        std::vector<std::list<int> > preAssign;

        std::vector<bool> isPreAssign;

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
        short remain;
        bool fork;

        int bg;

        std::vector<int> dependentCodes;

        int dependentVar;

        explicit ExCode(unsigned f);

        ExCode(unsigned f, int a);

        ExCode(unsigned f, int a, int depCode);

        ExCode(unsigned f, int a, int depCode1, int depCode2);
    };

    class Parser
    {
    protected:

        Lexer* lexer;

        static const int CACHE_MAX = 8;

        Token buffer[CACHE_MAX];

        int h, size;

        FILE *lexFp;

        FILE *parserFp;

        FILE *errorFp;

        int ip;

        std::vector<ExCode> codes;

        int loopCode;

        int loopLevel;

        bool optimize;

        bool hasError;

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

        static bool EXPRESSION_SELECT[static_cast<unsigned>(TokenType::END)];

        static bool STATEMENT_SELECT[static_cast<unsigned>(TokenType::END)];

        static bool hasInited;

        static void init();

        void nextToken(bool accept = true);

        void rollback(unsigned n);

        Token& preToken(unsigned n = 1);

        void print(const char* name);

        void printWarning(int row, char type, const char* format, ...);

        void printErr(int row, char type, const char* format, ...);

        void findVar(Var*& var);

        void verifyVar(Var*& var, const Token& token);

        void verifyWritableVar(Var*& var, const Token& token);

        void verifyElement(Var*& var, const Token& token);

        int loadVar(Var* var);

        void loadElement(Var* var, int depCode);

        void storeVar(Var* var, VarType type, int depCode);

        void storeElement(Var* var, VarType typer, int depCode1, int depCode2);

        void allocAddr(int codesH);

    public:

        explicit Parser(bool optimize);

        virtual ~Parser();

        void setLexer(Lexer* lexer);

        void open(const char* lexFileName, const char* parserFileName, const char* errorFileName);

        void close();

        virtual bool parse() = 0;

        bool hasErr();

        void writeBin(const char* fileName);

        void writeText(const char* fileName);
    };

    class RecursiveParser : public Parser
    {
    protected:

        static const int RET_NONE = 0;

        static const int RET_PARTIAL = 1;

        static const int RET_ALL = 3;

        void beginLoop();

        void endLoop(bool isLoop = true);

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

        int compoundSt();

        void param();

        void mainFun();

        VarType expression(int& lastCode);

        VarType item(int& lastCode);

        VarType factor(int& lastCode);

        int statement();

        void assignSt();

        int conditionSt();

        int condition(bool inv = false);

        int loopSt();

        int step();

        VarType funCall(bool remain = true);

        void voidFunCall();

        void paramVal(const Fun& fun);

        int statementBlock();

        void readSt();

        void writeSt();

        void returnSt();

    public:

        using Parser::Parser;

        virtual bool parse() override;

    };
}

#endif // _SCC_PARSER_H_
