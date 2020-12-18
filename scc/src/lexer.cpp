#include <cstdio>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <cassert>

#include "lexer.h"
#include "regexp"
#include "trie"
#include "define.h"
#include "exception.h"

#ifdef CG
#include "cg.h"
#endif

namespace scc
{
    wchar_t typeName[static_cast<unsigned>(WordType::END)][TYPE_NAME_MAX] = {{'\0'}};

    Trie<WordType> lexTrie;

    void readLang(const char* fileName, bool buildTrie)
    {
        FILE* fp;
#ifdef CG
        fp = fopen(fileName, "w");
        fwprintf(fp, L"%ls", lang);
        fclose(fp);
#endif
        fp = fopen(fileName, "r");
        if (fp == nullptr)
        {
            throw FileError(fileName, L"lang");
        }
        RegExp<WordType> regExp(lexTrie);
        const int BUFFER_MAX = 256;
        wchar_t buffer[BUFFER_MAX];
        int n = static_cast<int>(WordType::END);
        for (int i = 0; i < n; i++)
        {
            fwscanf(fp, L"%l[^,],", typeName[i]);
            if (buildTrie) // TODO: none & //
            {
                if (fgetwc(fp) == '/')
                {
                    fgetwc(fp);
                    fgetws(buffer, BUFFER_MAX, fp);
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
                while (fgetwc(fp) != L'\n'); // TODO
            }
        }
        fclose(fp);
    }

    // class Lexer

    Lexer::Lexer() : fp(nullptr)
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
            throw FileError(fileName, L"input");
        }
        ch = fgetwc(fp);
    }

    void Lexer::close()
    {
        if (fp != nullptr)
        {
            fclose(fp);
            fp = nullptr;
        }
    }

    // class TrieLexer

    void TrieLexer::nextWord(Word& word)
    {
        assert(fp != nullptr);
        int p = 0;

        while (ch != wchar_t(EOF) && (ch <= ' ' || isspace(ch)))
        {
            ch = fgetwc(fp);
        }
        if (ch == wchar_t(EOF))
        {
            return;
        }

        while (true)
        {
            if (ch < lexTrie.KEY_L || ch == wchar_t(EOF)) // TODO: merge
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
            ch = fgetwc(fp);
        }

        word.type = lexTrie.nodes[p].data;
        if (word.type == WordType::CHARCON || word.type == WordType::STRCON)
        {
            word.val.pop_back();
            word.val = word.val.substr(1);
        }
    }

    // class DFALexer

    bool DFALexer::isAlpha(wchar_t ch)
    {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
    }

    bool DFALexer::isDigit(wchar_t ch)
    {
        return ch >= '0' && ch <= '9';
    }

    void DFALexer::nextWord(Word& word)
    {
        assert(fp != nullptr);

        while (ch != wchar_t(EOF) && (ch <= ' ' || isspace(ch)))
        {
            ch = fgetwc(fp);
        }
        if (ch == wchar_t(EOF))
        {
            return;
        }

        switch (ch)
        {
        case '\'':
            ch = fgetwc(fp);
            if (!(ch == L'+' || ch == L'-' || ch == L'*' || ch == L'/' || isAlpha(ch) || isDigit(ch)))
            {
                return; // TODO: ERROR
            }
            word.val.push_back(ch);
            ch = fgetwc(fp);
            if (ch != L'\'')
            {
                return; // TODO: ERROR
            }
            ch = fgetwc(fp);
            word.type = WordType::CHARCON;
            break;

        case '"':
            while ((ch = fgetwc(fp)) != L'"')
            {
                if (ch < L' ' || ch > wchar_t(126) || ch == wchar_t(EOF))
                {
                    return; // TODO: ERROR
                }
                word.val.push_back(ch);
            }
            ch = fgetwc(fp);
            word.type = WordType::STRCON;
            break;

        case '+':
            word.val.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::PLUS;
            break;

        case '-':
            word.val.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::MINU;
            break;

        case '*':
            word.val.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::MULT;
            break;

        case '/':
            word.val.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::DIV;
            break;

        case '<':
            word.val.push_back(ch);
            ch = fgetwc(fp);
            if (ch == L'=')
            {
                word.val.push_back(ch);
                ch = fgetwc(fp);
                word.type = WordType::LEQ;
            }
            else
            {
                word.type = WordType::LSS;
            }
            break;

        case '>':
            word.val.push_back(ch);
            ch = fgetwc(fp);
            if (ch == L'=')
            {
                word.val.push_back(ch);
                ch = fgetwc(fp);
                word.type = WordType::GEQ;
            }
            else
            {
                word.type = WordType::GRE;
            }
            break;

        case '=':
            word.val.push_back(ch);
            ch = fgetwc(fp);
            if (ch == L'=')
            {
                word.val.push_back(ch);
                ch = fgetwc(fp);
                word.type = WordType::EQL;
            }
            else
            {
                word.type = WordType::ASSIGN;
            }
            break;

        case '!':
            word.val.push_back(ch);
            ch = fgetwc(fp);
            if (ch != L'=')
            {
                return;
            }
            word.val.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::NEQ;
            break;

        case ';':
            word.val.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::SEMICN;
            break;

        case ',':
            word.val.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::COMMA;
            break;

        case '(':
            word.val.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::LPARENT;
            break;

        case ')':
            word.val.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::RPARENT;
            break;

        case '[':
            word.val.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::LBRACK;
            break;

        case ']':
            word.val.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::RBRACK;
            break;

        case '{':
            word.val.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::LBRACE;
            break;

        case '}':
            word.val.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::RBRACE;
            break;

        case '0': // TODO: ERROR
            word.val.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::INTCON;
            break;

        default:
            if (isDigit(ch))
            {
                do
                {
                    word.val.push_back(ch);
                    ch = fgetwc(fp);
                } while (isDigit(ch));
                word.type = WordType::INTCON;
            }
            else if (isAlpha(ch))
            {
                do
                {
                    word.val.push_back(ch);
                    ch = fgetwc(fp);
                } while (isAlpha(ch) || isDigit(ch));
                word.type = lexTrie.get(word.val.c_str());
            }
            else
            {
                // TODO: ERROR
            }
            break;
        }
    }
}
