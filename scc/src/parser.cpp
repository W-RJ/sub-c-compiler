#include <cstdio>
#include <climits>
#include <cstdarg>
#include <cassert>

#include <algorithm>
#include <vector>
#include <string>
#include <queue>

#include "lexer.h"
#include "parser.h"
#include "trie"
#include "define.h"

#include "../../common/src/pcode.h"
#include "../../common/src/exception.h"

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

    ExCode::ExCode(unsigned f) : code{f}, id(0), fork(false), bg(INT_MAX >> 1), dependentVar(0)
    {
    }

    ExCode::ExCode(unsigned f, int a) : code{f, a}, id(0), fork(false), bg(INT_MAX >> 1), dependentVar(0)
    {
    }

    // class Parser

    bool Parser::EXPRESSION_SELECT[static_cast<unsigned>(WordType::END)] = {false};
    bool Parser::STATEMENT_SELECT[static_cast<unsigned>(WordType::END)] = {false};

    bool Parser::hasInited = false;

    void Parser::init()
    {
        EXPRESSION_SELECT[static_cast<unsigned>(WordType::PLUS)] = true;
        EXPRESSION_SELECT[static_cast<unsigned>(WordType::MINU)] = true;
        EXPRESSION_SELECT[static_cast<unsigned>(WordType::IDENFR)] = true;
        EXPRESSION_SELECT[static_cast<unsigned>(WordType::LPARENT)] = true;
        EXPRESSION_SELECT[static_cast<unsigned>(WordType::INTCON)] = true;
        EXPRESSION_SELECT[static_cast<unsigned>(WordType::CHARCON)] = true;

        STATEMENT_SELECT[static_cast<unsigned>(WordType::IFTK)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(WordType::WHILETK)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(WordType::DOTK)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(WordType::FORTK)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(WordType::LBRACE)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(WordType::IDENFR)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(WordType::SCANFTK)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(WordType::PRINTFTK)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(WordType::SEMICN)] = true;
        STATEMENT_SELECT[static_cast<unsigned>(WordType::RETURNTK)] = true;
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

    void Parser::write(const char* fileName)
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

        fwrite(&sci::BPCODE_PREFIX, sizeof(sci::BPCODE_PREFIX), 1, fp);

        fwrite(&globalSize, sizeof(int), 1, fp);

        fwrite(&strSize, sizeof(int), 1, fp);

        int zero = 0;

        for (auto it : strVector)
        {
            fwrite(it.first.c_str(), it.first.size(), 1, fp);
            fwrite(&zero, sizeof(int) - (it.first.size()) % sizeof(int), 1, fp);
        }

        fwrite(&ip, sizeof(int), 1, fp);

        fwrite(&codes[0].code, sizeof(codes[0].code), 1, fp);
        for (auto it = codes.begin() + 1; it != codes.end(); ++it)
        {
            if (it->id >= static_cast<int>(optimize))
            {
                fwrite(&it->code, sizeof(it->code), 1, fp);
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

        for (auto it : strVector)
        {
            fprintf(fp, "%s 0 %d %s\n", sci::fs[013].name, static_cast<int>(it.first.size() / sizeof(int)) + 1, it.first.c_str());
        }

        fprintf(fp, "%s\n", sci::TPCODE_CODE);

        fprintf(fp, "%s %u %d\n", sci::fs[codes[0].code.f >> 3].name, codes[0].code.f & 07, codes[0].code.a);
        for (auto it = codes.begin() + 1; it != codes.end(); ++it)
        {
            if (it->id >= static_cast<int>(optimize))
            {
                fprintf(fp, "%s %u %d\n", sci::fs[it->code.f >> 3].name, it->code.f & 07, it->code.a);
            }
        }

        fclose(fp);
    }

    void Parser::nextWord(bool accept)
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
            buffer[h].type = WordType::NONE;
            buffer[h].val.clear();
            lexer->nextWord(buffer[h]);
            if (buffer[h].type == WordType::INTERR)
            {
                printErr(buffer[h].row, 'a', "invalid integer constant '%s'", buffer[h].val.c_str());
                buffer[h].type = WordType::INTCON;
            }
            else if (buffer[h].type == WordType::CHARERR)
            {
                printErr(buffer[h].row, 'a', "invalid character constant '%c'", buffer[h].val[0]);
                buffer[h].type = WordType::CHARCON;
            }
            else if (buffer[h].type == WordType::NONE)
            {
                printErr(buffer[h].row, 'a', "invalid word '%c'", buffer[h].val.c_str());
            }
        }
    }

    void Parser::rollback(unsigned n)
    {
        h = (h - n + CACHE_MAX) % CACHE_MAX;
        size += n;
    }

    Word& Parser::preWord(unsigned n)
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

    void Parser::verifyVar(Var*& var, const Word& word)
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

    void Parser::verifyWritableVar(Var*& var, const Word& word)
    {
        if (var != nullptr)
        {
            if (!var->writable)
            {
                printErr(word.row, 'j', "assignment of read-only variable '%s'", word.val.c_str());
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

    void Parser::verifyElement(Var*& var, const Word& word)
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
                    if (preCode.code.f == 0031 && preCode.code.a == var->addr && preCode.id >= 0 && !preCode.fork && optimize)
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
                    if (preCode.code.f == 0030 && preCode.code.a == var->addr && preCode.id >= 0 && !preCode.fork && optimize)
                    {
                        preCode.code.f |= 2u;
                        res = codes.size() - 2;
                        while (codes[res].code.f == 0032 && codes[res].id >= 0)
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

    void Parser::loadElement(Var* var)
    {
        if (var != nullptr)
        {
            if (var->global)
            {
                codes.emplace_back(0111, var->addr);
            }
            else
            {
                codes.emplace_back(0110, var->addr);
            }
        }
    }

    void Parser::storeVar(Var* var, VarType type, int exp)
    {
        if (var != nullptr)
        {
            if (var->type != type)
            {
                // TODO: ERROR
            }
            if (var->global)
            {
                codes.emplace_back(0031, var->addr);
                codes.back().dependentCodes.push_back(exp);
            }
            else
            {
                var->preAssign.resize(loopLevel);
                var->preAssign.emplace_back();
                var->preAssign.back().push_back(codes.size());
                var->isPreAssign.back() = true;
                codes.emplace_back(0030, var->addr);
                codes.back().dependentCodes.push_back(exp);
            }
        }
    }

    void Parser::storeElement(Var* var, VarType type, int exp1, int exp2)
    {
        if (var != nullptr)
        {
            if (var->type != type)
            {
                // TODO: ERROR
            }
            if (var->global)
            {
                codes.emplace_back(0121, var->addr);
                codes.back().dependentCodes.push_back(exp1);
                codes.back().dependentCodes.push_back(exp2);
            }
            else
            {
                codes.emplace_back(0120, var->addr);
                codes.back().dependentCodes.push_back(exp1);
                codes.back().dependentCodes.push_back(exp2);
            }
        }
    }

    void Parser::allocAddr(int codesH)
    {
        Fun& fun = funVector.back();
        int n = codes.size();
        int tmp;
        std::queue<int> q;
        bool* vis = new bool[n - codesH];
        memset(vis, false, (n - codesH) * sizeof(bool));
        for (int i = n - 1; i >= codesH; --i)
        {
            if (codes[i].id == 1
                || (codes[i].code.f != 0010 && codes[i].code.f != 0020
                    && codes[i].code.f != 0030 && codes[i].code.f != 0032)
                || (codes[i].code.f == 0100 && (codes[i].code.a < 2 || codes[i].code.a > 5)))
            {
                codes[i].id = 1;
                vis[i - codesH] = true;
                q.push(i);
            }
        }
        while (!q.empty())
        {
            tmp = q.front();     

            for (auto it : codes[tmp].dependentCodes)
            {
                if (it != 0 && !vis[it - codesH])
                {
                    codes[it].id = 1;
                    vis[it - codesH] = true;
                    q.push(it);
                }
            }
            q.pop();
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
            codes[codesH].id = -1;
        }

        n = codes.size();
        for (int i = codesH; i < n; i++)
        {
            if (codes[i].id >= static_cast<int>(optimize))
            {
                codes[i].id = ip++;
            }
            // TODO
        }

        for (int i = codesH; i < n; i++)
        {
            if (codes[i].id >= static_cast<int>(optimize))
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
        if (buffer[h].type != WordType::STRCON)
        {
            // TODO: ERROR
        }
        int& id = strTrie.at(buffer[h].val.c_str());
        if (id == 0)
        {
            strVector.emplace_back(buffer[h].val, globalSize + strSize);
            id = strVector.size();
            strSize += (buffer[h].val.size()) / sizeof(int) + 1;
        }

        nextWord();

        print("<字符串>\n");

        return strVector[id - 1].second;
    }

    void RecursiveParser::constBlock()
    {
        if (buffer[h].type != WordType::CONSTTK)
        {
            // TODO: ERROR
        }
        do
        {
            nextWord();
            constDef();
            if (buffer[h].type != WordType::SEMICN)
            {
                printErr(preWord().row, 'k', "expect ';' after '%s'", preWord().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextWord();
            }

        } while (buffer[h].type == WordType::CONSTTK);

        print("<常量说明>\n");
    }

    void RecursiveParser::constDef()
    {
        if (buffer[h].type == WordType::INTTK)
        {
            do
            {
                nextWord();
                if (buffer[h].type != WordType::IDENFR)
                {
                    // TODO: ERROR
                }
                int idH = h;
                nextWord();
                if (buffer[h].type != WordType::ASSIGN)
                {
                    // TODO: ERROR
                }
                nextWord();

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
            } while (buffer[h].type == WordType::COMMA);
        }
        else if (buffer[h].type == WordType::CHARTK)
        {
            do
            {
                nextWord();
                if (buffer[h].type != WordType::IDENFR)
                {
                    // TODO: ERROR
                }
                int idH = h;
                nextWord();
                if (buffer[h].type != WordType::ASSIGN)
                {
                    // TODO: ERROR
                }
                nextWord();
                if (buffer[h].type != WordType::CHARCON)
                {
                    printErr(buffer[idH].row, 'o', "except character constant, not '%s'", buffer[h].val.c_str());
                    // TODO: ERROR
                }
                char data = buffer[h].val[0];
                nextWord();

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
            } while (buffer[h].type == WordType::COMMA);
        }
        else
        {
            // TODO: ERROR
        }

        print("<常量定义>\n");
    }

    int RecursiveParser::uinteger()
    {
        if (buffer[h].type != WordType::INTCON)
        {
            throw ParsingError("not a uinteger");
            // TODO: ERROR
        }
        nextWord();

        print("<无符号整数>\n");
        try
        {
            return std::stoi(preWord().val);
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

        if (buffer[h].type == WordType::PLUS)
        {
            nextWord();
            res = uinteger();
        }
        else if (buffer[h].type == WordType::MINU)
        {
            nextWord();
            res = -uinteger();
        }
        else if (buffer[h].type == WordType::INTCON)
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
        if (buffer[h].type == WordType::INTTK)
        {
            type = VarType::INT;
        }
        else if (buffer[h].type == WordType::CHARTK)
        {
            type = VarType::CHAR;
        }
        else
        {
            // TODO: ERROR
        }

        nextWord();
        if (buffer[h].type != WordType::IDENFR)
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

        nextWord();

        print("<声明头部>\n");
    }

    void RecursiveParser::varBlock()
    {
        while (true)
        {
            varDef();
            if (buffer[h].type != WordType::SEMICN)
            {
                printErr(preWord().row, 'k', "expect ';' after '%s'", preWord().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextWord();
            }
            if (buffer[h].type != WordType::INTTK && buffer[h].type != WordType::CHARTK)
            {
                break;
            }
            nextWord(false);
            if (buffer[h].type != WordType::IDENFR)
            {
                rollback(1);
                break;
            }
            nextWord(false);
            if (buffer[h].type != WordType::LBRACK && buffer[h].type != WordType::COMMA && buffer[h].type != WordType::SEMICN)
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
        if (buffer[h].type == WordType::INTTK)
        {
            type = VarType::INT;
        }
        else if (buffer[h].type == WordType::CHARTK)
        {
            type = VarType::CHAR;
        }
        else
        {
            // TODO: ERROR
        }
        do
        {
            nextWord();
            if (buffer[h].type != WordType::IDENFR)
            {
                // TODO: ERROR
            }
            int idH = h;
            int size = Var::SINGLE;
            nextWord();
            if (buffer[h].type == WordType::LBRACK)
            {
                nextWord();
                size = uinteger();
                if (size == 0)
                {
                    // TODO: ERROR
                }
                if (buffer[h].type != WordType::RBRACK)
                {
                    printErr(preWord().row, 'm', "except ']' after '%s'", preWord().val.c_str());
                    // TODO: ERROR
                }
                else
                {
                    nextWord();
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
        } while (buffer[h].type == WordType::COMMA);

        print("<变量定义>\n");
    }

    void RecursiveParser::funDef()
    {
        const int codeH = codes.size();

        declareHead();
        if (buffer[h].type != WordType::LPARENT)
        {
            // TODO: ERROR
        }
        nextWord();
        param();
        if (buffer[h].type != WordType::RPARENT)
        {
            printErr(preWord().row, 'l', "except ')' after '%s'", preWord().val.c_str());
            // TODO: ERROR
        }
        else
        {
            nextWord();
        }
        if (buffer[h].type != WordType::LBRACE)
        {
            // TODO: ERROR
        }
        nextWord();

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

        if (buffer[h].type != WordType::RBRACE)
        {
            // TODO: ERROR
        }
        nextWord();

        allocAddr(codeH);

        print("<有返回值函数定义>\n");
    }

    void RecursiveParser::voidFunDef()
    {
        const int codeH = codes.size();

        if (buffer[h].type != WordType::VOIDTK)
        {
            // TODO: ERROR
        }
        nextWord();
        if (buffer[h].type != WordType::IDENFR)
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

        nextWord();
        if (buffer[h].type != WordType::LPARENT)
        {
            // TODO: ERROR
        }
        nextWord();
        param();
        if (buffer[h].type != WordType::RPARENT)
        {
            printErr(preWord().row, 'l', "except ')' after '%s'", preWord().val.c_str());
            // TODO: ERROR
        }
        else
        {
            nextWord();
        }
        if (buffer[h].type != WordType::LBRACE)
        {
            // TODO: ERROR
        }
        nextWord();

        if (compoundSt() != RET_ALL)
        {
            codes.emplace_back(0100, 0);
        }

        if (buffer[h].type != WordType::RBRACE)
        {
            // TODO: ERROR
        }
        nextWord();

        allocAddr(codeH);

        print("<无返回值函数定义>\n");
    }

    int RecursiveParser::compoundSt()
    {
        codes.back().fork = true;

        if (buffer[h].type == WordType::CONSTTK)
        {
            constBlock();
        }
        if (buffer[h].type == WordType::INTTK || buffer[h].type == WordType::CHARTK)
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
        if (buffer[h].type == WordType::INTTK)
        {
            type = VarType::INT;
        }
        else if (buffer[h].type == WordType::CHARTK)
        {
            type = VarType::CHAR;
        }
        if (type != VarType::NONE)
        {
            nextWord();
            if (buffer[h].type != WordType::IDENFR)
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
            nextWord();
            while (buffer[h].type == WordType::COMMA)
            {
                nextWord();
                if (buffer[h].type == WordType::INTTK)
                {
                    type = VarType::INT;
                }
                else if (buffer[h].type == WordType::CHARTK)
                {
                    type = VarType::CHAR;
                }
                else
                {
                    type = VarType::NONE;
                    // TODO: ERROR
                }
                nextWord();
                if (buffer[h].type != WordType::IDENFR)
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
                nextWord();
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

        if (buffer[h].type != WordType::VOIDTK)
        {
            // TODO: ERROR
        }
        nextWord();
        if (buffer[h].type != WordType::MAINTK)
        {
            // TODO: ERROR
        }
        funVector.emplace_back(VarType::VOID, ip);
        funTrie.at(buffer[h].val.c_str()) = funVector.size();
        codes[0].code.a = ip;
        nextWord();
        if (buffer[h].type != WordType::LPARENT)
        {
            // TODO: ERROR
        }
        nextWord();
        if (buffer[h].type != WordType::RPARENT)
        {
            printErr(preWord().row, 'l', "except ')' after '%s'", preWord().val.c_str());
            // TODO: ERROR
        }
        else
        {
            nextWord();
        }
        if (buffer[h].type != WordType::LBRACE)
        {
            // TODO: ERROR
        }
        nextWord();

        if (compoundSt() != RET_ALL)
        {
            codes.emplace_back(0100, 0);
        }

        if (buffer[h].type != WordType::RBRACE)
        {
            // TODO: ERROR
        }
        nextWord();

        allocAddr(codeH);

        print("<主函数>\n");
    }

    VarType RecursiveParser::expression(int& lastCode)
    {
        VarType type;

        int curCode;

        if (buffer[h].type == WordType::PLUS)
        {
            nextWord();
            item(lastCode);

            type = VarType::INT;
        }
        else if (buffer[h].type == WordType::MINU)
        {
            nextWord();
            item(lastCode);
            codes.emplace_back(0100, 1);
            codes.back().dependentCodes.push_back(lastCode);
            lastCode = codes.size() - 1;

            type = VarType::INT;
        }
        else
        {
            type = item(lastCode);
        }

        while (true)
        {
            if (buffer[h].type == WordType::PLUS)
            {
                nextWord();
                item(curCode);
                codes.emplace_back(0100, 2);
                codes.back().dependentCodes.push_back(curCode);
                codes.back().dependentCodes.push_back(lastCode);

                type = VarType::INT;
            }
            else if (buffer[h].type == WordType::MINU)
            {
                nextWord();
                item(curCode);
                codes.emplace_back(0100, 3);
                codes.back().dependentCodes.push_back(curCode);
                codes.back().dependentCodes.push_back(lastCode);

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

        while (buffer[h].type == WordType::MULT || buffer[h].type == WordType::DIV)
        {
            if (buffer[h].type == WordType::MULT)
            {
                nextWord();
                factor(curCode);
                codes.emplace_back(0100, 4);
                codes.back().dependentCodes.push_back(curCode);
                codes.back().dependentCodes.push_back(lastCode);

                type = VarType::INT;
            }
            else if (buffer[h].type == WordType::DIV)
            {
                nextWord();
                factor(curCode);
                codes.emplace_back(0100, 5);
                codes.back().dependentCodes.push_back(curCode);
                codes.back().dependentCodes.push_back(lastCode);

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
        case WordType::IDENFR:
            nextWord(false);
            if (buffer[h].type == WordType::LPARENT)
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

                nextWord();
                if (buffer[h].type == WordType::LBRACK)
                {
                    verifyElement(var, preWord());
                    nextWord();

                    if (expression(lastCode) != VarType::INT)
                    {
                        printErr(preWord().row, 'i', "invalid type for array subscript");
                    }

                    if (buffer[h].type != WordType::RBRACK)
                    {
                        printErr(preWord().row, 'm', "except ']' after '%s'", preWord().val.c_str());
                        // TODO: ERROR
                    }
                    else
                    {
                        nextWord();
                    }

                    loadElement(var);

                    lastCode = codes.size() - 1;
                }
                else
                {
                    verifyVar(var, preWord());
                    lastCode = loadVar(var);
                }

                if (var != nullptr)
                {
                    type = var->type;
                }
            }
            break;

        case WordType::LPARENT:
            nextWord();
            expression(lastCode);
            if (buffer[h].type != WordType::RPARENT)
            {
                printErr(preWord().row, 'l', "except ')' after '%s'", preWord().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextWord();
            }
            break;

        case WordType::PLUS:
        case WordType::MINU:
        case WordType::INTCON:
            codes.emplace_back(0010, integer());
            lastCode = codes.size() - 1;
            break;

        case WordType::CHARCON:
            codes.emplace_back(0010, static_cast<int>(buffer[h].val[0]));
            lastCode = codes.size() - 1;
            nextWord();
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
        case WordType::IFTK:
            retStatus = conditionSt();
            break;

        case WordType::WHILETK:
        case WordType::DOTK:
        case WordType::FORTK:
            retStatus = loopSt();
            break;

        case WordType::LBRACE:
            nextWord();
            retStatus = statementBlock();
            if (buffer[h].type != WordType::RBRACE)
            {
                // TODO: ERROR
            }
            nextWord();
            break;

        case WordType::IDENFR:
            nextWord(false);
            if (buffer[h].type == WordType::ASSIGN || buffer[h].type == WordType::LBRACK)
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
            if (buffer[h].type != WordType::SEMICN)
            {
                printErr(preWord().row, 'k', "expect ';' after '%s'", preWord().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextWord();
            }
            break;

        case WordType::SCANFTK:
            readSt();
            if (buffer[h].type != WordType::SEMICN)
            {
                printErr(preWord().row, 'k', "expect ';' after '%s'", preWord().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextWord();
            }
            break;

        case WordType::PRINTFTK:
            writeSt();
            if (buffer[h].type != WordType::SEMICN)
            {
                printErr(preWord().row, 'k', "expect ';' after '%s'", preWord().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextWord();
            }
            break;

        case WordType::SEMICN:
            nextWord();
            break;

        case WordType::RETURNTK:
            returnSt();
            if (buffer[h].type != WordType::SEMICN)
            {
                printErr(preWord().row, 'k', "expect ';' after '%s'", preWord().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextWord();
            }
            retStatus = RET_ALL;
            break;

        default:
            break;
        }

        print("<语句>\n");

        return retStatus;
    }

    void RecursiveParser::assignSt()
    {
        if (buffer[h].type != WordType::IDENFR)
        {
            // TODO: ERROR
        }

        Var* var;
        findVar(var);

        nextWord();
        if (buffer[h].type == WordType::ASSIGN)
        {
            verifyWritableVar(var, preWord());

            nextWord();

            int lastCode;
            VarType type = expression(lastCode);
            storeVar(var, type, lastCode);
        }
        else if (buffer[h].type == WordType::LBRACK)
        {
            verifyElement(var, preWord());

            nextWord();

            int exp1, exp2;

            if (expression(exp1) != VarType::INT)
            {
                printErr(preWord().row, 'i', "invalid type for array subscript");
            }

            if (buffer[h].type != WordType::RBRACK)
            {
                printErr(preWord().row, 'm', "except ']' after '%s'", preWord().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextWord();
            }
            if (buffer[h].type != WordType::ASSIGN)
            {
                // TODO: ERROR
            }
            nextWord();

            VarType type = expression(exp2);
            storeElement(var, type, exp1, exp2);
        }
        else
        {
            // TODO: ERROR
        }

        print("<赋值语句>\n");
    }

    int RecursiveParser::conditionSt()
    {
        int retStatus1, retStatus;

        if (buffer[h].type != WordType::IFTK)
        {
            // TODO: ERROR
        }
        nextWord();
        if (buffer[h].type != WordType::LPARENT)
        {
            // TODO: ERROR
        }
        nextWord();
        int lastCode = condition();
        if (buffer[h].type != WordType::RPARENT)
        {
            printErr(preWord().row, 'l', "except ')' after '%s'", preWord().val.c_str());
            // TODO: ERROR
        }
        else
        {
            nextWord();
        }

        int jpcIp = codes.size();
        codes.emplace_back(0070);
        codes.back().dependentCodes.push_back(lastCode);

        int preLoopCode = loopCode;
        loopCode = codes.size();
        loopLevel++;
        beginLoop();

        retStatus1 = statement();

        endLoop(false);
        loopCode = preLoopCode;
        --loopLevel;

        codes.back().fork = true;

        if (buffer[h].type == WordType::ELSETK)
        {
            int jmpIp = codes.size();
            codes.emplace_back(0060);
            codes[jpcIp].code.a = codes.size();

            nextWord();

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
            printErr(preWord().row, 'f', "invalid type for condition");
        }

        switch (buffer[h].type)
        {
        case WordType::LSS:
            nextWord();
            type = expression(curCode);
            codes.emplace_back(0100, inv && optimize ? 11 : 8);
            codes.back().dependentCodes.push_back(curCode);
            codes.back().dependentCodes.push_back(lastCode);
            lastCode = codes.size() - 1;
            break;

        case WordType::LEQ:
            nextWord();
            type = expression(curCode);
            codes.emplace_back(0100, inv && optimize ? 10 : 9);
            codes.back().dependentCodes.push_back(curCode);
            codes.back().dependentCodes.push_back(lastCode);
            lastCode = codes.size() - 1;
            break;

        case WordType::GRE:
            nextWord();
            type = expression(curCode);
            codes.emplace_back(0100, inv && optimize ? 9 : 10);
            codes.back().dependentCodes.push_back(curCode);
            codes.back().dependentCodes.push_back(lastCode);
            lastCode = codes.size() - 1;
            break;

        case WordType::GEQ:
            nextWord();
            type = expression(curCode);
            codes.emplace_back(0100, inv && optimize ? 8 : 11);
            codes.back().dependentCodes.push_back(curCode);
            codes.back().dependentCodes.push_back(lastCode);
            lastCode = codes.size() - 1;
            break;

        case WordType::EQL:
            nextWord();
            type = expression(curCode);
            codes.emplace_back(0100, inv && optimize ? 13 : 12);
            codes.back().dependentCodes.push_back(curCode);
            codes.back().dependentCodes.push_back(lastCode);
            lastCode = codes.size() - 1;
            break;

        case WordType::NEQ:
            nextWord();
            type = expression(curCode);
            codes.emplace_back(0100, inv && optimize ? 12 : 13);
            codes.back().dependentCodes.push_back(curCode);
            codes.back().dependentCodes.push_back(lastCode);
            lastCode = codes.size() - 1;
            break;

        default:
            if (inv && optimize)
            {
                codes.emplace_back(0100, 7);
                codes.back().dependentCodes.push_back(lastCode);
                lastCode = codes.size() - 1;
            }
            break;
        }

        if (!optimize && inv)
        {
            codes.emplace_back(0010, 0);
            codes.emplace_back(0100, 12);
            codes.back().dependentCodes.push_back(codes.size() - 2);
            codes.back().dependentCodes.push_back(lastCode);
            lastCode = codes.size() - 1;
        }

        if (type != VarType::INT)
        {
            printErr(preWord().row, 'f', "invalid type for condition");
        }

        print("<条件>\n");

        return lastCode;
    }

    int RecursiveParser::loopSt()
    {
        int retStatus = RET_NONE;

        if (buffer[h].type == WordType::WHILETK)
        {
            codes.back().fork = true;

            nextWord();
            if (buffer[h].type != WordType::LPARENT)
            {
                // TODO: ERROR
            }
            nextWord();

            int conditionIp = codes.size();
            int preLoopCode = loopCode;
            loopCode = conditionIp;
            loopLevel++;
            beginLoop();

            int lastCode = condition();
            if (buffer[h].type != WordType::RPARENT)
            {
                printErr(preWord().row, 'l', "except ')' after '%s'", preWord().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextWord();
            }

            int jpcIp = codes.size();
            codes.emplace_back(0070);
            codes.back().dependentCodes.push_back(lastCode);

            retStatus = statement() & 1;

            codes.emplace_back(0060, conditionIp);

            codes[jpcIp].code.a = codes.size();

            codes.back().fork = true;

            endLoop();
            loopCode = preLoopCode;
            --loopLevel;

        }
        else if (buffer[h].type == WordType::DOTK)
        {
            int doIp = codes.size();
            int preLoopCode = loopCode;
            loopCode = doIp;
            loopLevel++;
            beginLoop();

            codes.back().fork = true;

            nextWord();
            retStatus = statement();
            if (buffer[h].type != WordType::WHILETK)
            {
                printErr(buffer[h].row, 'n', "except 'while' before '%s'", buffer[h].val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextWord();
            }
            if (buffer[h].type != WordType::LPARENT)
            {
                // TODO: ERROR
            }
            nextWord();
            int lastCode = condition(true);
            if (buffer[h].type != WordType::RPARENT)
            {
                printErr(preWord().row, 'l', "except ')' after '%s'", preWord().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextWord();
            }

            codes.emplace_back(0070, doIp);
            codes.back().dependentCodes.push_back(lastCode);

            endLoop();
            loopCode = preLoopCode;
            --loopLevel;

        }
        else if (buffer[h].type == WordType::FORTK)
        {
            nextWord();
            if (buffer[h].type != WordType::LPARENT)
            {
                // TODO: ERROR
            }
            nextWord();
            if (buffer[h].type != WordType::IDENFR)
            {
                // TODO: ERROR
            }

            Var* var;
            findVar(var);
            verifyWritableVar(var, buffer[h]);

            nextWord();
            if (buffer[h].type != WordType::ASSIGN)
            {
                // TODO: ERROR
            }
            nextWord();

            int lastCode;
            VarType type = expression(lastCode);
            storeVar(var, type, lastCode); // NOTE: Cautious when optimize (i)

            codes.back().fork = true;

            if (buffer[h].type != WordType::SEMICN)
            {
                printErr(preWord().row, 'k', "expect ';' after '%s'", preWord().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextWord();
            }

            int conditionIp = codes.size();
            int preLoopCode = loopCode;
            loopCode = conditionIp;
            loopLevel++;
            beginLoop();

            lastCode = condition();
            if (buffer[h].type != WordType::SEMICN)
            {
                printErr(preWord().row, 'k', "expect ';' after '%s'", preWord().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextWord();
            }

            int jpcIp = codes.size();
            codes.emplace_back(0070);
            codes.back().dependentCodes.push_back(lastCode);

            if (buffer[h].type != WordType::IDENFR)
            {
                // TODO: ERROR
            }

            findVar(var);
            verifyWritableVar(var, buffer[h]);

            nextWord();
            if (buffer[h].type != WordType::ASSIGN)
            {
                // TODO: ERROR
            }
            nextWord();
            if (buffer[h].type != WordType::IDENFR)
            {
                // TODO: ERROR
            }

            Var* varR;
            findVar(varR);
            verifyVar(var, buffer[h]);
            /// TODO

            bool plus = true;
            int st;

            nextWord();
            if (buffer[h].type == WordType::PLUS)
            {
                nextWord();
            }
            else if (buffer[h].type == WordType::MINU)
            {
                plus = false;
                nextWord();
            }
            else
            {
                // TODO: ERROR
            }
            st = step();
            if (buffer[h].type != WordType::RPARENT)
            {
                printErr(preWord().row, 'l', "except ')' after '%s'", preWord().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextWord();
            }

            retStatus = statement() & 1;

            lastCode = loadVar(varR);

            codes.emplace_back(0010, st);

            if (plus)
            {
                codes.emplace_back(0100, 2);
                codes.back().dependentCodes.push_back(lastCode);
                codes.back().dependentCodes.push_back(codes.size() - 2);
            }
            else
            {
                codes.emplace_back(0100, 3);
                codes.back().dependentCodes.push_back(lastCode);
                codes.back().dependentCodes.push_back(codes.size() - 2);
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
            // TODO: ERROR
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
        if (buffer[h].type != WordType::IDENFR)
        {
            // TODO: ERROR
        }

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

        nextWord();
        if (buffer[h].type != WordType::LPARENT)
        {
            // TODO: ERROR
        }
        nextWord();
        paramVal(fun);
        if (buffer[h].type != WordType::RPARENT)
        {
            printErr(preWord().row, 'l', "except ')' after '%s'", preWord().val.c_str());
            // TODO: ERROR
        }
        else
        {
            nextWord();
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
        if (buffer[h].type != WordType::IDENFR)
        {
            // TODO: ERROR
        }

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

        nextWord();
        if (buffer[h].type != WordType::LPARENT)
        {
            // TODO: ERROR
        }
        nextWord();
        paramVal(fun);
        if (buffer[h].type != WordType::RPARENT)
        {
            printErr(preWord().row, 'l', "except ')' after '%s'", preWord().val.c_str());
            // TODO: ERROR
        }
        else
        {
            nextWord();
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
            codes[lastCode].id = 1;
            if (i < n && type != fun.paramTypes[i])
            {
                printErr(preWord().row, 'e', "the type of 0th parameter mismatches");
            }
            ++i;

            while (buffer[h].type == WordType::COMMA)
            {
                nextWord();
                int lastCode;
                type = expression(lastCode); // TODO: judge
                codes[lastCode].id = 1;
                if (i < n && type != fun.paramTypes[i])
                {
                    printErr(preWord().row, 'e', "the type of %dth parameter mismatches", i);
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
        if (buffer[h].type != WordType::SCANFTK)
        {
            // TODO: ERROR
        }
        nextWord();
        if (buffer[h].type != WordType::LPARENT)
        {
            // TODO: ERROR
        }
        do
        {
            nextWord();
            if (buffer[h].type != WordType::IDENFR)
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

            nextWord();

        } while (buffer[h].type == WordType::COMMA);

        if (buffer[h].type != WordType::RPARENT)
        {
            printErr(preWord().row, 'l', "except ')' after '%s'", preWord().val.c_str());
            // TODO: ERROR
        }
        else
        {
            nextWord();
        }

        print("<读语句>\n");
    }

    void RecursiveParser::writeSt()
    {
        if (buffer[h].type != WordType::PRINTFTK)
        {
            // TODO: ERROR
        }
        nextWord();
        if (buffer[h].type != WordType::LPARENT)
        {
            // TODO: ERROR
        }
        nextWord();

        int lastCode;

        if (buffer[h].type == WordType::STRCON)
        {
            codes.emplace_back(0010, str());
            codes.emplace_back(0100, 18);
            codes.back().dependentCodes.push_back(codes.size() - 2);
            if (buffer[h].type == WordType::COMMA)
            {
                nextWord();
                if (expression(lastCode) == VarType::INT)
                {
                    codes.emplace_back(0100, 14);
                }
                else
                {
                    codes.emplace_back(0100, 19);
                }
                codes.back().dependentCodes.push_back(lastCode);
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
                codes.emplace_back(0100, 14);
            }
            else
            {
                codes.emplace_back(0100, 19);
            }
            codes.back().dependentCodes.push_back(lastCode);
        }
        if (buffer[h].type != WordType::RPARENT)
        {
            printErr(preWord().row, 'l', "except ')' after '%s'", preWord().val.c_str());
            // TODO: ERROR
        }
        else
        {
            nextWord();
        }

        print("<写语句>\n");
    }

    void RecursiveParser::returnSt()
    {
        if (buffer[h].type != WordType::RETURNTK)
        {
            // TODO: ERROR
        }
        nextWord();

        const Fun& fun = funVector.back();

        if (buffer[h].type == WordType::LPARENT)
        {
            nextWord();

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
            if (buffer[h].type != WordType::RPARENT)
            {
                printErr(preWord().row, 'l', "except ')' after '%s'", preWord().val.c_str());
                // TODO: ERROR
            }
            else
            {
                nextWord();
            }

            // TODO: judge

            codes.emplace_back(0030, -std::max(static_cast<int>(fun.paramTypes.size()), 1));
            codes.back().dependentCodes.push_back(lastCode);
            codes.back().id = 1;
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

    void RecursiveParser::parse()
    {
        assert(lexer != nullptr);

        global = true;

        nextWord(false);

        if (buffer[h].type == WordType::CONSTTK)
        {
            constBlock();
        }

        if (buffer[h].type == WordType::INTTK || buffer[h].type == WordType::CHARTK)
        {
            nextWord(false);
            if (buffer[h].type != WordType::IDENFR)
            {
                rollback(1);
            }
            else
            {
                nextWord(false);
                if (buffer[h].type != WordType::LBRACK && buffer[h].type != WordType::COMMA && buffer[h].type != WordType::SEMICN)
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
        ++ip;

        while (true)
        {
            localTrie.clear();
            localVector.clear();
            if (buffer[h].type == WordType::INTTK || buffer[h].type == WordType::CHARTK)
            {
                funDef();
            }
            else if (buffer[h].type == WordType::VOIDTK)
            {
                nextWord(false);
                if (buffer[h].type != WordType::IDENFR)
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
    }
}
