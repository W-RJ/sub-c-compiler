/*
    Regexp analyzer & Trie generator of SCC.
    Copyright (C) 2020-2021 Renjian Wang

    This file is part of SCC.

    SCC is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SCC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SCC.  If not, see <https://www.gnu.org/licenses/>.
*/

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
