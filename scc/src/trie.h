#pragma once

#ifndef _SCC_TRIE_H_
#define _SCC_TRIE_H_

#include <cstdio>

#include <vector>

namespace scc
{
    template<class T, wchar_t _KEY_L = static_cast<wchar_t>(32), wchar_t _KEY_R = static_cast<wchar_t>(126)>
    class Trie
    {
    public:

        struct Node
        {
            int son[static_cast<unsigned>(_KEY_R - _KEY_L + 1)];
            T data;

            Node();
        };

        static const wchar_t KEY_L = _KEY_L;
        static const wchar_t KEY_R = _KEY_R;
        const T DEFAULT_VAL;

        std::vector<Node> nodes;

        Trie();

        virtual ~Trie();

        T& at(const wchar_t* key);

        T& get(const wchar_t* key);

        const T& find(const wchar_t* key);

        void clear();

        void print(FILE* fp) const;

        // TODO
    };
}

#endif // _SCC_TRIE_H_
