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
        WordType type;
        const char *val;
    };

    class Lexer
    {
    private:

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
         * @return true if succeed, false otherwise
         */
        bool open(const char *fileName);

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
}

#endif // _SCC_LEXER_H_
