/*
    Implementation of trie.
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

#ifndef _SCC_TRIE_H_
#define _SCC_TRIE_H_

#include <cstdio>

#include <vector>

namespace scc
{
    template<class T, char _KEY_L = static_cast<char>(32), char _KEY_R = static_cast<char>(126)>
    class Trie
    {
    public:

        struct Node
        {
            int son[static_cast<unsigned>(_KEY_R - _KEY_L + 1)];
            T data;

            Node();
        };

        static const char KEY_L = _KEY_L;
        static const char KEY_R = _KEY_R;
        const T DEFAULT_VAL;

        std::vector<Node> nodes;

        Trie();

        virtual ~Trie();

        T& at(const char* key);

        T& get(const char* key);

        const T& find(const char* key);

        void clear();

        void print(FILE* fp) const;

        // TODO
    };
}

#endif // _SCC_TRIE_H_
