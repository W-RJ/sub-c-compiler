/*
    TrieLexer & DFALexer & lang reader of SCC.
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
#include "regexp"
#include "trie"
#include "define.h"

#include "../../common/src/exception.h"

#include <cstdio>
#include <cstring>
#include <cassert>

#ifdef CG
#include "cg.h"
#endif

namespace scc
{
    char typeName[static_cast<unsigned>(TokenType::END)][TYPE_NAME_MAX] = {{'\0'}};

    Trie<TokenType> lexTrie;

    void readLang(const char* fileName, bool buildTrie)
    {
        FILE* fp;
#ifdef CG
        fp = fopen(fileName, "w");
        fprintf(fp, "%s", lang);
        fclose(fp);
#endif
        fp = fopen(fileName, "r");
        if (fp == nullptr)
        {
            throw FileError(fileName, "lang");
        }
        RegExp<TokenType> regExp(lexTrie);
        const int BUFFER_MAX = 256;
        char buffer[BUFFER_MAX];
        int n = static_cast<int>(TokenType::END);
        for (int i = 0; i < n; i++)
        {
            fscanf(fp, "%[^,],", typeName[i]);
            if (buildTrie) // TODO: none & //
            {
                if (fgetc(fp) == '/')
                {
                    fgetc(fp);
                    fgets(buffer, BUFFER_MAX, fp);
                    try
                    {
                        regExp.analyze(buffer, TokenType(i));
                    }
                    catch(RegExpError& e)
                    {
                        e.setTypeName(typeName[i]);
                        e.print(stderr);
                    }

                    // TODO
                }
            }
            else
            {
                while (fgetc(fp) != '\n'); // TODO
            }
        }
        fclose(fp);
    }

    // class Lexer

    Lexer::Lexer() : fp(nullptr), fileName(nullptr)
    {
    }

    Lexer::~Lexer()
    {
        if (fp != nullptr)
        {
            fclose(fp);
        }
    }

    void Lexer::open(const char *fileName)
    {
        if (strcmp(fileName, "-") == 0)
        {
            fp = stdin;
        }
        else
        {
            fp = fopen(fileName, "r");
        }
        if (fp == nullptr)
        {
            throw FileError(fileName, "input");
        }
        this->fileName = fileName;
        ch = fgetc(fp);
        row = 1;
    }

    void Lexer::close()
    {
        if (fp != nullptr)
        {
            fclose(fp);
            fp = nullptr;
        }
    }

    const char* Lexer::getFileName()
    {
        return fileName;
    }

    // class TrieLexer

    void TrieLexer::nextToken(Token& token)
    {
        assert(fp != nullptr);

        while (true)
        {
            int p = 0;

            while (ch != static_cast<char>(EOF) && (ch <= ' ' || isspace(ch)))
            {
                if (ch == '\n')
                {
                    ++row;
                }
                ch = fgetc(fp);
            }
            if (ch == static_cast<char>(EOF))
            {
                token.type = TokenType::FEOF;
                return;
            }

            token.row = row;

            while (true)
            {
                if (ch < lexTrie.KEY_L || ch == static_cast<char>(EOF)) // TODO: merge
                {
                    break;
                }
                else if (ch > lexTrie.KEY_R)
                {
                    return; // TODO: ERROR
                }
                else if (lexTrie.nodes[p].son[ch - lexTrie.KEY_L] == 0)
                {
                    break;
                }
                else
                {
                    p = lexTrie.nodes[p].son[ch - lexTrie.KEY_L];
                }
                token.val.push_back(ch);
                ch = fgetc(fp);
            }

            token.type = lexTrie.nodes[p].data;

            if (token.type == TokenType::COMMENT)
            {
                while (ch != '\n' && ch != static_cast<char>(EOF))
                {
                    ch = fgetc(fp);
                }
                ++row;
                ch = fgetc(fp);
                token.type = TokenType::NONE;
                token.val.clear();
                continue;
            }

            if (token.type == TokenType::CHARCON || token.type == TokenType::STRCON
                    || token.type == TokenType::CHARERR)
            {
                token.val.pop_back();
                token.val = token.val.substr(1);
            }

            return;
        }
    }

    // class DFALexer

    bool DFALexer::isAlpha(char ch)
    {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
    }

    bool DFALexer::isDigit(char ch)
    {
        return ch >= '0' && ch <= '9';
    }

    void DFALexer::nextToken(Token& token)
    {
        assert(fp != nullptr);

        while (true)
        {
            while (ch != static_cast<char>(EOF) && (ch <= ' ' || isspace(ch)))
            {
                if (ch == '\n')
                {
                    ++row;
                }
                ch = fgetc(fp);
            }
            if (ch == static_cast<char>(EOF))
            {
                token.type = TokenType::FEOF;
                return;
            }

            token.row = row;

            switch (ch)
            {
            case '\'':
                ch = fgetc(fp);
                if (!(ch == '+' || ch == '-' || ch == '*' || ch == '/' || isAlpha(ch) || isDigit(ch)))
                {
                    token.type = TokenType::CHARERR;
                }
                else
                {
                    token.type = TokenType::CHARCON;
                }
                token.val.push_back(ch);
                ch = fgetc(fp);
                if (ch != '\'')
                {
                    token.type = TokenType::NONE;
                    return; // TODO: ERROR
                }
                ch = fgetc(fp);
                break;

            case '"':
                while ((ch = fgetc(fp)) != '"')
                {
                    if (ch < ' ' || ch > static_cast<char>(126) || ch == static_cast<char>(EOF))
                    {
                        return; // TODO: ERROR
                    }
                    token.val.push_back(ch);
                }
                ch = fgetc(fp);
                token.type = TokenType::STRCON;
                break;

            case '+':
                token.val.push_back(ch);
                ch = fgetc(fp);
                token.type = TokenType::PLUS;
                break;

            case '-':
                token.val.push_back(ch);
                ch = fgetc(fp);
                token.type = TokenType::MINU;
                break;

            case '*':
                token.val.push_back(ch);
                ch = fgetc(fp);
                token.type = TokenType::MULT;
                break;

            case '/':
                token.val.push_back(ch);
                ch = fgetc(fp);
                if (ch == '/')
                {
                    do
                    {
                        ch = fgetc(fp);
                    } while (ch != '\n' && ch != static_cast<char>(EOF));
                    ++row;
                    ch = fgetc(fp);
                    token.type = TokenType::NONE;
                    token.val.clear();
                    continue;
                }
                token.type = TokenType::DIV;
                break;

            case '<':
                token.val.push_back(ch);
                ch = fgetc(fp);
                if (ch == '=')
                {
                    token.val.push_back(ch);
                    ch = fgetc(fp);
                    token.type = TokenType::LEQ;
                }
                else
                {
                    token.type = TokenType::LSS;
                }
                break;

            case '>':
                token.val.push_back(ch);
                ch = fgetc(fp);
                if (ch == '=')
                {
                    token.val.push_back(ch);
                    ch = fgetc(fp);
                    token.type = TokenType::GEQ;
                }
                else
                {
                    token.type = TokenType::GRE;
                }
                break;

            case '=':
                token.val.push_back(ch);
                ch = fgetc(fp);
                if (ch == '=')
                {
                    token.val.push_back(ch);
                    ch = fgetc(fp);
                    token.type = TokenType::EQL;
                }
                else
                {
                    token.type = TokenType::ASSIGN;
                }
                break;

            case '!':
                token.val.push_back(ch);
                ch = fgetc(fp);
                if (ch != '=')
                {
                    return;
                }
                token.val.push_back(ch);
                ch = fgetc(fp);
                token.type = TokenType::NEQ;
                break;

            case ';':
                token.val.push_back(ch);
                ch = fgetc(fp);
                token.type = TokenType::SEMICN;
                break;

            case ',':
                token.val.push_back(ch);
                ch = fgetc(fp);
                token.type = TokenType::COMMA;
                break;

            case '(':
                token.val.push_back(ch);
                ch = fgetc(fp);
                token.type = TokenType::LPARENT;
                break;

            case ')':
                token.val.push_back(ch);
                ch = fgetc(fp);
                token.type = TokenType::RPARENT;
                break;

            case '[':
                token.val.push_back(ch);
                ch = fgetc(fp);
                token.type = TokenType::LBRACK;
                break;

            case ']':
                token.val.push_back(ch);
                ch = fgetc(fp);
                token.type = TokenType::RBRACK;
                break;

            case '{':
                token.val.push_back(ch);
                ch = fgetc(fp);
                token.type = TokenType::LBRACE;
                break;

            case '}':
                token.val.push_back(ch);
                ch = fgetc(fp);
                token.type = TokenType::RBRACE;
                break;

            case '0': // TODO: ERROR
                token.val.push_back(ch);
                ch = fgetc(fp);
                if (isDigit(ch) || isAlpha(ch))
                {
                    token.type = TokenType::INTERR;
                    do
                    {
                        ch = fgetc(fp);

                    } while (isDigit(ch) || isAlpha(ch));
                }
                else
                {
                    token.type = TokenType::INTCON;
                }
                break;

            default:
                if (isDigit(ch))
                {
                    do
                    {
                        token.val.push_back(ch);
                        ch = fgetc(fp);

                    } while (isDigit(ch));

                    if (isAlpha(ch))
                    {
                        token.type = TokenType::INTERR;
                        do
                        {
                            ch = fgetc(fp);

                        } while (isDigit(ch) || isAlpha(ch));
                    }
                    else
                    {
                        token.type = TokenType::INTCON;
                    }
                }
                else if (isAlpha(ch))
                {
                    do
                    {
                        token.val.push_back(ch);
                        ch = fgetc(fp);
                    } while (isAlpha(ch) || isDigit(ch));
                    token.type = lexTrie.get(token.val.c_str());
                }
                else
                {
                    // TODO: ERROR
                }
                break;
            }

            return;
        }
    }
}
