#pragma once

#ifndef _SCC_REGEXP_H_
#define _SCC_REGEXP_H_

#include <cstdio>

namespace scc
{
    class RegExp
    {
    protected:

        virtual wchar_t nextChar() = 0;

    public:

        virtual ~RegExp();

        void analyze(); // TODO: Trie
    };
}

#endif // _SCC_REGEXP_H_
