#include <cstdio>
#include <cwchar>
#include <cwctype>

#include "lexer.h"
#include "regexp"
#include "trie"
#include "exception.h"

namespace scc
{
    wchar_t typeName[unsigned(WordType::END)][TYPE_NAME_MAX] = {{'\0'}};

    Trie<WordType> lexTrie;

    void readLang(const char* fileName, bool buildTrie)
    {
        FILE* fp = fopen(fileName, "r");
        if (fp == nullptr)
        {
            throw NoSuchFileError(fileName, L"lang");
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
        fp = fopen(fileName, "r");
        if (fp == nullptr)
        {
            throw NoSuchFileError(fileName, L"input");
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
        Word word;
        buffer.clear();
        int p = 0;
        if (fp != nullptr)
        {
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
                    return word; // TODO: ERROR
                }
                else if (ch > lexTrie.KEY_R)
                {
                    return word; // TODO: ERROR
                }
                else if (lexTrie.nodes[p].son[ch - lexTrie.KEY_L] == 0)
                {
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
                    return word; // TODO: ERROR
                }
                else
                {
                    p = lexTrie.nodes[p].son[ch - lexTrie.KEY_L];
                }
                buffer.push_back(ch);
                ch = fgetwc(fp);
            }
        }
        return word;
    }

    // class DFALexer

    Word DFALexer::nextWord()
    {
        Word word;
        if (fp != nullptr)
        {
            // TODO: DFA
            // fgetc -> fgetwc
            // char -> wchar_t
        }
        return word;
    }
}
