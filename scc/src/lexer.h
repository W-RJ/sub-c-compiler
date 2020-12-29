#pragma once

#ifndef _SCC_LEXER_H_
#define _SCC_LEXER_H_

#include <cstdio>
#include <string>

#include "regexp"

namespace scc
{
    enum class WordType
    {
        #include "sc.lang"
    };

    extern char typeName[static_cast<unsigned>(WordType::END)][TYPE_NAME_MAX];

    extern Trie<WordType> lexTrie;

    struct Word
    {
        int row;
        WordType type = WordType::NONE;
        std::string val;
    };

    void readLang(const char* fileName, bool buildTrie);

    /**
     * Abstract base class of lexers used for lexical analysis
     */
    class Lexer
    {
    protected:

        // File Pointer
        FILE *fp;

        const char* fileName;

        int row;

        char ch;

    public:

        Lexer();

        virtual ~Lexer();

        /**
         * Open source file
         * 
         * @param filename: name of source file
         * 
         * @exception throw FileError if fail
         */
        void open(const char *fileName);

        /**
         * Close source file
         */
        void close();

        const char* getFileName();

        /**
         * Get next word
         * 
         * @param word: next word. type == WordType::NONE if fail
         */
        virtual void nextWord(Word& word) = 0; // NOTE

    };

    /**
     * Lexical analyzer with Trie
     */
    class TrieLexer : public Lexer
    {
    public:
        virtual void nextWord(Word& word) override;
    };

    /**
     * Lexical analyzer with DFA
     */
    class DFALexer : public Lexer
    {
    private:

        bool isAlpha(char ch);

        bool isDigit(char ch);

    public:

        virtual void nextWord(Word& word) override;

    };
}

#endif // _SCC_LEXER_H_
