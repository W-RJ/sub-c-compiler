#pragma once

#ifndef _SCC_TRIE_TCC_
#define _SCC_TRIE_TCC_

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
}

#endif // _SCC_TRIE_TCC_
