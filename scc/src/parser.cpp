#include <cstdio>
#include <climits>
#include <cassert>
#include <algorithm>
#include <vector>
#include <string>

#include "lexer.h"
#include "parser.h"
#include "trie"
#include "../../common/src/pcode.h"
#include "../../common/src/exception.h"

namespace scc
{
    // struct Var

    Var::Var(VarType type) : type(type), global(false), writable(true), size(SINGLE)
    {
    }

    Var::Var(VarType type, bool global, int addr, bool writable) : type(type), global(global), writable(writable), addr(addr), size(SINGLE)
    {
    }

    Var::Var(VarType type, bool global, int addr, int size) : type(type), global(global), writable(true), addr(addr), size(size)
    {
    }

    // struct Fun

    Fun::Fun(VarType returnType, int addr) : addr(addr), returnType(returnType)
    {
    }

    // struct ExCode

    ExCode::ExCode(unsigned f) : code{f}, id(0)
    {
    }

    ExCode::ExCode(unsigned f, int a) : code{f, a}, id(0)
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
            parserFp(nullptr), ip(0), optimize(optimize), global(true), globalSize(0), strSize(0)
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
            throw NullPointerError("Parser::setLexer: lexer is null");
        }
        this->lexer = lexer;
    }


    void Parser::open(const char* lexFileName, const char* parserFileName)
    {
        if (lexFileName != nullptr)
        {
            if (lexFp != nullptr)
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
            if (parserFp != nullptr)
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

        for (auto it : codes)
        {
            if (it.id >= 0)
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

        // TODO

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
                // TODO: ERROR
                var = nullptr;
            }
        }
    }

    void Parser::loadVar(Var* var)
    {
        if (var == nullptr)
        {
            // TODO: ERROR
        }
        else if (var->writable)
        {
            if (var->size != Var::SINGLE)
            {
                // TODO: ERROR
            }
            if (var->global)
            {
                codes.emplace_back(0021, var->addr);
            }
            else
            {
                codes.emplace_back(0020, var->addr);
            }
        }
        else
        {
            codes.emplace_back(0010, var->addr);
        }
    }

    void Parser::loadElement(Var* var)
    {
        if (var == nullptr)
        {
            // TODO: ERROR
        }
        else if (var->size == Var::SINGLE)
        {
            // TODO: ERROR
        }
        else
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

    void Parser::storeVar(Var* var)
    {
        if (var == nullptr)
        {
            // TODO: ERROR
        }
        else if (!var->writable)
        {
            // TODO: ERROR
        }
        else if (var->size != Var::SINGLE)
        {
            // TODO: ERROR
        }
        else
        {
            if (var->global)
            {
                codes.emplace_back(0031, var->addr);
            }
            else
            {
                codes.emplace_back(0030, var->addr);
            }
        }
    }

    void Parser::storeElement(Var* var)
    {
        if (var == nullptr)
        {
            // TODO: ERROR
        }
        else if (var->size == Var::SINGLE)
        {
            // TODO: ERROR
        }
        else
        {
            if (var->global)
            {
                codes.emplace_back(0121, var->addr);
            }
            else
            {
                codes.emplace_back(0120, var->addr);
            }
        }
    }

    // class RecursiveParser

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
                // TODO: ERROR
            }
            nextWord();

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
                int data = integer();

                if (global)
                {
                    if (funTrie.find(buffer[idH].val.c_str()) != 0)
                    {
                        // TODO: ERROR
                        continue;
                    }
                    else
                    {
                        int& id = globalTrie.at(buffer[idH].val.c_str()); // TODO
                        if (id != 0) // TODO
                        {
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
                    // TODO: ERROR
                }
                char data = buffer[h].val[0];
                nextWord();

                if (global)
                {
                    if (funTrie.find(buffer[idH].val.c_str()) != 0)
                    {
                        // TODO: ERROR
                        continue;
                    }
                    else
                    {
                        int& id = globalTrie.at(buffer[idH].val.c_str()); // TODO
                        if (id != 0) // TODO
                        {
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
            // TODO: ERROR
        }
        nextWord();

        print("<无符号整数>\n");
        try
        {
            return std::stoi(preWord(1).val);
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
            // TODO: ERROR
        }
        int& id = funTrie.at(buffer[h].val.c_str());
        if (id != 0)
        {
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
                // TODO: ERROR
            }
            nextWord();
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
                    // TODO: ERROR
                }
                nextWord();
            }

            if (global)
            {
                if (funTrie.find(buffer[idH].val.c_str()) != 0)
                {
                    // TODO: ERROR
                    continue;
                }
                else
                {
                    int& id = globalTrie.at(buffer[idH].val.c_str()); // TODO
                    if (id != 0) // TODO
                    {
                        // TODO: ERROR
                        continue;
                    }
                    else
                    {
                        globalVector.emplace_back(type, global, globalSize++, size);
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
        declareHead();
        if (buffer[h].type != WordType::LPARENT)
        {
            // TODO: ERROR
        }
        nextWord();
        param();
        if (buffer[h].type != WordType::RPARENT)
        {
            // TODO: ERROR
        }
        nextWord();
        if (buffer[h].type != WordType::LBRACE)
        {
            // TODO: ERROR
        }
        nextWord();
        compoundSt();
        if (buffer[h].type != WordType::RBRACE)
        {
            // TODO: ERROR
        }
        nextWord();

        print("<有返回值函数定义>\n");
    }

    void RecursiveParser::voidFunDef()
    {
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
            // TODO: ERROR
        }
        int& id = funTrie.at(buffer[h].val.c_str());
        if (id != 0)
        {
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
            // TODO: ERROR
        }
        nextWord();
        if (buffer[h].type != WordType::LBRACE)
        {
            // TODO: ERROR
        }
        nextWord();
        compoundSt();
        if (buffer[h].type != WordType::RBRACE)
        {
            // TODO: ERROR
        }
        nextWord();

        print("<无返回值函数定义>\n");
    }

    void RecursiveParser::compoundSt()
    {
        if (buffer[h].type == WordType::CONSTTK)
        {
            constBlock();
        }
        if (buffer[h].type == WordType::INTTK || buffer[h].type == WordType::CHARTK)
        {
            varBlock();
        }
        statementBlock();

        print("<复合语句>\n");
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
            // TODO: ERROR
        }
        nextWord();
        if (buffer[h].type != WordType::LBRACE)
        {
            // TODO: ERROR
        }
        nextWord();
        compoundSt();
        if (buffer[h].type != WordType::RBRACE)
        {
            // TODO: ERROR
        }
        nextWord();

        print("<主函数>\n");
    }

    void RecursiveParser::expression()
    {
        if (buffer[h].type == WordType::PLUS)
        {
            nextWord();
            item();
        }
        else if (buffer[h].type == WordType::MINU)
        {
            nextWord();
            item();
            codes.emplace_back(0100, 1);
        }
        else
        {
            item();
        }
        while (true)
        {
            if (buffer[h].type == WordType::PLUS)
            {
                nextWord();
                item();
                codes.emplace_back(0100, 2);
            }
            else if (buffer[h].type == WordType::MINU)
            {
                nextWord();
                item();
                codes.emplace_back(0100, 3);
            }
            else
            {
                break;
            }
        }

        print("<表达式>\n");
    }

    void RecursiveParser::item()
    {
        factor();
        while (buffer[h].type == WordType::MULT || buffer[h].type == WordType::DIV)
        {
            if (buffer[h].type == WordType::MULT)
            {
                nextWord();
                factor();
                codes.emplace_back(0100, 4);
            }
            else if (buffer[h].type == WordType::DIV)
            {
                nextWord();
                factor();
                codes.emplace_back(0100, 5);
            }
            else
            {
                break;
            }
        }

        print("<项>\n");
    }

    void RecursiveParser::factor()
    {
        switch (buffer[h].type)
        {
        case WordType::IDENFR:
            nextWord(false);
            if (buffer[h].type == WordType::LPARENT)
            {
                rollback(1);
                funCall();
            }
            else
            {
                rollback(1);
                nextWord();
                if (buffer[h].type == WordType::LBRACK)
                {
                    nextWord();
                    expression();
                    if (buffer[h].type != WordType::RBRACK)
                    {
                        // TODO: ERROR
                    }
                    nextWord();
                }
            }
            break;

        case WordType::LPARENT:
            nextWord();
            expression();
            if (buffer[h].type != WordType::RPARENT)
            {
                // TODO: ERROR
            }
            nextWord();
            break;

        case WordType::PLUS:
        case WordType::MINU:
        case WordType::INTCON:
            codes.emplace_back(0010, integer());
            break;

        case WordType::CHARCON:
            codes.emplace_back(0010, static_cast<int>(buffer[h].val[0]));
            nextWord();
            break;

        default:
            // TODO: ERROR
            break;
        }

        print("<因子>\n");
    }

    void RecursiveParser::statement()
    {
        switch (buffer[h].type)
        {
        case WordType::IFTK:
            conditionSt();
            break;

        case WordType::WHILETK:
        case WordType::DOTK:
        case WordType::FORTK:
            loopSt();
            break;

        case WordType::LBRACE:
            nextWord();
            statementBlock();
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
                // TODO: ERROR
            }
            nextWord();
            break;

        case WordType::SCANFTK:
            readSt();
            if (buffer[h].type != WordType::SEMICN)
            {
                // TODO: ERROR
            }
            nextWord();
            break;

        case WordType::PRINTFTK:
            writeSt();
            if (buffer[h].type != WordType::SEMICN)
            {
                // TODO: ERROR
            }
            nextWord();
            break;

        case WordType::SEMICN:
            nextWord();
            break;

        case WordType::RETURNTK:
            returnSt();
            if (buffer[h].type != WordType::SEMICN)
            {
                // TODO: ERROR
            }
            nextWord();
            break;

        default:
            break;
        }

        print("<语句>\n");
    }

    void RecursiveParser::assignSt()
    {
        if (buffer[h].type != WordType::IDENFR)
        {
            // TODO: ERROR
        }
        nextWord();
        if (buffer[h].type == WordType::ASSIGN)
        {
            nextWord();
            expression();
        }
        else if (buffer[h].type == WordType::LBRACK)
        {
            nextWord();
            expression();
            if (buffer[h].type != WordType::RBRACK)
            {
                // TODO: ERROR
            }
            nextWord();
            if (buffer[h].type != WordType::ASSIGN)
            {
                // TODO: ERROR
            }
            nextWord();
            expression();
        }
        else
        {
            // TODO: ERROR
        }

        print("<赋值语句>\n");
    }

    void RecursiveParser::conditionSt()
    {
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
        condition();
        if (buffer[h].type != WordType::RPARENT)
        {
            // TODO: ERROR
        }
        nextWord();

        int jpcIp = codes.size();
        codes.emplace_back(0070);

        statement();
        if (buffer[h].type == WordType::ELSETK)
        {
            int jmpIp = codes.size();
            codes.emplace_back(0060);
            codes[jpcIp].code.a = codes.size();

            nextWord();
            statement();

            codes[jmpIp].code.a = codes.size();
        }
        else
        {
            codes[jpcIp].code.a = codes.size();
        }

        print("<条件语句>\n");
    }

    void RecursiveParser::condition(bool inv)
    {
        expression();
        switch (buffer[h].type)
        {
        case WordType::LSS:
            nextWord();
            expression();
            codes.emplace_back(0100, inv && optimize ? 11 : 8);
            break;

        case WordType::LEQ:
            nextWord();
            expression();
            codes.emplace_back(0100, inv && optimize ? 10 : 9);
            break;

        case WordType::GRE:
            nextWord();
            expression();
            codes.emplace_back(0100, inv && optimize ? 9 : 10);
            break;

        case WordType::GEQ:
            nextWord();
            expression();
            codes.emplace_back(0100, inv && optimize ? 8 : 11);
            break;

        case WordType::EQL:
            nextWord();
            expression();
            codes.emplace_back(0100, inv && optimize ? 13 : 12);
            break;

        case WordType::NEQ:
            nextWord();
            expression();
            codes.emplace_back(0100, inv && optimize ? 12 : 13);
            break;

        default:
            if (inv && optimize)
            {
                codes.emplace_back(0100, 7);
            }
            break;
        }

        if (!optimize && inv)
        {
            codes.emplace_back(0010, 0);
            codes.emplace_back(0100, 12);
        }

        print("<条件>\n");
    }

    void RecursiveParser::loopSt()
    {
        if (buffer[h].type == WordType::WHILETK)
        {
            nextWord();
            if (buffer[h].type != WordType::LPARENT)
            {
                // TODO: ERROR
            }
            nextWord();

            int conditionIp = codes.size();

            condition();
            if (buffer[h].type != WordType::RPARENT)
            {
                // TODO: ERROR
            }
            nextWord();

            int jpcIp = codes.size();
            codes.emplace_back(0070);

            statement();

            codes.emplace_back(0060, conditionIp);

            codes[jpcIp].code.a = codes.size();
        }
        else if (buffer[h].type == WordType::DOTK)
        {
            int doIp = codes.size();

            nextWord();
            statement();
            if (buffer[h].type != WordType::WHILETK)
            {
                // TODO: ERROR
            }
            nextWord();
            if (buffer[h].type != WordType::LPARENT)
            {
                // TODO: ERROR
            }
            nextWord();
            condition(true);
            if (buffer[h].type != WordType::RPARENT)
            {
                // TODO: ERROR
            }
            nextWord();

            codes.emplace_back(0070, doIp);
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
            nextWord();
            if (buffer[h].type != WordType::ASSIGN)
            {
                // TODO: ERROR
            }
            nextWord();
            expression();
            if (buffer[h].type != WordType::SEMICN)
            {
                // TODO: ERROR
            }
            nextWord();
            condition();
            if (buffer[h].type != WordType::SEMICN)
            {
                // TODO: ERROR
            }
            nextWord();
            if (buffer[h].type != WordType::IDENFR)
            {
                // TODO: ERROR
            }
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
            nextWord();
            if (buffer[h].type == WordType::PLUS)
            {
                nextWord();
            }
            else if (buffer[h].type == WordType::MINU)
            {
                nextWord();
            }
            else
            {
                // TODO: ERROR
            }
            step();
            if (buffer[h].type != WordType::RPARENT)
            {
                // TODO: ERROR
            }
            nextWord();
            statement();
        }
        else
        {
            // TODO: ERROR
        }

        print("<循环语句>\n");
    }

    int RecursiveParser::step()
    {
        int res = uinteger();

        print("<步长>\n");

        return res;
    }

    void RecursiveParser::funCall(bool remain)
    {
        if (buffer[h].type != WordType::IDENFR)
        {
            // TODO: ERROR
        }

        int id = funTrie.find(buffer[h].val.c_str());
        if (id == 0)
        {
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
            // TODO: ERROR
        }
        nextWord();

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
            // TODO: ERROR
        }
        nextWord();

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
        if (EXPRESSION_SELECT[static_cast<unsigned>(buffer[h].type)])
        {
            expression(); // TODO: judge
            while (buffer[h].type == WordType::COMMA)
            {
                nextWord();
                expression(); // TODO: judge
            }
        }

        print("<值参数表>\n");
    }

    void RecursiveParser::statementBlock()
    {
        while (STATEMENT_SELECT[static_cast<unsigned>(buffer[h].type)])
        {
            statement();
        }

        print("<语句列>\n");
    }

    void RecursiveParser::readSt()
    {
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
            nextWord();
        } while (buffer[h].type == WordType::COMMA);

        if (buffer[h].type != WordType::RPARENT)
        {
            // TODO: ERROR
        }
        nextWord();

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
        if (buffer[h].type == WordType::STRCON)
        {
            codes.emplace_back(0010, str());
            codes.emplace_back(0100, 18);
            if (buffer[h].type == WordType::COMMA)
            {
                nextWord();
                expression();
                codes.emplace_back(0100, 14);
            }
            else
            {
                codes.emplace_back(0100, 15);
            }
        }
        else if (EXPRESSION_SELECT[static_cast<unsigned>(buffer[h].type)])
        {
            expression();
            codes.emplace_back(0100, 14);
        }
        if (buffer[h].type != WordType::RPARENT)
        {
            // TODO: ERROR
        }
        nextWord();

        print("<写语句>\n");
    }

    void RecursiveParser::returnSt()
    {
        if (buffer[h].type != WordType::RETURNTK)
        {
            // TODO: ERROR
        }
        nextWord();
        if (buffer[h].type == WordType::LPARENT)
        {
            nextWord();
            expression();
            if (buffer[h].type != WordType::RPARENT)
            {
                // TODO: ERROR
            }
            nextWord();
        }

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

        while (true)
        {
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
