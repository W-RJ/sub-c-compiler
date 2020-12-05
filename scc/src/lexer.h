#pragma once

#ifndef _SCC_LEXER_H_
#define _SCC_LEXER_H_

#include <cstdio>

#include "regexp.h"

namespace scc
{
    enum class WordType
    {
        #include "sc.lang"
    };

    extern wchar_t typeName[int(WordType::END)][TYPE_NAME_MAX];

    struct Word
    {
        WordType type = WordType::NONE;
        const wchar_t *val = nullptr;
    };

    class LangReader : public RegExp
    {
    private:

        FILE* fp;

    protected:

        virtual wchar_t nextChar() override;

    public:

        LangReader();

        virtual ~LangReader() override;

        void open(const char* fileName);

        void close();

        void read(bool buildTrie); // TODO: build type name?
    };

    /**
     * Abstract base class of lexers used for lexical analysis
     */
    class Lexer
    {
    protected:

        // File Pointer
        FILE *fp;

    public:

        Lexer();

        virtual ~Lexer();

        /**
         * Open source file
         * 
         * @param filename: name of source file
         * 
         * @exception throw NoSuchFileError if fail
         */
        void open(const char *fileName);

        /**
         * Close source file
         */
        void close();

        /**
         * Get next word
         * 
         * @return next word. type == WordType::NONE if fail
         */
        virtual Word nextWord() = 0; // NOTE

    };

    /**
     * Lexical analyzer with DFA
     */
    class DFALexer : public Lexer
    {
    public:
        virtual Word nextWord() override;
    };
}

#endif // _SCC_LEXER_H_
