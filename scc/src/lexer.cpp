#include <cstdio>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <cassert>

#include "lexer.h"
#include "regexp"
#include "trie"
#include "config.h"
#include "exception.h"

#ifdef CG
#include "cg.h"
#endif

namespace scc
{
    wchar_t typeName[unsigned(WordType::END)][TYPE_NAME_MAX] = {{'\0'}};

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
        int n = int(WordType::END);
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

    Word TrieLexer::nextWord()
    {
        assert(fp != nullptr);
        int p = 0;
        Word word;
        buffer.clear();

        while (ch != wchar_t(EOF) && (ch <= ' ' || isspace(ch)))
        {
            ch = fgetwc(fp);
        }
        if (ch == wchar_t(EOF))
        {
            return word;
        }

        while (true)
        {
            if (ch < lexTrie.KEY_L || ch == wchar_t(EOF)) // TODO: merge
            {
                break;
            }
            else if (ch > lexTrie.KEY_R)
            {
                return word; // TODO: ERROR
            }
            else if (lexTrie.nodes[p].son[ch - lexTrie.KEY_L] == 0)
            {
                break;
            }
            else
            {
                p = lexTrie.nodes[p].son[ch - lexTrie.KEY_L];
            }
            buffer.push_back(ch);
            ch = fgetwc(fp);
        }

        word.type = lexTrie.nodes[p].data;
        if (word.type == WordType::CHARCON || word.type == WordType::STRCON)
        {
            buffer.pop_back();
            word.val = buffer.c_str() + 1;
        }
        else
        {
            word.val = buffer.c_str();
        }
        return word;
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

    Word DFALexer::nextWord()
    {
        assert(fp != nullptr);
        Word word;
        buffer.clear();

        while (ch != wchar_t(EOF) && (ch <= ' ' || isspace(ch)))
        {
            ch = fgetwc(fp);
        }
        if (ch == wchar_t(EOF))
        {
            return word;
        }

        switch (ch)
        {
        case '\'':
            ch = fgetwc(fp);
            if (!(ch == L'+' || ch == L'-' || ch == L'*' || ch == L'/' || isAlpha(ch) || isDigit(ch)))
            {
                return word; // TODO: ERROR
            }
            buffer.push_back(ch);
            ch = fgetwc(fp);
            if (ch != L'\'')
            {
                return word; // TODO: ERROR
            }
            ch = fgetwc(fp);
            word.type = WordType::CHARCON;
            word.val = buffer.c_str();
            break;
        
        case '"':
            while ((ch = fgetwc(fp)) != L'"')
            {
                if (ch < L' ' || ch > wchar_t(126) || ch == wchar_t(EOF))
                {
                    return word; // TODO: ERROR
                }
                buffer.push_back(ch);
            }
            ch = fgetwc(fp);
            word.type = WordType::STRCON;
            word.val = buffer.c_str();
            break;
        
        case '+':
            buffer.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::PLUS;
            word.val = buffer.c_str();
            break;
        
        case '-':
            buffer.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::MINU;
            word.val = buffer.c_str();
            break;
        
        case '*':
            buffer.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::MULT;
            word.val = buffer.c_str();
            break;
        
        case '/':
            buffer.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::DIV;
            word.val = buffer.c_str();
            break;
        
        case '<':
            buffer.push_back(ch);
            ch = fgetwc(fp);
            if (ch == L'=')
            {
                buffer.push_back(ch);
                ch = fgetwc(fp);
                word.type = WordType::LEQ;
                word.val = buffer.c_str();
            }
            else
            {
                word.type = WordType::LSS;
                word.val = buffer.c_str();
            }
            break;
        
        case '>':
            buffer.push_back(ch);
            ch = fgetwc(fp);
            if (ch == L'=')
            {
                buffer.push_back(ch);
                ch = fgetwc(fp);
                word.type = WordType::GEQ;
                word.val = buffer.c_str();
            }
            else
            {
                word.type = WordType::GRE;
                word.val = buffer.c_str();
            }
            break;
        
        case '=':
            buffer.push_back(ch);
            ch = fgetwc(fp);
            if (ch == L'=')
            {
                buffer.push_back(ch);
                ch = fgetwc(fp);
                word.type = WordType::EQL;
                word.val = buffer.c_str();
            }
            else
            {
                word.type = WordType::ASSIGN;
                word.val = buffer.c_str();
            }
            break;
        
        case '!':
            buffer.push_back(ch);
            ch = fgetwc(fp);
            if (ch != L'=')
            {
                return word;
            }
            buffer.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::NEQ;
            word.val = buffer.c_str();
            break;
        
        case ';':
            buffer.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::SEMICN;
            word.val = buffer.c_str();
            break;
        
        case ',':
            buffer.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::COMMA;
            word.val = buffer.c_str();
            break;
        
        case '(':
            buffer.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::LPARENT;
            word.val = buffer.c_str();
            break;
        
        case ')':
            buffer.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::RPARENT;
            word.val = buffer.c_str();
            break;
        
        case '[':
            buffer.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::LBRACK;
            word.val = buffer.c_str();
            break;
        
        case ']':
            buffer.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::RBRACK;
            word.val = buffer.c_str();
            break;
        
        case '{':
            buffer.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::LBRACE;
            word.val = buffer.c_str();
            break;
        
        case '}':
            buffer.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::RBRACE;
            word.val = buffer.c_str();
            break;
        
        case '0': // TODO: ERROR
            buffer.push_back(ch);
            ch = fgetwc(fp);
            word.type = WordType::INTCON;
            word.val = buffer.c_str();
            break;

        default:
            if (isDigit(ch))
            {
                do
                {
                    buffer.push_back(ch);
                    ch = fgetwc(fp);
                } while (isDigit(ch));
                word.type = WordType::INTCON;
                word.val = buffer.c_str();
            }
            else if (isAlpha(ch))
            {
                do
                {
                    buffer.push_back(ch);
                    ch = fgetwc(fp);
                } while (isAlpha(ch) || isDigit(ch));
                word.val = buffer.c_str();
                word.type = lexTrie.find(word.val);
            }
            else
            {
                // TODO: ERROR
            }
            break;
        }

        return word;
    }
}
