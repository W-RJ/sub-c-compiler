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

        wchar_t ch;

        const wchar_t* reg;

        wchar_t nextChar();

        void analyzeTail();

        void expr();

        void exprTail(std::vector<int>*& heads);

        void item(std::vector<int>*& heads);

        bool star();

        void opt(std::vector< std::pair<wchar_t, wchar_t> >& ranges);

        void optTail(std::vector< std::pair<wchar_t, wchar_t> >& ranges);

        void optItem(std::vector< std::pair<wchar_t, wchar_t> >& ranges);

        wchar_t range(wchar_t optCh);

        wchar_t itemChar();

        wchar_t optChar();

    public:

        explicit RegExp(Trie<T>& trie);

        virtual ~RegExp();

        void analyze(const wchar_t* reg, const T& val); // TODO: Trie
    };
}

#endif // _SCC_REGEXP_H_
