#pragma once

#ifndef _SCC_REGEXP_H_
#define _SCC_REGEXP_H_

#include <cstdio>
#include <vector>
#include <utility>

#include "trie"

namespace scc
{
    template<class T>
    class RegExp
    {
    protected:

        Trie<T>& trie;

        T val;

        char ch;

        const char* reg;

        char nextChar();

        void analyzeTail();

        void expr();

        void exprTail(std::vector<int>*& heads);

        void item(std::vector<int>*& heads);

        bool star();

        void opt(std::vector< std::pair<char, char> >& ranges);

        void optTail(std::vector< std::pair<char, char> >& ranges);

        void optItem(std::vector< std::pair<char, char> >& ranges);

        char range(char optCh);

        char itemChar();

        char optChar();

    public:

        explicit RegExp(Trie<T>& trie);

        virtual ~RegExp();

        void analyze(const char* reg, const T& val); // TODO: Trie
    };
}

#endif // _SCC_REGEXP_H_
