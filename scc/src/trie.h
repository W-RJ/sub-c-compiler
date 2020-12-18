#pragma once

#ifndef _SCC_TRIE_H_
#define _SCC_TRIE_H_

#include <cstdio>

#include <vector>

namespace scc
{
    template<class T, wchar_t _KEY_L = wchar_t(32), wchar_t _KEY_R = wchar_t(126)>
    class Trie
    {
    public:

        struct Node
        {
            int son[unsigned(_KEY_R - _KEY_L + 1)];
            T data;

            Node();
        };

        static const wchar_t KEY_L = _KEY_L;
        static const wchar_t KEY_R = _KEY_R;
        static const T DEFAULT_VAL = T();

        std::vector<Node> nodes;

        Trie();

        virtual ~Trie();

        T find(const wchar_t* key);

        T findSafe(const wchar_t* key);

        void print(FILE* fp);

        // TODO
    };
}

#endif // _SCC_TRIE_H_
