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

#include "lexer.h"
#include "parser.h"
#include "trie"
#include "define.h"

#include "../../common/src/pcode.h"
#include "../../common/src/exception.h"

#include <cstdio>
#include <climits>
#include <cstdarg>
#include <cassert>

#include <algorithm>
#include <vector>
#include <string>
#include <queue>

namespace scc
{
    // struct Var

    Var::Var(VarType type) : type(type), global(false), writable(true), size(SINGLE)
    {
        preAssign.emplace_back();
        isPreAssign.push_back(false);
    }

    Var::Var(VarType type, bool global, int addr, bool writable) : type(type), global(global), writable(writable), addr(addr), size(SINGLE)
    {
        preAssign.emplace_back();
        isPreAssign.push_back(false);
    }

    Var::Var(VarType type, bool global, int addr, int size) : type(type), global(global), writable(true), addr(addr), size(size)
    {
        preAssign.emplace_back();
        isPreAssign.push_back(false);
    }

    // struct Fun

    Fun::Fun(VarType returnType, int addr) : addr(addr), returnType(returnType)
    {
    }

    // struct ExCode

    ExCode::ExCode(unsigned f) : code{f}, id(0), remain(0), fork(false), bg(INT_MAX >> 1), dependentVar(0)
    {
    }

    ExCode::ExCode(unsigned f, int a) : code{f, a}, id(0), remain(0), fork(false), bg(INT_MAX >> 1), dependentVar(0)
    {
    }

    ExCode::ExCode(unsigned f, int a, int depCode) : code{f, a}, id(0), remain(0), fork(false), bg(INT_MAX >> 1), dependentVar(0)
    {
        dependentCodes.push_back(depCode);
    }

    ExCode::ExCode(unsigned f, int a, int depCode1, int depCode2) : code{f, a}, id(0), remain(0), fork(false), bg(INT_MAX >> 1), dependentVar(0)
    {
        dependentCodes.push_back(depCode1);
        dependentCodes.push_back(depCode2);
    }

    // class Parser

    bool Parser::EXPRESSION_SELECT[static_cast<unsigned>(TokenType::END)] = {false};
    bool Parser::STATEMENT_SELECT[static_cast<unsigned>(TokenType::END)] = {false};

    bool Parser::hasInited = false;

    void Parser::init()
    {
        EXPRESSION_SELECT[static_cast<unsigned>(TokenType::PLUS)] = true;
        EXPRESSION_SELECT[static_cast<unsigned>(TokenType::MINU)] = true;
        EXPRESSION_SELECT[static_cast<unsigned>(TokenType::IDENFR)] = true;
        EXPRESSION_SELECT[static_cast<unsigned>(TokenType::LPARENT)] = true;
        EXPRESSION_SELECT[static_cast<unsigned>(TokenType::INTCON)] = true;
        EXPRESSION_SELECT[static_cast<unsigned>(TokenType::CHARCON)] = true;

        STATEMENT_SELECT[static_cast<unsigned>(TokenType::IFTK)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(TokenType::WHILETK)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(TokenType::DOTK)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(TokenType::FORTK)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(TokenType::LBRACE)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(TokenType::IDENFR)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(TokenType::SCANFTK)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(TokenType::PRINTFTK)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(TokenType::SEMICN)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(TokenType::RETURNTK)] = true;
    }

    Parser::Parser(bool optimize) : lexer(nullptr), h(0), size(0), lexFp(nullptr),
            parserFp(nullptr), errorFp(nullptr), ip(0), loopCode(0), loopLevel(0), optimize(optimize),
            hasError(false), global(true), globalSize(0), strSize(0)
    {
        if (!hasInited)
        {
            hasInited = true;
            init();
        }
    }

    Parser::~Parser()
    {
        if (lexFp != nullptr && lexFp != stdout)
        {
            fclose(lexFp);
        }
        if (parserFp != nullptr && parserFp != lexFp && parserFp != stdout)
        {
            fclose(parserFp);
        }
        if (errorFp != nullptr && errorFp != stderr)
        {
            fclose(errorFp);
        }
        // TODO
    }

    void Parser::setLexer(Lexer* lexer)
    {
        if (lexer == nullptr)
        {
            throw NullPointerError("Parser::setLexer: lexer is null");
        }
        this->lexer = lexer;
    }


    void Parser::open(const char* lexFileName, const char* parserFileName, const char* errorFileName)
    {
        if (lexFileName != nullptr)
        {
            if (lexFp != nullptr && lexFp != stdout)
            {
                fclose(lexFp);
            }
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
                throw FileError(lexFileName, "lexical analysis result");
            }
        }

        if (parserFileName != nullptr)
        {
            if (parserFp != nullptr && parserFp != stdout)
            {
                fclose(parserFp);
            }
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
                throw FileError(parserFileName, "parsing result");
            }
        }

        if (errorFileName != nullptr)
        {
            if (errorFp != nullptr && errorFp != stderr)
            {
                fclose(errorFp);
            }
            if (strcmp(errorFileName, "-") == 0)
            {
                errorFp = stderr;
            }
            else
            {
                errorFp = fopen(errorFileName, "w");
            }
            if (errorFp == nullptr)
            {
                throw FileError(errorFileName, "error");
            }
        }
    }

    void Parser::close()
    {
        if (lexFp != nullptr && lexFp != stdout)
        {
            fclose(lexFp);
        }
        if (parserFp != nullptr && parserFp != lexFp && parserFp != stdout)
        {
            fclose(parserFp);
        }
        if (errorFp != nullptr && errorFp != stderr)
        {
            fclose(errorFp);
        }
        lexFp = nullptr;
        parserFp = nullptr;
        errorFp = nullptr;
    }

    bool Parser::hasErr()
    {
        return hasError;
    }

    void Parser::writeBin(const char* fileName)
    {
        assert(fileName != nullptr);

        FILE* fp;
        if (strcmp(fileName, "-") == 0)
        {
            fp = stdout;
        }
        else
        {
            fp = fopen(fileName, "wb");
        }
        if (fp == nullptr)
        {
            throw FileError(fileName, "object");
        }

        sci::BPcodeBlockType type;
        int tmpInt;

        fwrite(&sci::BPCODE_PREFIX, sizeof(sci::BPCODE_PREFIX), 1, fp);

        fwrite(&sci::BPCODE_MIN_VERSION, sizeof(int), 1, fp);

        fwrite(&sci::BPCODE_VERSION, sizeof(int), 1, fp);

        // general
        type = sci::BPcodeBlockType::GENERAL;
        tmpInt = 1;
        fwrite(&type, sizeof(sci::BPcodeBlockType), 1, fp);
        fwrite(&tmpInt, sizeof(int), 1, fp);

        fwrite(&globalSize, sizeof(int), 1, fp);

        // str
        type = sci::BPcodeBlockType::STR;
        fwrite(&type, sizeof(sci::BPcodeBlockType), 1, fp);
        fwrite(&strSize, sizeof(int), 1, fp);

        tmpInt = 0;

        for (const auto& it : strVector)
        {
            fwrite(it.first.c_str(), it.first.size(), 1, fp);
            fwrite(&tmpInt, sizeof(int) - (it.first.size()) % sizeof(int), 1, fp);
        }

        // code
        type = sci::BPcodeBlockType::CODE;
        fwrite(&type, sizeof(sci::BPcodeBlockType), 1, fp);
        fwrite(&ip, sizeof(int), 1, fp);

        for (const auto& it : codes)
        {
            if (it.remain >= static_cast<int>(optimize))
            {
                fwrite(&it.code, sizeof(it.code), 1, fp);
            }
        }

        fclose(fp);
    }

    void Parser::writeText(const char* fileName)
    {
        assert(fileName != nullptr);

        FILE* fp;
        if (strcmp(fileName, "-") == 0)
        {
            fp = stdout;
        }
        else
        {
            fp = fopen(fileName, "w");
        }
        if (fp == nullptr)
        {
            throw FileError(fileName, "object");
        }

        fprintf(fp, "%s\n", sci::TPCODE_DATA);

        fprintf(fp, "%s 0 %d\n", sci::fs[005].name, globalSize);

        for (const auto& it : strVector)
        {
            fprintf(fp, "%s 0 %d %s\n", sci::fs[013].name, static_cast<int>(it.first.size() / sizeof(int)) + 1, it.first.c_str());
        }

        fprintf(fp, "%s\n", sci::TPCODE_CODE);

        for (const auto& it : codes)
        {
            if (it.remain >= static_cast<int>(optimize))
            {
                fprintf(fp, "%s %u %d\n", sci::fs[it.code.f >> 3].name, it.code.f & 07, it.code.a);
            }
        }

        fclose(fp);
    }

    void Parser::nextToken(bool accept)
    {
        if (accept && lexFp != nullptr)
        {
            fprintf(lexFp, "%s %s\n", scc::typeName[static_cast<unsigned>(buffer[h].type)], buffer[h].val.c_str());
        }
        ++h %= CACHE_MAX;
        if (size > 0)
        {
            --size;
        }
        else
        {
            buffer[h].type = TokenType::NONE;
            buffer[h].val.clear();
            lexer->nextToken(buffer[h]);
            if (buffer[h].type == TokenType::INTERR)
            {
                printErr(buffer[h].row, 'a', "invalid integer constant '%s'", buffer[h].val.c_str());
                buffer[h].type = TokenType::INTCON;
            }
            else if (buffer[h].type == TokenType::CHARERR)
            {
                printErr(buffer[h].row, 'a', "invalid character constant '%c'", buffer[h].val[0]);
                buffer[h].type = TokenType::CHARCON;
            }
            else if (buffer[h].type == TokenType::NONE)
            {
                printErr(buffer[h].row, 'a', "invalid token '%c'", buffer[h].val.c_str());
            }
        }
    }

    void Parser::rollback(unsigned n)
    {
        h = (h - n + CACHE_MAX) % CACHE_MAX;
        size += n;
    }

    Token& Parser::preToken(unsigned n)
    {
        return buffer[(h - n + CACHE_MAX) % CACHE_MAX];
    }

    void Parser::print(const char* name)
    {
        if (parserFp != nullptr)
        {
            fputs(name, parserFp);
        }
    }

    void Parser::printWarning(int row, char type, const char* format, ...)
    {
#if defined(CG) && CG == 3
        if (type != '\0')
        {
            fprintf(errorFp, "%d %c\n", row, type);
        }
#else
        va_list args;
        va_start(args, format);
        fprintf(errorFp, "%s:%d: warning: ", lexer->getFileName(), row);
        vfprintf(errorFp, format, args);
        fputs("\n", errorFp);
        va_end(args);
#endif
    }

    void Parser::printErr(int row, char type, const char* format, ...)
    {
        hasError = true;
#if defined(CG) && CG == 3
        if (type != '\0')
        {
            fprintf(errorFp, "%d %c\n", row, type);
        }
#else
        va_list args;
        va_start(args, format);
        fprintf(errorFp, "%s:%d: error: ", lexer->getFileName(), row);
        vfprintf(errorFp, format, args);
        fputs("\n", errorFp);
        va_end(args);
#endif
    }

    void Parser::findVar(Var*& var)
    {
        int id = localTrie.find(buffer[h].val.c_str());
        if (id != 0)
        {
            var = localVector.data() + id - 1;
        }
        else
        {
            id = globalTrie.find(buffer[h].val.c_str());
            if (id != 0)
            {
                var = globalVector.data() + id - 1;
            }
            else
            {
                printErr(buffer[h].row, 'c', "'%s' was not declared in this scope", buffer[h].val.c_str());
                // TODO: ERROR
                var = nullptr;
            }
        }
    }

    void Parser::verifyVar(Var*& var, const Token& token)
    {
        if (var != nullptr)
        {
            if (var->size != Var::SINGLE)
            {
                // TODO: ERROR
                var = nullptr;
            }
        }
    }

    void Parser::verifyWritableVar(Var*& var, const Token& token)
    {
        if (var != nullptr)
        {
            if (!var->writable)
            {
                printErr(token.row, 'j', "assignment of read-only variable '%s'", token.val.c_str());
                // TODO: ERROR
                var = nullptr;
            }
            else if (var->size != Var::SINGLE)
            {
                // TODO: ERROR
                var = nullptr;
            }
        }
    }

    void Parser::verifyElement(Var*& var, const Token& token)
    {
        if (var != nullptr)
        {
            if (var->size == Var::SINGLE)
            {
                // TODO: ERROR
                var = nullptr;
            }
        }
    }

    int Parser::loadVar(Var* var)
    {
        int res = 0;

        if (var != nullptr)
        {
            if (var->writable)
            {
                ExCode& preCode = codes.back();
                if (var->global)
                {
                    if (preCode.code.f == 0031 && preCode.code.a == var->addr && preCode.remain >= 0 && !preCode.fork && optimize)
                    {
                        preCode.code.f |= 2u;
                    }
                    else
                    {
                        codes.emplace_back(0021, var->addr);
                    }
                }
                else
                {
                    if (preCode.code.f == 0030 && preCode.code.a == var->addr && preCode.remain >= 0 && !preCode.fork && optimize)
                    {
                        preCode.code.f |= 2u;
                        res = codes.size() - 2;
                        while (codes[res].code.f == 0032 && codes[res].remain >= 0)
                        {
                            res--;
                        }
                    }
                    else
                    {
                        res = codes.size();
                        codes.emplace_back(0020, var->addr);
                        // TODO
                        int bg = loopLevel; // TODO: FIXME
                        while (bg >= 0 && !var->isPreAssign[bg])
                        {
                            --bg;
                        }
                        if (bg < 0)
                        {
                            bg = 0;
                        }
                        codes.back().bg = bg; // TODO: FIXME
                        while (bg <= loopLevel)
                        {
                            codes.back().dependentCodes.insert(codes.back().dependentCodes.end(), var->preAssign[bg].begin(), var->preAssign[bg].end());
                            bg++;
                        }
                        codes.back().dependentVar = var - localVector.data();
                    }
                }
            }
            else
            {
                res = codes.size();
                codes.emplace_back(0010, var->addr);
            }
        }

        return res;
    }

    void Parser::loadElement(Var* var, int depCode)
    {
        if (var != nullptr)
        {
            if (var->global)
            {
                codes.emplace_back(0111, var->addr, depCode);
            }
            else
            {
                codes.emplace_back(0110, var->addr, depCode);
            }
        }
    }

    void Parser::storeVar(Var* var, VarType type, int depCode)
    {
        if (var != nullptr)
        {
            if (var->type != type)
            {
                // TODO: ERROR
            }
            if (var->global)
            {
                codes.emplace_back(0031, var->addr, depCode);
            }
            else
            {
                var->preAssign.resize(loopLevel);
                var->preAssign.emplace_back();
                var->preAssign.back().push_back(codes.size());
                var->isPreAssign.back() = true;
                codes.emplace_back(0030, var->addr, depCode);
            }
        }
    }

    void Parser::storeElement(Var* var, VarType type, int depCode1, int depCode2)
    {
        if (var != nullptr)
        {
            if (var->type != type)
            {
                // TODO: ERROR
            }
            if (var->global)
            {
                codes.emplace_back(0121, var->addr, depCode1, depCode2);
            }
            else
            {
                codes.emplace_back(0120, var->addr, depCode1, depCode2);
            }
        }
    }

    void Parser::allocAddr(int codesH)
    {
        Fun& fun = funVector.back();
        int n = codes.size();
        int cur;
        std::queue<int> q;
        bool* vis = new bool[n - codesH];
        memset(vis, false, (n - codesH) * sizeof(bool));
        for (int i = n - 1; i >= codesH; --i)
        {
            if (codes[i].remain == 1
                || (codes[i].code.f != 0010 && codes[i].code.f != 0020 && codes[i].code.f != 0030
                    && codes[i].code.f != 0032 && codes[i].code.f != 0100)
                || (codes[i].code.f == 0100 && (codes[i].code.a < 2 || codes[i].code.a > 5)))
            {
                codes[i].remain = 1;
                vis[i - codesH] = true;
                q.push(i);
            }
        }
        while (!q.empty())
        {
            cur = q.front();
            q.pop();

            for (auto it : codes[cur].dependentCodes)
            {
                if (it != 0 && !vis[it - codesH])
                {
                    codes[it].remain = 1;
                    vis[it - codesH] = true;
                    q.push(it);
                }
            }
        }
        delete[] vis;
        int addr = 2;
        n = localVector.size();
        for (int i = fun.paramTypes.size(); i < n; i++)
        {
            if (localVector[i].writable)
            {
                if (localVector[i].size == Var::SINGLE)
                {
                    localVector[i].addr = addr++;
                }
                else
                {
                    localVector[i].addr = addr;
                    addr += localVector[i].size;
                }
            }
        }
        if (addr > 2)
        {
            codes[codesH].code.a = addr - 2;
        }
        else if (codes[codesH].code.f == 0050)
        {
            codes[codesH].remain = -1;
        }

        n = codes.size();
        for (int i = codesH; i < n; i++)
        {
            codes[i].id = ip;
            if (codes[i].remain >= static_cast<int>(optimize))
            {
                ip++;
            }
            // TODO
        }

        for (int i = codesH; i < n; i++)
        {
            if (codes[i].remain >= static_cast<int>(optimize))
            {
                switch (codes[i].code.f)
                {
                case 0020:
                case 0030:
                case 0032:
                case 0110:
                case 0120:
                    if (codes[i].code.a >= 0)
                    {
                        codes[i].code.a = localVector[codes[i].code.a].addr;
                    }
                    break;

                case 0060:
                case 0070:
                    codes[i].code.a = codes[codes[i].code.a].id;
                    break;
                }
            }
        }
    }

    // class RecursiveParser

    void RecursiveParser::beginLoop()
    {
        for (auto& it : localVector)
        {
            it.preAssign.resize(loopLevel + 1);
            it.isPreAssign.push_back(false);
        }
    }

    void RecursiveParser::endLoop(bool isLoop)
    {
        if (isLoop)
        {
            int n = codes.size();
            for (int i = loopCode; i < n; ++i)
            {
                if (codes[i].bg < loopLevel)
                {
                    for (auto it : localVector[codes[i].dependentVar].preAssign[loopLevel])
                    {
                        codes[i].dependentCodes.push_back(it);
                    }
                }
            }
        }
        for (auto& it : localVector)
        {
            it.preAssign[loopLevel - 1].splice(it.preAssign[loopLevel - 1].begin(), std::move(it.preAssign[loopLevel]));
            it.preAssign.resize(loopLevel);
            it.isPreAssign.resize(loopLevel);
        }
    }

    int RecursiveParser::str()
    {
        assert(buffer[h].type == TokenType::STRCON);

        int& id = strTrie.at(buffer[h].val.c_str());
        if (id == 0 || !optimize)
        {
            strVector.emplace_back(buffer[h].val, globalSize + strSize);
            id = strVector.size();
            strSize += static_cast<int>(buffer[h].val.size() / sizeof(int)) + 1;
        }

        nextToken();

        print("<字符串>\n");

        return strVector[id - 1].second;
    }

    void RecursiveParser::constBlock()
    {
        if (buffer[h].type != TokenType::CONSTTK)
        {
            // TODO: ERROR
        }
        do
        {
            nextToken();
            constDef();
            if (buffer[h].type != TokenType::SEMICN)
            {
                printErr(preToken().row, 'k', "expect ';' after '%s'", preToken().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextToken();
            }

        } while (buffer[h].type == TokenType::CONSTTK);

        print("<常量说明>\n");
    }

    void RecursiveParser::constDef()
    {
        if (buffer[h].type == TokenType::INTTK)
        {
            do
            {
                nextToken();
                if (buffer[h].type != TokenType::IDENFR)
                {
                    // TODO: ERROR
                }
                int idH = h;
                nextToken();
                if (buffer[h].type != TokenType::ASSIGN)
                {
                    // TODO: ERROR
                }
                nextToken();

                int data;
                try
                {
                    data = integer();
                }
                catch (const ParsingError& e)
                {
                    printErr(buffer[idH].row, 'o', "except numeric constant, not '%s'", buffer[h].val.c_str());
                }

                if (global)
                {
                    if (funTrie.find(buffer[idH].val.c_str()) != 0)
                    {
                        printErr(buffer[idH].row, 'b', "redeclaration of '%s'", buffer[idH].val.c_str());
                        // TODO: ERROR
                        continue;
                    }
                    else
                    {
                        int& id = globalTrie.at(buffer[idH].val.c_str()); // TODO
                        if (id != 0) // TODO
                        {
                            printErr(buffer[idH].row, 'b', "redeclaration of '%s'", buffer[idH].val.c_str());
                            // TODO: ERROR
                            continue;
                        }
                        else
                        {
                            globalVector.emplace_back(VarType::INT, global, data, false);
                            id = globalVector.size();

                            // TODO
                        }
                    }
                }
                else
                {
                    int& id = localTrie.at(buffer[idH].val.c_str()); // TODO
                    if (id != 0)
                    {
                        printErr(buffer[idH].row, 'b', "redeclaration of '%s'", buffer[idH].val.c_str());
                        // TODO: ERROR
                        continue;
                    }
                    else
                    {
                        localVector.emplace_back(VarType::INT, global, data, false); // TODO
                        id = localVector.size();
                    }
                }
            } while (buffer[h].type == TokenType::COMMA);
        }
        else if (buffer[h].type == TokenType::CHARTK)
        {
            do
            {
                nextToken();
                if (buffer[h].type != TokenType::IDENFR)
                {
                    // TODO: ERROR
                }
                int idH = h;
                nextToken();
                if (buffer[h].type != TokenType::ASSIGN)
                {
                    // TODO: ERROR
                }
                nextToken();
                if (buffer[h].type != TokenType::CHARCON)
                {
                    printErr(buffer[idH].row, 'o', "except character constant, not '%s'", buffer[h].val.c_str());
                    // TODO: ERROR
                }
                char data = buffer[h].val[0];
                nextToken();

                if (global)
                {
                    if (funTrie.find(buffer[idH].val.c_str()) != 0)
                    {
                        printErr(buffer[idH].row, 'b', "redeclaration of '%s'", buffer[idH].val.c_str());
                        // TODO: ERROR
                        continue;
                    }
                    else
                    {
                        int& id = globalTrie.at(buffer[idH].val.c_str()); // TODO
                        if (id != 0) // TODO
                        {
                            printErr(buffer[idH].row, 'b', "redeclaration of '%s'", buffer[idH].val.c_str());
                            // TODO: ERROR
                            continue;
                        }
                        else
                        {
                            globalVector.emplace_back(VarType::CHAR, global, data, false);
                            id = globalVector.size();

                            // TODO
                        }
                    }
                }
                else
                {
                    int& id = localTrie.at(buffer[idH].val.c_str()); // TODO
                    if (id != 0)
                    {
                        printErr(buffer[idH].row, 'b', "redeclaration of '%s'", buffer[idH].val.c_str());
                        // TODO: ERROR
                        continue;
                    }
                    else
                    {
                        localVector.emplace_back(VarType::CHAR, global, data, false); // TODO
                        id = localVector.size();
                    }
                }
            } while (buffer[h].type == TokenType::COMMA);
        }
        else
        {
            // TODO: ERROR
        }

        print("<常量定义>\n");
    }

    int RecursiveParser::uinteger()
    {
        if (buffer[h].type != TokenType::INTCON)
        {
            throw ParsingError("not a uinteger");
            // TODO: ERROR
        }
        nextToken();

        print("<无符号整数>\n");
        try
        {
            return std::stoi(preToken().val);
        }
        catch(const std::invalid_argument& e)
        {
            // TODO: ERROR
            return INT_MAX;
        }
    }

    int RecursiveParser::integer()
    {
        int res = 0;

        if (buffer[h].type == TokenType::PLUS)
        {
            nextToken();
            res = uinteger();
        }
        else if (buffer[h].type == TokenType::MINU)
        {
            nextToken();
            res = -uinteger();
        }
        else if (buffer[h].type == TokenType::INTCON)
        {
            res = uinteger();
        }
        else
        {
            throw ParsingError("not a integer");
            // TODO: ERROR
        }

        print("<整数>\n");

        return res;
    }

    void RecursiveParser::declareHead()
    {
        VarType type = VarType::NONE;
        if (buffer[h].type == TokenType::INTTK)
        {
            type = VarType::INT;
        }
        else if (buffer[h].type == TokenType::CHARTK)
        {
            type = VarType::CHAR;
        }
        else
        {
            // TODO: ERROR
        }

        nextToken();
        if (buffer[h].type != TokenType::IDENFR)
        {
            // TODO: ERROR
        }

        if (globalTrie.find(buffer[h].val.c_str()) != 0)
        {
            printErr(buffer[h].row, 'b', "redeclaration of '%s'", buffer[h].val.c_str());
            // TODO: ERROR
        }
        int& id = funTrie.at(buffer[h].val.c_str());
        if (id != 0)
        {
            printErr(buffer[h].row, 'b', "redeclaration of '%s'", buffer[h].val.c_str());
            // TODO: ERROR
        }
        funVector.emplace_back(type, ip);
        id = funVector.size();

        nextToken();

        print("<声明头部>\n");
    }

    void RecursiveParser::varBlock()
    {
        while (true)
        {
            varDef();
            if (buffer[h].type != TokenType::SEMICN)
            {
                printErr(preToken().row, 'k', "expect ';' after '%s'", preToken().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextToken();
            }
            if (buffer[h].type != TokenType::INTTK && buffer[h].type != TokenType::CHARTK)
            {
                break;
            }
            nextToken(false);
            if (buffer[h].type != TokenType::IDENFR)
            {
                rollback(1);
                break;
            }
            nextToken(false);
            if (buffer[h].type != TokenType::LBRACK && buffer[h].type != TokenType::COMMA && buffer[h].type != TokenType::SEMICN)
            {
                rollback(2);
                break;
            }
            rollback(2);
        }

        print("<变量说明>\n");
    }

    void RecursiveParser::varDef()
    {
        VarType type = VarType::NONE;
        if (buffer[h].type == TokenType::INTTK)
        {
            type = VarType::INT;
        }
        else if (buffer[h].type == TokenType::CHARTK)
        {
            type = VarType::CHAR;
        }
        else
        {
            throw ParsingError("invalid type of var");
        }
        do
        {
            nextToken();
            if (buffer[h].type != TokenType::IDENFR)
            {
                // TODO: ERROR
            }
            int idH = h;
            int size = Var::SINGLE;
            nextToken();
            if (buffer[h].type == TokenType::LBRACK)
            {
                nextToken();
                size = uinteger();
                if (size == 0)
                {
                    // TODO: ERROR
                }
                if (buffer[h].type != TokenType::RBRACK)
                {
                    printErr(preToken().row, 'm', "except ']' after '%s'", preToken().val.c_str());
                    // TODO: ERROR
                }
                else
                {
                    nextToken();
                }
            }

            if (global)
            {
                if (funTrie.find(buffer[idH].val.c_str()) != 0)
                {
                    printErr(buffer[idH].row, 'b', "redeclaration of '%s'", buffer[idH].val.c_str());
                    // TODO: ERROR
                    continue;
                }
                else
                {
                    int& id = globalTrie.at(buffer[idH].val.c_str()); // TODO
                    if (id != 0) // TODO
                    {
                        printErr(buffer[idH].row, 'b', "redeclaration of '%s'", buffer[idH].val.c_str());
                        // TODO: ERROR
                        continue;
                    }
                    else
                    {
                        globalVector.emplace_back(type, global, globalSize, size);
                        if (size >= 0)
                        {
                            globalSize += size;
                        }
                        else
                        {
                            globalSize++;
                        }
                        id = globalVector.size();

                        // TODO
                    }
                }
            }
            else
            {
                int& id = localTrie.at(buffer[idH].val.c_str()); // TODO
                if (id != 0)
                {
                    printErr(buffer[idH].row, 'b', "redeclaration of '%s'", buffer[idH].val.c_str());
                    // TODO: ERROR
                    continue;
                }
                else
                {
                    id = localVector.size() + 1;
                    localVector.emplace_back(type, global, id - 1, size); // TODO
                }
            }
        } while (buffer[h].type == TokenType::COMMA);

        print("<变量定义>\n");
    }

    void RecursiveParser::funDef()
    {
        const int codeH = codes.size();

        declareHead();
        if (buffer[h].type != TokenType::LPARENT)
        {
            // TODO: ERROR
        }
        nextToken();
        param();
        if (buffer[h].type != TokenType::RPARENT)
        {
            printErr(preToken().row, 'l', "except ')' after '%s'", preToken().val.c_str());
            // TODO: ERROR
        }
        else
        {
            nextToken();
        }
        if (buffer[h].type != TokenType::LBRACE)
        {
            // TODO: ERROR
        }
        nextToken();

        int retStatus = compoundSt();
        if (retStatus == RET_NONE)
        {
            printWarning(buffer[h].row, 'h', "no return statement in function returning non-void");
            codes.emplace_back(0100, 0);
        }
        else if (retStatus == RET_PARTIAL)
        {
            printWarning(buffer[h].row, '\0', "control reaches end of non-void function");
            codes.emplace_back(0100, 0);
        }

        if (buffer[h].type != TokenType::RBRACE)
        {
            // TODO: ERROR
        }
        nextToken();

        allocAddr(codeH);

        print("<有返回值函数定义>\n");
    }

    void RecursiveParser::voidFunDef()
    {
        const int codeH = codes.size();

        assert(buffer[h].type == TokenType::VOIDTK);

        nextToken();
        if (buffer[h].type != TokenType::IDENFR)
        {
            // TODO: ERROR
        }

        if (globalTrie.find(buffer[h].val.c_str()) != 0)
        {
            printErr(buffer[h].row, 'b', "redeclaration of '%s'", buffer[h].val.c_str());
            // TODO: ERROR
        }
        int& id = funTrie.at(buffer[h].val.c_str());
        if (id != 0)
        {
            printErr(buffer[h].row, 'b', "redeclaration of '%s'", buffer[h].val.c_str());
            // TODO: ERROR
        }
        funVector.emplace_back(VarType::VOID, ip);
        id = funVector.size();

        nextToken();
        if (buffer[h].type != TokenType::LPARENT)
        {
            // TODO: ERROR
        }
        nextToken();
        param();
        if (buffer[h].type != TokenType::RPARENT)
        {
            printErr(preToken().row, 'l', "except ')' after '%s'", preToken().val.c_str());
            // TODO: ERROR
        }
        else
        {
            nextToken();
        }
        if (buffer[h].type != TokenType::LBRACE)
        {
            // TODO: ERROR
        }
        nextToken();

        if (compoundSt() != RET_ALL)
        {
            codes.emplace_back(0100, 0);
        }

        if (buffer[h].type != TokenType::RBRACE)
        {
            // TODO: ERROR
        }
        nextToken();

        allocAddr(codeH);

        print("<无返回值函数定义>\n");
    }

    int RecursiveParser::compoundSt()
    {
        codes.back().fork = true;

        if (buffer[h].type == TokenType::CONSTTK)
        {
            constBlock();
        }
        if (buffer[h].type == TokenType::INTTK || buffer[h].type == TokenType::CHARTK)
        {
            varBlock();
            codes.emplace_back(0050);
        }

        int retStatus = statementBlock(); // TODO: return

        print("<复合语句>\n");

        return retStatus;
    }

    void RecursiveParser::param()
    {
        VarType type = VarType::NONE;
        if (buffer[h].type == TokenType::INTTK)
        {
            type = VarType::INT;
        }
        else if (buffer[h].type == TokenType::CHARTK)
        {
            type = VarType::CHAR;
        }
        if (type != VarType::NONE)
        {
            nextToken();
            if (buffer[h].type != TokenType::IDENFR)
            {
                // TODO: ERROR
            }
            int* id = &localTrie.at(buffer[h].val.c_str());
            if (*id != 0)
            {
                // TODO: ERROR
            }
            Fun& fun = funVector.back();
            fun.paramTypes.push_back(type);
            localVector.emplace_back(type);
            *id = localVector.size();
            nextToken();
            while (buffer[h].type == TokenType::COMMA)
            {
                nextToken();
                if (buffer[h].type == TokenType::INTTK)
                {
                    type = VarType::INT;
                }
                else if (buffer[h].type == TokenType::CHARTK)
                {
                    type = VarType::CHAR;
                }
                else
                {
                    type = VarType::NONE;
                    // TODO: ERROR
                }
                nextToken();
                if (buffer[h].type != TokenType::IDENFR)
                {
                    // TODO: ERROR
                }
                id = &localTrie.at(buffer[h].val.c_str());
                if (*id != 0)
                {
                    // TODO: ERROR
                }
                fun.paramTypes.push_back(type);
                localVector.emplace_back(type);
                *id = localVector.size();
                nextToken();
            }
            int n = static_cast<int>(localVector.size());
            for (int i = 0; i < n; i++)
            {
                localVector[i].addr = i - n;
            }
        }

        print("<参数表>\n");
    }

    void RecursiveParser::mainFun()
    {
        const int codeH = codes.size();

        if (buffer[h].type != TokenType::VOIDTK)
        {
            // TODO: ERROR
        }
        nextToken();
        if (buffer[h].type != TokenType::MAINTK)
        {
            // TODO: ERROR
        }
        funVector.emplace_back(VarType::VOID, ip);
        funTrie.at(buffer[h].val.c_str()) = funVector.size();
        codes[0].code.a = ip;
        nextToken();
        if (buffer[h].type != TokenType::LPARENT)
        {
            // TODO: ERROR
        }
        nextToken();
        if (buffer[h].type != TokenType::RPARENT)
        {
            printErr(preToken().row, 'l', "except ')' after '%s'", preToken().val.c_str());
            // TODO: ERROR
        }
        else
        {
            nextToken();
        }
        if (buffer[h].type != TokenType::LBRACE)
        {
            // TODO: ERROR
        }
        nextToken();

        if (compoundSt() != RET_ALL)
        {
            codes.emplace_back(0100, 0);
        }

        if (buffer[h].type != TokenType::RBRACE)
        {
            // TODO: ERROR
        }
        nextToken();

        allocAddr(codeH);

        print("<主函数>\n");
    }

    VarType RecursiveParser::expression(int& lastCode)
    {
        VarType type;

        int curCode;

        if (buffer[h].type == TokenType::PLUS)
        {
            nextToken();
            item(lastCode);

            type = VarType::INT;
        }
        else if (buffer[h].type == TokenType::MINU)
        {
            nextToken();
            item(lastCode);
            codes.emplace_back(0100, 1, lastCode);
            lastCode = codes.size() - 1;

            type = VarType::INT;
        }
        else
        {
            type = item(lastCode);
        }

        while (true)
        {
            if (buffer[h].type == TokenType::PLUS)
            {
                nextToken();
                item(curCode);
                codes.emplace_back(0100, 2, curCode, lastCode);

                type = VarType::INT;
            }
            else if (buffer[h].type == TokenType::MINU)
            {
                nextToken();
                item(curCode);
                codes.emplace_back(0100, 3, curCode, lastCode);

                type = VarType::INT;
            }
            else
            {
                break;
            }

            lastCode = codes.size() - 1;
        }

        print("<表达式>\n");

        return type;
    }

    VarType RecursiveParser::item(int& lastCode)
    {
        VarType type = factor(lastCode);

        int curCode;

        while (buffer[h].type == TokenType::MULT || buffer[h].type == TokenType::DIV)
        {
            if (buffer[h].type == TokenType::MULT)
            {
                nextToken();
                factor(curCode);
                codes.emplace_back(0100, 4, curCode, lastCode);

                type = VarType::INT;
            }
            else if (buffer[h].type == TokenType::DIV)
            {
                nextToken();
                factor(curCode);
                codes.emplace_back(0100, 5, curCode, lastCode);

                type = VarType::INT;
            }
            else
            {
                break;
            }

            lastCode = codes.size() - 1;
        }

        print("<项>\n");

        return type;
    }

    VarType RecursiveParser::factor(int& lastCode)
    {
        VarType type = VarType::INT;

        switch (buffer[h].type)
        {
        case TokenType::IDENFR:
            nextToken(false);
            if (buffer[h].type == TokenType::LPARENT)
            {
                rollback(1);
                type = funCall();
                lastCode = codes.size() - 1;
            }
            else
            {
                rollback(1);

                Var* var;
                findVar(var);

                nextToken();
                if (buffer[h].type == TokenType::LBRACK)
                {
                    verifyElement(var, preToken());
                    nextToken();

                    if (expression(lastCode) != VarType::INT)
                    {
                        printErr(preToken().row, 'i', "invalid type for array subscript");
                    }

                    if (buffer[h].type != TokenType::RBRACK)
                    {
                        printErr(preToken().row, 'm', "except ']' after '%s'", preToken().val.c_str());
                        // TODO: ERROR
                    }
                    else
                    {
                        nextToken();
                    }

                    loadElement(var, lastCode);

                    lastCode = codes.size() - 1;
                }
                else
                {
                    verifyVar(var, preToken());
                    lastCode = loadVar(var);
                }

                if (var != nullptr)
                {
                    type = var->type;
                }
            }
            break;

        case TokenType::LPARENT:
            nextToken();
            expression(lastCode);
            if (buffer[h].type != TokenType::RPARENT)
            {
                printErr(preToken().row, 'l', "except ')' after '%s'", preToken().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextToken();
            }
            break;

        case TokenType::PLUS:
        case TokenType::MINU:
        case TokenType::INTCON:
            codes.emplace_back(0010, integer());
            lastCode = codes.size() - 1;
            break;

        case TokenType::CHARCON:
            codes.emplace_back(0010, static_cast<int>(buffer[h].val[0]));
            lastCode = codes.size() - 1;
            nextToken();
            type = VarType::CHAR;
            break;

        default:
            // TODO: ERROR
            break;
        }

        print("<因子>\n");

        return type;
    }

    int RecursiveParser::statement()
    {
        int retStatus = RET_NONE;

        switch (buffer[h].type)
        {
        case TokenType::IFTK:
            retStatus = conditionSt();
            break;

        case TokenType::WHILETK:
        case TokenType::DOTK:
        case TokenType::FORTK:
            retStatus = loopSt();
            break;

        case TokenType::LBRACE:
            nextToken();
            retStatus = statementBlock();
            if (buffer[h].type != TokenType::RBRACE)
            {
                // TODO: ERROR
            }
            nextToken();
            break;

        case TokenType::IDENFR:
            nextToken(false);
            if (buffer[h].type == TokenType::ASSIGN || buffer[h].type == TokenType::LBRACK)
            {
                rollback(1);
                assignSt();
            }
            else
            {
                rollback(1);
                int id = funTrie.find(buffer[h].val.c_str());
                if (id == 0)
                {
                    // TODO: ERROR
                    funCall(false);
                }
                else if (funVector[--id].returnType == VarType::VOID)
                {
                    voidFunCall();
                }
                else
                {
                    funCall(false);
                }
            }
            if (buffer[h].type != TokenType::SEMICN)
            {
                printErr(preToken().row, 'k', "expect ';' after '%s'", preToken().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextToken();
            }
            break;

        case TokenType::SCANFTK:
            readSt();
            if (buffer[h].type != TokenType::SEMICN)
            {
                printErr(preToken().row, 'k', "expect ';' after '%s'", preToken().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextToken();
            }
            break;

        case TokenType::PRINTFTK:
            writeSt();
            if (buffer[h].type != TokenType::SEMICN)
            {
                printErr(preToken().row, 'k', "expect ';' after '%s'", preToken().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextToken();
            }
            break;

        case TokenType::SEMICN:
            nextToken();
            break;

        case TokenType::RETURNTK:
            returnSt();
            if (buffer[h].type != TokenType::SEMICN)
            {
                printErr(preToken().row, 'k', "expect ';' after '%s'", preToken().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextToken();
            }
            retStatus = RET_ALL;
            break;

        default:
            // TODO: ERROR
            break;
        }

        print("<语句>\n");

        return retStatus;
    }

    void RecursiveParser::assignSt()
    {
        assert(buffer[h].type == TokenType::IDENFR);

        Var* var;
        findVar(var);

        nextToken();
        if (buffer[h].type == TokenType::ASSIGN)
        {
            verifyWritableVar(var, preToken());

            nextToken();

            int lastCode;
            VarType type = expression(lastCode);
            storeVar(var, type, lastCode);
        }
        else if (buffer[h].type == TokenType::LBRACK)
        {
            verifyElement(var, preToken());

            nextToken();

            int depCode1, depCode2;

            if (expression(depCode1) != VarType::INT)
            {
                printErr(preToken().row, 'i', "invalid type for array subscript");
            }

            if (buffer[h].type != TokenType::RBRACK)
            {
                printErr(preToken().row, 'm', "except ']' after '%s'", preToken().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextToken();
            }
            if (buffer[h].type != TokenType::ASSIGN)
            {
                // TODO: ERROR
            }
            nextToken();

            VarType type = expression(depCode2);
            storeElement(var, type, depCode1, depCode2);
        }
        else
        {
            throw ParsingError("expect '=' or '['");
        }

        print("<赋值语句>\n");
    }

    int RecursiveParser::conditionSt()
    {
        int retStatus1, retStatus;

        assert(buffer[h].type == TokenType::IFTK);

        nextToken();
        if (buffer[h].type != TokenType::LPARENT)
        {
            // TODO: ERROR
        }
        nextToken();
        int lastCode = condition();
        if (buffer[h].type != TokenType::RPARENT)
        {
            printErr(preToken().row, 'l', "except ')' after '%s'", preToken().val.c_str());
            // TODO: ERROR
        }
        else
        {
            nextToken();
        }

        int jpcIp = codes.size();
        codes.emplace_back(0070, lastCode);

        int preLoopCode = loopCode;
        loopCode = codes.size();
        loopLevel++;
        beginLoop();

        retStatus1 = statement();

        endLoop(false);
        loopCode = preLoopCode;
        --loopLevel;

        codes.back().fork = true;

        if (buffer[h].type == TokenType::ELSETK)
        {
            int jmpIp = codes.size();
            codes.emplace_back(0060);
            codes[jpcIp].code.a = codes.size();

            nextToken();

            int preLoopCode = loopCode;
            loopCode = codes.size();
            loopLevel++;
            beginLoop();

            retStatus = statement();
            retStatus |= retStatus1 & 1;
            retStatus &= retStatus1 | 1;

            endLoop(false);
            loopCode = preLoopCode;
            --loopLevel;

            codes[jmpIp].code.a = codes.size();

            codes.back().fork = true;

        }
        else
        {
            retStatus = retStatus1 & 1;

            codes[jpcIp].code.a = codes.size();
        }

        print("<条件语句>\n");

        return retStatus;
    }

    int RecursiveParser::condition(bool inv)
    {
        VarType type = VarType::INT;

        int lastCode, curCode;

        if (expression(lastCode) != VarType::INT)
        {
            printErr(preToken().row, 'f', "invalid type for condition");
        }

        switch (buffer[h].type)
        {
        case TokenType::LSS:
            nextToken();
            type = expression(curCode);
            codes.emplace_back(0100, inv && optimize ? 11 : 8, curCode, lastCode);
            lastCode = codes.size() - 1;
            break;

        case TokenType::LEQ:
            nextToken();
            type = expression(curCode);
            codes.emplace_back(0100, inv && optimize ? 10 : 9, curCode, lastCode);
            lastCode = codes.size() - 1;
            break;

        case TokenType::GRE:
            nextToken();
            type = expression(curCode);
            codes.emplace_back(0100, inv && optimize ? 9 : 10, curCode, lastCode);
            lastCode = codes.size() - 1;
            break;

        case TokenType::GEQ:
            nextToken();
            type = expression(curCode);
            codes.emplace_back(0100, inv && optimize ? 8 : 11, curCode, lastCode);
            lastCode = codes.size() - 1;
            break;

        case TokenType::EQL:
            nextToken();
            type = expression(curCode);
            codes.emplace_back(0100, inv && optimize ? 13 : 12, curCode, lastCode);
            lastCode = codes.size() - 1;
            break;

        case TokenType::NEQ:
            nextToken();
            type = expression(curCode);
            codes.emplace_back(0100, inv && optimize ? 12 : 13, curCode, lastCode);
            lastCode = codes.size() - 1;
            break;

        default:
            if (inv && optimize)
            {
                codes.emplace_back(0100, 7, lastCode);
                lastCode = codes.size() - 1;
            }
            break;
        }

        if (!optimize && inv)
        {
            codes.emplace_back(0010, 0);
            codes.emplace_back(0100, 12, codes.size() - 1, lastCode);
            lastCode = codes.size() - 1;
        }

        if (type != VarType::INT)
        {
            printErr(preToken().row, 'f', "invalid type for condition");
        }

        print("<条件>\n");

        return lastCode;
    }

    int RecursiveParser::loopSt()
    {
        int retStatus = RET_NONE;

        if (buffer[h].type == TokenType::WHILETK)
        {
            codes.back().fork = true;

            nextToken();
            if (buffer[h].type != TokenType::LPARENT)
            {
                // TODO: ERROR
            }
            nextToken();

            int conditionIp = codes.size();
            int preLoopCode = loopCode;
            loopCode = conditionIp;
            loopLevel++;
            beginLoop();

            int lastCode = condition();
            if (buffer[h].type != TokenType::RPARENT)
            {
                printErr(preToken().row, 'l', "except ')' after '%s'", preToken().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextToken();
            }

            int jpcIp = codes.size();
            codes.emplace_back(0070, lastCode);

            retStatus = statement() & 1;

            codes.emplace_back(0060, conditionIp);

            codes[jpcIp].code.a = codes.size();

            codes.back().fork = true;

            endLoop();
            loopCode = preLoopCode;
            --loopLevel;

        }
        else if (buffer[h].type == TokenType::DOTK)
        {
            int doIp = codes.size();
            int preLoopCode = loopCode;
            loopCode = doIp;
            loopLevel++;
            beginLoop();

            codes.back().fork = true;

            nextToken();
            retStatus = statement();
            if (buffer[h].type != TokenType::WHILETK)
            {
                printErr(buffer[h].row, 'n', "except 'while' before '%s'", buffer[h].val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextToken();
            }
            if (buffer[h].type != TokenType::LPARENT)
            {
                // TODO: ERROR
            }
            nextToken();
            int lastCode = condition(true);
            if (buffer[h].type != TokenType::RPARENT)
            {
                printErr(preToken().row, 'l', "except ')' after '%s'", preToken().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextToken();
            }

            codes.emplace_back(0070, doIp, lastCode);

            endLoop();
            loopCode = preLoopCode;
            --loopLevel;

        }
        else if (buffer[h].type == TokenType::FORTK)
        {
            nextToken();
            if (buffer[h].type != TokenType::LPARENT)
            {
                // TODO: ERROR
            }
            nextToken();
            if (buffer[h].type != TokenType::IDENFR)
            {
                // TODO: ERROR
            }

            Var* var;
            findVar(var);
            verifyWritableVar(var, buffer[h]);

            nextToken();
            if (buffer[h].type != TokenType::ASSIGN)
            {
                // TODO: ERROR
            }
            nextToken();

            int lastCode;
            VarType type = expression(lastCode);
            storeVar(var, type, lastCode); // NOTE: Cautious when optimize (i)

            codes.back().fork = true;

            if (buffer[h].type != TokenType::SEMICN)
            {
                printErr(preToken().row, 'k', "expect ';' after '%s'", preToken().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextToken();
            }

            int conditionIp = codes.size();
            int preLoopCode = loopCode;
            loopCode = conditionIp;
            loopLevel++;
            beginLoop();

            lastCode = condition();
            if (buffer[h].type != TokenType::SEMICN)
            {
                printErr(preToken().row, 'k', "expect ';' after '%s'", preToken().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextToken();
            }

            int jpcIp = codes.size();
            codes.emplace_back(0070, lastCode);

            if (buffer[h].type != TokenType::IDENFR)
            {
                // TODO: ERROR
            }

            findVar(var);
            verifyWritableVar(var, buffer[h]);

            nextToken();
            if (buffer[h].type != TokenType::ASSIGN)
            {
                // TODO: ERROR
            }
            nextToken();
            if (buffer[h].type != TokenType::IDENFR)
            {
                // TODO: ERROR
            }

            Var* varR;
            findVar(varR);
            verifyVar(var, buffer[h]);
            /// TODO

            bool plus = true;
            int st;

            nextToken();
            if (buffer[h].type == TokenType::PLUS)
            {
                nextToken();
            }
            else if (buffer[h].type == TokenType::MINU)
            {
                plus = false;
                nextToken();
            }
            else
            {
                // TODO: ERROR
            }
            st = step();
            if (buffer[h].type != TokenType::RPARENT)
            {
                printErr(preToken().row, 'l', "except ')' after '%s'", preToken().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextToken();
            }

            retStatus = statement() & 1;

            lastCode = loadVar(varR);

            codes.emplace_back(0010, st);

            if (plus)
            {
                codes.emplace_back(0100, 2, codes.size() - 1, lastCode);
            }
            else
            {
                codes.emplace_back(0100, 3, codes.size() - 1, lastCode);
            }

            storeVar(var, VarType::INT, codes.size() - 1);

            codes.emplace_back(0060, conditionIp);
            codes[jpcIp].code.a = codes.size();

            codes.back().fork = true;

            endLoop();
            loopCode = preLoopCode;
            --loopLevel;

        }
        else
        {
            throw ParsingError("not a loop statement");
        }

        print("<循环语句>\n");

        return retStatus;
    }

    int RecursiveParser::step()
    {
        int res = uinteger();

        print("<步长>\n");

        return res;
    }

    VarType RecursiveParser::funCall(bool remain)
    {
        assert(buffer[h].type == TokenType::IDENFR);

        int id = funTrie.find(buffer[h].val.c_str());
        if (id == 0)
        {
            printErr(buffer[h].row, 'c', "'%s' was not declared in this scope", buffer[h].val.c_str());
            // TODO: ERROR
        }
        else
        {
            --id;
        }
        const Fun& fun = funVector[id];
        if (fun.returnType == VarType::VOID)
        {
            // TODO: ERROR
        }

        nextToken();
        if (buffer[h].type != TokenType::LPARENT)
        {
            // TODO: ERROR
        }
        nextToken();
        paramVal(fun);
        if (buffer[h].type != TokenType::RPARENT)
        {
            printErr(preToken().row, 'l', "except ')' after '%s'", preToken().val.c_str());
            // TODO: ERROR
        }
        else
        {
            nextToken();
        }

        int&& paramCnt = fun.paramTypes.size();
        if (paramCnt == 0)
        {
            codes.emplace_back(0042, fun.addr);
            if (!remain)
            {
                codes.emplace_back(0000, 1);
            }
        }
        else
        {
            codes.emplace_back(0040, fun.addr);
            if (!remain)
            {
                codes.emplace_back(0000, paramCnt);
            }
            else if (paramCnt > 1)
            {
                codes.emplace_back(0000, paramCnt - 1);
            }
        }

        print("<有返回值函数调用语句>\n");

        return fun.returnType;
    }

    void RecursiveParser::voidFunCall()
    {
        assert(buffer[h].type == TokenType::IDENFR);

        int id = funTrie.find(buffer[h].val.c_str());
        if (id == 0)
        {
            printErr(buffer[h].row, 'c', "'%s' was not declared in this scope", buffer[h].val.c_str());
            // TODO: ERROR
        }
        else
        {
            --id;
        }
        const Fun& fun = funVector[id];
        if (fun.returnType != VarType::VOID)
        {
            // TODO: ERROR
        }

        nextToken();
        if (buffer[h].type != TokenType::LPARENT)
        {
            // TODO: ERROR
        }
        nextToken();
        paramVal(fun);
        if (buffer[h].type != TokenType::RPARENT)
        {
            printErr(preToken().row, 'l', "except ')' after '%s'", preToken().val.c_str());
            // TODO: ERROR
        }
        else
        {
            nextToken();
        }

        codes.emplace_back(0040, fun.addr);
        int&& paramCnt = fun.paramTypes.size();
        if (paramCnt != 0)
        {
            codes.emplace_back(0000, paramCnt);
        }

        print("<无返回值函数调用语句>\n");
    }

    void RecursiveParser::paramVal(const Fun& fun)
    {
        int i = 0;
        int n = fun.paramTypes.size();

        if (EXPRESSION_SELECT[static_cast<unsigned>(buffer[h].type)])
        {
            VarType type;
            int lastCode;
            type = expression(lastCode); // TODO: judge
            codes[lastCode].remain = 1;
            if (i < n && type != fun.paramTypes[i])
            {
                printErr(preToken().row, 'e', "the type of 0th parameter mismatches");
            }
            ++i;

            while (buffer[h].type == TokenType::COMMA)
            {
                nextToken();
                int lastCode;
                type = expression(lastCode); // TODO: judge
                codes[lastCode].remain = 1;
                if (i < n && type != fun.paramTypes[i])
                {
                    printErr(preToken().row, 'e', "the type of %dth parameter mismatches", i);
                }
                ++i;
            }
        }

        if (i != n)
        {
            if (i > n)
            {
                printErr(buffer[h].row, 'd', "too many arguments");
            }
            else
            {
                printErr(buffer[h].row, 'd', "too few arguments");
            }
        }

        print("<值参数表>\n");
    }

    int RecursiveParser::statementBlock()
    {
        int retStatus = RET_NONE;

        while (STATEMENT_SELECT[static_cast<unsigned>(buffer[h].type)])
        {
            retStatus |= statement();
        }

        print("<语句列>\n");

        return retStatus;
    }

    void RecursiveParser::readSt()
    {
        Var* var;
        assert(buffer[h].type == TokenType::SCANFTK);

        nextToken();
        if (buffer[h].type != TokenType::LPARENT)
        {
            // TODO: ERROR
        }
        do
        {
            nextToken();
            if (buffer[h].type != TokenType::IDENFR)
            {
                // TODO: ERROR
            }

            findVar(var);
            verifyWritableVar(var, buffer[h]);

            if (var != nullptr)
            {
                if (var->type == VarType::INT)
                {
                    codes.emplace_back(0100, 16);
                }
                else
                {
                    codes.emplace_back(0100, 17);
                }

                storeVar(var, var->type, codes.size() - 1);
            }

            nextToken();

        } while (buffer[h].type == TokenType::COMMA);

        if (buffer[h].type != TokenType::RPARENT)
        {
            printErr(preToken().row, 'l', "except ')' after '%s'", preToken().val.c_str());
            // TODO: ERROR
        }
        else
        {
            nextToken();
        }

        print("<读语句>\n");
    }

    void RecursiveParser::writeSt()
    {
        assert(buffer[h].type == TokenType::PRINTFTK);

        nextToken();
        if (buffer[h].type != TokenType::LPARENT)
        {
            // TODO: ERROR
        }
        nextToken();

        int lastCode;

        if (buffer[h].type == TokenType::STRCON)
        {
            codes.emplace_back(0010, str());
            codes.emplace_back(0100, 18, codes.size() - 1);
            if (buffer[h].type == TokenType::COMMA)
            {
                nextToken();
                if (expression(lastCode) == VarType::INT)
                {
                    codes.emplace_back(0100, 14, lastCode);
                }
                else
                {
                    codes.emplace_back(0100, 19, lastCode);
                }
            }
            else
            {
                codes.emplace_back(0100, 15);
            }
        }
        else if (EXPRESSION_SELECT[static_cast<unsigned>(buffer[h].type)])
        {
            if (expression(lastCode) == VarType::INT)
            {
                codes.emplace_back(0100, 14, lastCode);
            }
            else
            {
                codes.emplace_back(0100, 19, lastCode);
            }
        }
        if (buffer[h].type != TokenType::RPARENT)
        {
            printErr(preToken().row, 'l', "except ')' after '%s'", preToken().val.c_str());
            // TODO: ERROR
        }
        else
        {
            nextToken();
        }

        print("<写语句>\n");
    }

    void RecursiveParser::returnSt()
    {
        assert(buffer[h].type == TokenType::RETURNTK);

        nextToken();

        const Fun& fun = funVector.back();

        if (buffer[h].type == TokenType::LPARENT)
        {
            nextToken();

            int lastCode;

            if (expression(lastCode) != fun.returnType)
            {
                if (fun.returnType == VarType::VOID)
                {
                    printErr(buffer[h].row, 'g', "return-statement with a value, in function returning 'void'");
                }
                else
                {
                    printErr(buffer[h].row, 'h', "the type of return value mismatches");
                }
            }
            if (buffer[h].type != TokenType::RPARENT)
            {
                printErr(preToken().row, 'l', "except ')' after '%s'", preToken().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextToken();
            }

            // TODO: judge

            codes.emplace_back(0030, -std::max(static_cast<int>(fun.paramTypes.size()), 1), lastCode);
            codes.back().remain = 1;
        }
        else
        {
            if (fun.returnType != VarType::VOID)
            {
                printErr(buffer[h].row, 'h', "return-statement with no value, in non-void function");
                // TODO: ERROR
            }
        }

        codes.emplace_back(0100, 0);

        print("<返回语句>\n");
    }

    bool RecursiveParser::parse()
    {
        assert(lexer != nullptr);

        global = true;

        nextToken(false);

        if (buffer[h].type == TokenType::CONSTTK)
        {
            constBlock();
        }

        if (buffer[h].type == TokenType::INTTK || buffer[h].type == TokenType::CHARTK)
        {
            nextToken(false);
            if (buffer[h].type != TokenType::IDENFR)
            {
                rollback(1);
            }
            else
            {
                nextToken(false);
                if (buffer[h].type != TokenType::LBRACK && buffer[h].type != TokenType::COMMA && buffer[h].type != TokenType::SEMICN)
                {
                    rollback(2);
                }
                else
                {
                    rollback(2);
                    varBlock();
                }
            }
        }

        global = false;

        codes.emplace_back(0040);
        codes.back().remain = 1;
        ++ip;

        while (true)
        {
            localTrie.clear();
            localVector.clear();
            if (buffer[h].type == TokenType::INTTK || buffer[h].type == TokenType::CHARTK)
            {
                funDef();
            }
            else if (buffer[h].type == TokenType::VOIDTK)
            {
                nextToken(false);
                if (buffer[h].type != TokenType::IDENFR)
                {
                    rollback(1);
                    break;
                }
                rollback(1);
                voidFunDef();
            }
            else
            {
                break; // TODO
            }
        }

        // TODO

        mainFun();

        print("<程序>\n");

        // TODO

        return !hasError;
    }
}
