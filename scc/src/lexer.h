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

    extern wchar_t typeName[unsigned(WordType::END)][TYPE_NAME_MAX];

    extern Trie<WordType> lexTrie;

    struct Word
    {
        WordType type = WordType::NONE;
        std::wstring val;
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

        wchar_t ch;

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

        bool isAlpha(wchar_t ch);

        bool isDigit(wchar_t ch);

    public:

        virtual void nextWord(Word& word) override;

    };
}

#endif // _SCC_LEXER_H_
