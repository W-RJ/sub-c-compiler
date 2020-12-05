#include <cstdio>
#include <cwchar>

#include "lexer.h"
#include "exception.h"

namespace scc
{
    wchar_t typeName[int(WordType::END)][TYPE_NAME_MAX] = {{'\0'}};

    // class LangReader

    LangReader::LangReader() : fp(nullptr)
    {
    }

    LangReader::~LangReader()
    {
        if (fp != nullptr)
        {
            fclose(fp);
        }
    }

    void LangReader::open(const char* fileName)
    {
        fp = fopen(fileName, "r");
        if (fp == nullptr)
        {
            throw NoSuchFileError(L"Unable to open input file");
        }
    }

    void LangReader::close()
    {
        if (fp != nullptr)
        {
            fclose(fp);
            fp = nullptr;
        }
    }

    wchar_t LangReader::nextChar()
    {
        if (fp == nullptr)
        {
            return wchar_t(EOF);
        }
        else
        {
            return fgetwc(fp);
        }
    }

    void LangReader::read(bool buildTrie)
    {
        int n = int(WordType::END);
        wchar_t buffer[INT16_MAX];
        for (int i = 0; i < n; i++)
        {
            fwscanf(fp, L"%l[^,],", typeName[i]);
            if (buildTrie)
            {
                // TODO
            }
            else
            {
                fgetws(buffer, INT16_MAX, fp); // TODO
            }
        }
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
            throw NoSuchFileError(L"Unable to open input file");
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
