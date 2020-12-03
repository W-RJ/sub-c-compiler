#pragma once

#ifndef _SCC_LEXER_H_
#define _SCC_LEXER_H_

#include <cstdio>

namespace scc
{
    enum WordType
    {
        #include "sc.lang"
    };

    struct Word
    {
        WordType type = WordType::NONE;
        const char *val = NULL;
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
         * @return next word. type == NONE if fail
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
