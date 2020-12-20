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
