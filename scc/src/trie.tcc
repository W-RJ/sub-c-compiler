#pragma once

#ifndef _SCC_TRIE_TCC_
#define _SCC_TRIE_TCC_

#include <cstdio>

#include "trie.h"

namespace scc
{
    // class Trie

    template<class T, T DEFAULT_VAL, wchar_t KEY_L, wchar_t KEY_R>
    Trie<T, DEFAULT_VAL, KEY_L, KEY_R>::Trie()
    {
        nodes.push_back(Node{});
        // TODO
    }

    template<class T, T DEFAULT_VAL, wchar_t KEY_L, wchar_t KEY_R>
    Trie<T, DEFAULT_VAL, KEY_L, KEY_R>::~Trie()
    {
        // TODO
    }

    template<class T, T DEFAULT_VAL, wchar_t KEY_L, wchar_t KEY_R>
    void Trie<T, DEFAULT_VAL, KEY_L, KEY_R>::print(FILE* fp)
    {
        int n = nodes.size();
        for (int i = 0; i < n; i++)
        {
            fwprintf(fp, L"%d<%d>: ", i, int(nodes[i].data));
            for (wchar_t c = KEY_L; c <= KEY_R; c++)
            {
                if (nodes[i].son[c - KEY_L] != 0)
                {
                    fwprintf(fp, L"%lc:%d ", c, nodes[i].son[c - KEY_L]);
                }
            }
            fwprintf(fp, L"\n");
        }
        // TODO
    }
}

#endif // _SCC_TRIE_TCC_
