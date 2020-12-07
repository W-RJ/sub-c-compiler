#include <cstdio>
#include <cwchar>

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
    }

    void Lexer::close()
    {
        if (fp != nullptr)
        {
            fclose(fp);
            fp = nullptr;
        }
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
