#pragma once

#ifndef _SCC_TRIE_TCC_
#define _SCC_TRIE_TCC_

#include <cstdio>

#include "trie.h"
#include "../../common/src/exception.h"

namespace scc
{
    // class Trie

    template<class T, char KEY_L, char KEY_R>
    Trie<T, KEY_L, KEY_R>::Node::Node() : son{0}, data()
    {
    }

    template<class T, char KEY_L, char KEY_R>
    Trie<T, KEY_L, KEY_R>::Trie() : DEFAULT_VAL()
    {
        nodes.emplace_back();
        // TODO
    }

    template<class T, char KEY_L, char KEY_R>
    Trie<T, KEY_L, KEY_R>::~Trie()
    {
        // TODO
    }

    template<class T, char KEY_L, char KEY_R>
    T& Trie<T, KEY_L, KEY_R>::at(const char* key)
    {
        int p = 0;
        while (*key != L'\0')
        {
            if (*key < KEY_L || *key > KEY_R)
            {
                throw OutOfRangeError("Trie::at: key out of range");
            }
            if (nodes[p].son[*key - KEY_L] == 0)
            {
                nodes[p].son[*key - KEY_L] = nodes.size();
                nodes.emplace_back();
            }
            p = nodes[p].son[*key - KEY_L];
            key++;
        }
        return nodes[p].data;
    }

    template<class T, char KEY_L, char KEY_R>
    T& Trie<T, KEY_L, KEY_R>::get(const char* key)
    {
        int p = 0;
        while (*key != L'\0')
        {
            p = nodes[p].son[*key - KEY_L];
            key++;
        }
        return nodes[p].data;
    }

    template<class T, char KEY_L, char KEY_R>
    const T& Trie<T, KEY_L, KEY_R>::find(const char* key)
    {
        int p = 0;
        while (*key != L'\0')
        {
            if (*key < KEY_L || *key > KEY_R)
            {
                return DEFAULT_VAL;
            }
            p = nodes[p].son[*key - KEY_L];
            key++;
            if (p == 0)
            {
                return DEFAULT_VAL;
            }
        }
        return nodes[p].data;
    }

    template<class T, char KEY_L, char KEY_R>
    void Trie<T, KEY_L, KEY_R>::clear()
    {
        nodes.clear();
        nodes.emplace_back();
    }

    template<class T, char KEY_L, char KEY_R>
    void Trie<T, KEY_L, KEY_R>::print(FILE* fp) const
    {
        int n = nodes.size();
        for (int i = 0; i < n; i++)
        {
            fprintf(fp, "%d<%d>: ", i, static_cast<int>(nodes[i].data));
            for (char c = KEY_L; c <= KEY_R; c++)
            {
                if (nodes[i].son[c - KEY_L] != 0)
                {
                    fprintf(fp, "%c:%d ", c, nodes[i].son[c - KEY_L]);
                }
            }
            fprintf(fp, "\n");
        }
        // TODO
    }
}

#endif // _SCC_TRIE_TCC_
