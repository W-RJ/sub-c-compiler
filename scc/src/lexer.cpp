#include <cstdio>
#include <cstring>
#include <cassert>

#include "lexer.h"
#include "regexp"
#include "trie"
#include "define.h"
#include "../../common/src/exception.h"

#ifdef CG
#include "cg.h"
#endif

namespace scc
{
    char typeName[static_cast<unsigned>(WordType::END)][TYPE_NAME_MAX] = {{'\0'}};

    Trie<WordType> lexTrie;

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
        RegExp<WordType> regExp(lexTrie);
        const int BUFFER_MAX = 256;
        char buffer[BUFFER_MAX];
        int n = static_cast<int>(WordType::END);
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
                        regExp.analyze(buffer, WordType(i));
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

    void TrieLexer::nextWord(Word& word)
    {
        assert(fp != nullptr);
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
            word.type = WordType::FEOF;
            return;
        }

        word.row = row;

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
            word.val.push_back(ch);
            ch = fgetc(fp);
        }

        word.type = lexTrie.nodes[p].data;
        if (word.type == WordType::CHARCON || word.type == WordType::STRCON || word.type == WordType::CHARERR)
        {
            word.val.pop_back();
            word.val = word.val.substr(1);
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

    void DFALexer::nextWord(Word& word)
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
                word.type = WordType::FEOF;
                return;
            }

            word.row = row;

            switch (ch)
            {
            case '\'':
                ch = fgetc(fp);
                if (!(ch == '+' || ch == '-' || ch == '*' || ch == '/' || isAlpha(ch) || isDigit(ch)))
                {
                    word.type = WordType::CHARERR;
                }
                else
                {
                    word.type = WordType::CHARCON;
                }
                word.val.push_back(ch);
                ch = fgetc(fp);
                if (ch != '\'')
                {
                    word.type = WordType::NONE;
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
                    word.val.push_back(ch);
                }
                ch = fgetc(fp);
                word.type = WordType::STRCON;
                break;

            case '+':
                word.val.push_back(ch);
                ch = fgetc(fp);
                word.type = WordType::PLUS;
                break;

            case '-':
                word.val.push_back(ch);
                ch = fgetc(fp);
                word.type = WordType::MINU;
                break;

            case '*':
                word.val.push_back(ch);
                ch = fgetc(fp);
                word.type = WordType::MULT;
                break;

            case '/':
                word.val.push_back(ch);
                ch = fgetc(fp);
                if (ch == '/')
                {
                    do
                    {
                        ch = fgetc(fp);
                    } while (ch != '\n' && ch != static_cast<char>(EOF));
                    ++row;
                    ch = fgetc(fp);
                    word.val.clear();
                    continue;
                }
                word.type = WordType::DIV;
                break;

            case '<':
                word.val.push_back(ch);
                ch = fgetc(fp);
                if (ch == '=')
                {
                    word.val.push_back(ch);
                    ch = fgetc(fp);
                    word.type = WordType::LEQ;
                }
                else
                {
                    word.type = WordType::LSS;
                }
                break;

            case '>':
                word.val.push_back(ch);
                ch = fgetc(fp);
                if (ch == '=')
                {
                    word.val.push_back(ch);
                    ch = fgetc(fp);
                    word.type = WordType::GEQ;
                }
                else
                {
                    word.type = WordType::GRE;
                }
                break;

            case '=':
                word.val.push_back(ch);
                ch = fgetc(fp);
                if (ch == '=')
                {
                    word.val.push_back(ch);
                    ch = fgetc(fp);
                    word.type = WordType::EQL;
                }
                else
                {
                    word.type = WordType::ASSIGN;
                }
                break;

            case '!':
                word.val.push_back(ch);
                ch = fgetc(fp);
                if (ch != '=')
                {
                    return;
                }
                word.val.push_back(ch);
                ch = fgetc(fp);
                word.type = WordType::NEQ;
                break;

            case ';':
                word.val.push_back(ch);
                ch = fgetc(fp);
                word.type = WordType::SEMICN;
                break;

            case ',':
                word.val.push_back(ch);
                ch = fgetc(fp);
                word.type = WordType::COMMA;
                break;

            case '(':
                word.val.push_back(ch);
                ch = fgetc(fp);
                word.type = WordType::LPARENT;
                break;

            case ')':
                word.val.push_back(ch);
                ch = fgetc(fp);
                word.type = WordType::RPARENT;
                break;

            case '[':
                word.val.push_back(ch);
                ch = fgetc(fp);
                word.type = WordType::LBRACK;
                break;

            case ']':
                word.val.push_back(ch);
                ch = fgetc(fp);
                word.type = WordType::RBRACK;
                break;

            case '{':
                word.val.push_back(ch);
                ch = fgetc(fp);
                word.type = WordType::LBRACE;
                break;

            case '}':
                word.val.push_back(ch);
                ch = fgetc(fp);
                word.type = WordType::RBRACE;
                break;

            case '0': // TODO: ERROR
                word.val.push_back(ch);
                ch = fgetc(fp);
                if (isDigit(ch) || isAlpha(ch))
                {
                    word.type = WordType::INTERR;
                    do
                    {
                        ch = fgetc(fp);

                    } while (isDigit(ch) || isAlpha(ch));
                }
                else
                {
                    word.type = WordType::INTCON;
                }
                break;

            default:
                if (isDigit(ch))
                {
                    do
                    {
                        word.val.push_back(ch);
                        ch = fgetc(fp);

                    } while (isDigit(ch));

                    if (isAlpha(ch))
                    {
                        word.type = WordType::INTERR;
                        do
                        {
                            ch = fgetc(fp);

                        } while (isDigit(ch) || isAlpha(ch));
                    }
                    else
                    {
                        word.type = WordType::INTCON;
                    }
                }
                else if (isAlpha(ch))
                {
                    do
                    {
                        word.val.push_back(ch);
                        ch = fgetc(fp);
                    } while (isAlpha(ch) || isDigit(ch));
                    word.type = lexTrie.get(word.val.c_str());
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
