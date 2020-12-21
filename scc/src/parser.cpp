#include <cstdio>
#include <string>
#include <climits>
#include <cassert>

#include "lexer.h"
#include "parser.h"
#include "../../common/src/exception.h"

namespace scc
{
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

    Parser::Parser() : lexer(nullptr), h(0), size(0), lexFp(nullptr), parserFp(nullptr), global(true)
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

    // class RecursiveParser

    void RecursiveParser::str()
    {
        if (buffer[h].type != WordType::STRCON)
        {
            // TODO: ERROR
        }
        nextWord();

        print("<字符串>\n");
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
                integer();

                Var* var;
                if (global)
                {
                    if (funTrie.find(buffer[idH].val.c_str()).returnType != VarType::NONE)
                    {
                        // TODO: ERROR
                        continue;
                    }
                    else
                    {
                        var = &localTrie.at(buffer[idH].val.c_str()); // TODO
                    }
                }
                else
                {
                    var = &localTrie.at(buffer[idH].val.c_str()); // TODO
                }
                if (var->type != VarType::NONE)
                {
                    // TODO: ERROR
                    continue;
                }
                else
                {
                    var->type = VarType::INT;
                    var->writable = false;

                    // TODO
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
                nextWord();

                Var* var;
                if (global)
                {
                    if (funTrie.find(buffer[idH].val.c_str()).returnType != VarType::NONE)
                    {
                        // TODO: ERROR
                        continue;
                    }
                    else
                    {
                        var = &localTrie.at(buffer[idH].val.c_str()); // TODO
                    }
                }
                else
                {
                    var = &localTrie.at(buffer[idH].val.c_str()); // TODO
                }
                if (var->type != VarType::NONE)
                {
                    // TODO: ERROR
                    continue;
                }
                else
                {
                    var->type = VarType::CHAR;
                    var->writable = false;

                    // TODO
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

    void RecursiveParser::integer()
    {
        if (buffer[h].type == WordType::PLUS)
        {
            nextWord();
            uinteger();
        }
        else if (buffer[h].type == WordType::MINU)
        {
            nextWord();
            uinteger();
        }
        else if (buffer[h].type == WordType::INTCON)
        {
            uinteger();
        }
        else
        {
            // TODO: ERROR
        }

        print("<整数>\n");
    }

    Fun* RecursiveParser::declareHead()
    {
        Fun* fun = nullptr;
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
        if (type != VarType::NONE)
        {
            nextWord();
            if (buffer[h].type != WordType::IDENFR)
            {
                // TODO: ERROR
            }

            if (globalTrie.find(buffer[h].val.c_str()).type != VarType::NONE)
            {
                // TODO: ERROR
            }
            fun = &funTrie.at(buffer[h].val.c_str());
            if (fun->returnType != VarType::NONE)
            {
                // TODO: ERROR
            }
            fun->returnType = type;

            nextWord();
        }

        print("<声明头部>\n");

        return fun;
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

            Var* var;
            if (global)
            {
                if (funTrie.find(buffer[idH].val.c_str()).returnType != VarType::NONE)
                {
                    // TODO: ERROR
                    continue;
                }
                else
                {
                    var = &localTrie.at(buffer[idH].val.c_str()); // TODO
                }
            }
            else
            {
                var = &localTrie.at(buffer[idH].val.c_str()); // TODO
            }
            if (var->type != VarType::NONE)
            {
                // TODO: ERROR
                continue;
            }
            else
            {
                var->size = size;
                var->type = type;

                // TODO
            }

            // TODO
        } while (buffer[h].type == WordType::COMMA);

        print("<变量定义>\n");
    }

    void RecursiveParser::funDef()
    {
        /*Fun* fun =*/ declareHead();
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

        if (globalTrie.find(buffer[h].val.c_str()).type != VarType::NONE)
        {
            // TODO: ERROR
        }
        Fun& fun = funTrie.at(buffer[h].val.c_str());
        if (fun.returnType != VarType::NONE)
        {
            // TODO: ERROR
        }
        fun.returnType = VarType::VOID;

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
        if (buffer[h].type == WordType::INTTK || buffer[h].type == WordType::CHARTK)
        {
            nextWord();
            if (buffer[h].type != WordType::IDENFR)
            {
                // TODO: ERROR
            }
            nextWord();
            while (buffer[h].type == WordType::COMMA)
            {
                nextWord();
                if (buffer[h].type != WordType::INTTK && buffer[h].type != WordType::CHARTK)
                {
                    // TODO: ERROR
                }
                nextWord();
                if (buffer[h].type != WordType::IDENFR)
                {
                    // TODO: ERROR
                }
                nextWord();
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
        }
        else if (buffer[h].type == WordType::MINU)
        {
            nextWord();
        }
        item();
        while (EXPRESSION_SELECT[static_cast<unsigned>(buffer[h].type)])
        {
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
            item();
        }

        print("<表达式>\n");
    }

    void RecursiveParser::item()
    {
        factor();
        while (buffer[h].type == WordType::MULT || buffer[h].type == WordType::DIV)
        {
            nextWord();
            factor();
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
            integer();
            break;

        case WordType::CHARCON:
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
                if (funTrie.find(buffer[h].val.c_str()).returnType == VarType::VOID)
                {
                    voidFunCall();
                }
                else
                {
                    funCall();
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
        statement();
        if (buffer[h].type == WordType::ELSETK)
        {
            nextWord();
            statement();
        }

        print("<条件语句>\n");
    }

    void RecursiveParser::condition()
    {
        expression();
        switch (buffer[h].type)
        {
        case WordType::LSS:
            nextWord();
            expression();
            break;

        case WordType::LEQ:
            nextWord();
            expression();
            break;

        case WordType::GRE:
            nextWord();
            expression();
            break;

        case WordType::GEQ:
            nextWord();
            expression();
            break;

        case WordType::EQL:
            nextWord();
            expression();
            break;

        case WordType::NEQ:
            nextWord();
            expression();
            break;

        default:
            break;
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
            condition();
            if (buffer[h].type != WordType::RPARENT)
            {
                // TODO: ERROR
            }
            nextWord();
            statement();
        }
        else if (buffer[h].type == WordType::DOTK)
        {
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
            condition();
            if (buffer[h].type != WordType::RPARENT)
            {
                // TODO: ERROR
            }
            nextWord();
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

    void RecursiveParser::step()
    {
        uinteger();

        print("<步长>\n");
    }

    void RecursiveParser::funCall()
    {
        if (buffer[h].type != WordType::IDENFR)
        {
            // TODO: ERROR
        }
        nextWord();
        if (buffer[h].type != WordType::LPARENT)
        {
            // TODO: ERROR
        }
        nextWord();
        paramVal();
        if (buffer[h].type != WordType::RPARENT)
        {
            // TODO: ERROR
        }
        nextWord();

        print("<有返回值函数调用语句>\n");
    }

    void RecursiveParser::voidFunCall()
    {
        if (buffer[h].type != WordType::IDENFR)
        {
            // TODO: ERROR
        }
        nextWord();
        if (buffer[h].type != WordType::LPARENT)
        {
            // TODO: ERROR
        }
        nextWord();
        paramVal();
        if (buffer[h].type != WordType::RPARENT)
        {
            // TODO: ERROR
        }
        nextWord();

        print("<无返回值函数调用语句>\n");
    }

    void RecursiveParser::paramVal()
    {
        if (EXPRESSION_SELECT[static_cast<unsigned>(buffer[h].type)])
        {
            expression();
            while (buffer[h].type == WordType::COMMA)
            {
                nextWord();
                expression();
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
            str();
            if (buffer[h].type == WordType::COMMA)
            {
                nextWord();
                expression();
            }
        }
        else if (EXPRESSION_SELECT[static_cast<unsigned>(buffer[h].type)])
        {
            expression();
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
