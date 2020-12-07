#pragma once

#ifndef _SCC_REGEXP_TCC_
#define _SCC_REGEXP_TCC_

#include "regexp"
#include "trie"

#include "exception.h"

namespace scc
{
    // class RegExp

    template<class T>
    RegExp<T>::RegExp(Trie<T>& trie) : trie(trie)
    {
    }

    template<class T>
    RegExp<T>::~RegExp() = default;

    template<class T>
    wchar_t RegExp<T>::nextChar()
    {
        return *reg++;
    }

    template<class T>
    void RegExp<T>::analyze(const wchar_t* reg, const T& val)
    {
        this->reg = reg;
        this->val = val;
        ch = nextChar();
        if (ch >= trie.KEY_L && ch <= trie.KEY_R && ch != L']' && ch != L'*' && ch != L'|')
        {
            expr();
            analyzeTail();
        }
        else if (ch == L']' || ch == L'*' || ch > trie.KEY_R)
        {
            throw RegExpError(L"Unexpected character", ch);
        }
        else
        {
            throw RegExpError(L"RegExp end unexpectedly", ch);
        }

        // TODO
    }

    template<class T>
    void RegExp<T>::analyzeTail()
    {
        if (ch == L'|')
        {
            ch = nextChar();
            expr();
            analyzeTail();
        }
        else if (ch >= trie.KEY_L)
        {
            throw RegExpError(L"Unexpected character", ch);
        }
    }

    template<class T>
    void RegExp<T>::expr()
    {
        if (ch >= trie.KEY_L && ch <= trie.KEY_R && ch != L']' && ch != L'*' && ch != L'|')
        {
            std::vector<int>* heads = new std::vector<int>;
            heads->push_back(0);
            item(heads);
            exprTail(heads);
            for (auto it = heads->begin(); it != heads->end(); ++it)
            {
                if (trie.nodes[*it].data == trie.DEFAULT_VAL)
                {
                    trie.nodes[*it].data = val;
                }
            }
        }
        else if (ch == L']' || ch == L'*' || ch > trie.KEY_R)
        {
            throw RegExpError(L"Unexpected character", ch);
        }
        else
        {
            throw RegExpError(L"RegExp end unexpectedly", ch);
        }
    }

    template<class T>
    void RegExp<T>::exprTail(std::vector<int>*& heads)
    {
        if (ch >= trie.KEY_L && ch <= trie.KEY_R && ch != L']' && ch != L'*' && ch != L'|')
        {
            item(heads);
            exprTail(heads);
        }
        else if (!(ch < trie.KEY_L || ch == L'|'))
        {
            throw RegExpError(L"Unexpected character", ch);
        }
    }

    template<class T>
    void RegExp<T>::item(std::vector<int>*& heads)
    {
        if (ch == L'[')
        {
            ch = nextChar();
            std::vector< std::pair<wchar_t, wchar_t> > ranges;
            opt(ranges);
            if (ch != L']')
            {
                throw RegExpError(L"Unexpected character", ch);
            }
            ch = nextChar();
            bool isStar = star();
            bool newNode = false;

            if (!isStar)
            {
                std::vector<int>* newHeads = new std::vector<int>;
                for (auto itHeads = heads->begin(); itHeads != heads->end(); ++itHeads)
                {
                    for (auto itRanges = ranges.begin(); itRanges != ranges.end(); ++itRanges)
                    {
                        for (wchar_t c = itRanges->first; c <= itRanges->second; ++c)
                        {
                            if (trie.nodes[*itHeads].son[c - trie.KEY_L] == 0)
                            {
                                if (!newNode)
                                {
                                    newNode = true;
                                    trie.nodes.push_back(typename Trie<T>::Node{});
                                }
                                trie.nodes[*itHeads].son[c - trie.KEY_L] = trie.nodes.size() - 1;
                            }
                            newHeads->push_back(trie.nodes[*itHeads].son[c - trie.KEY_L]);
                        }
                    }
                }
                delete heads;
                heads = newHeads;
            }
            else
            {
                std::vector<int>* newHeads = new std::vector<int>(*heads);
                bool vis[trie.nodes.size() + 1] = {false};
                while (!heads->empty())
                {
                    std::vector<int>* curHeads = new std::vector<int>;
                    for (auto itHeads = heads->begin(); itHeads != heads->end(); ++itHeads)
                    {
                        vis[*itHeads] = true;
                        for (auto itRanges = ranges.begin(); itRanges != ranges.end(); ++itRanges)
                        {
                            for (wchar_t c = itRanges->first; c <= itRanges->second; ++c)
                            {
                                if (trie.nodes[*itHeads].son[c - trie.KEY_L] == 0)
                                {
                                    if (!newNode)
                                    {
                                        newNode = true;
                                        trie.nodes.push_back(typename Trie<T>::Node{});
                                    }
                                    trie.nodes[*itHeads].son[c - trie.KEY_L] = trie.nodes.size() - 1;
                                }
                                if (!vis[trie.nodes[*itHeads].son[c - trie.KEY_L]])
                                {
                                    vis[trie.nodes[*itHeads].son[c - trie.KEY_L]] = true;
                                    newHeads->push_back(trie.nodes[*itHeads].son[c - trie.KEY_L]);
                                    curHeads->push_back(trie.nodes[*itHeads].son[c - trie.KEY_L]);
                                }
                            }
                        }
                    }
                    delete heads;
                    heads = curHeads;
                }
                delete heads;
                heads = newHeads;
            }
 
            // TODO
        }
        else if (ch >= trie.KEY_L && ch <= trie.KEY_R && ch != L']' && ch != L'*' && ch != L'|')
        {
            wchar_t c = itemChar();
            for (auto itHeads = heads->begin(); itHeads != heads->end(); ++itHeads)
            {
                if (trie.nodes[*itHeads].son[c - trie.KEY_L] == 0)
                {
                    trie.nodes.push_back(typename Trie<T>::Node{});
                    trie.nodes[*itHeads].son[c - trie.KEY_L] = trie.nodes.size() - 1;
                }
                *itHeads = trie.nodes[*itHeads].son[c - trie.KEY_L];
            }
        }
        else if (ch == L']' || ch == L'*')
        {
            throw RegExpError(L"Unexpected character", ch);
        }
        else
        {
            throw RegExpError(L"RegExp end unexpectedly", ch);
        }
    }

    template<class T>
    bool RegExp<T>::star()
    {
        if (ch == L'*')
        {
            ch = nextChar();
            return true;
        }
        else if (ch > trie.KEY_R)
        {
            throw RegExpError(L"Unexpected character", ch);
        }
        return false;
    }

    template<class T>
    void RegExp<T>::opt(std::vector< std::pair<wchar_t, wchar_t> >& ranges)
    {
        if (ch >= trie.KEY_L && ch <= trie.KEY_R && ch != L']' && ch != L'-')
        {
            optItem(ranges);
            optTail(ranges);
        }
        else if (ch == L'-')
        {
            throw RegExpError(L"Cannot start with '-'", ch);
        }
        else if (ch > trie.KEY_R)
        {
            throw RegExpError(L"Unexpected character", ch);
        }
        else
        {
            throw RegExpError(L"RegExp end unexpectedly", ch);
        }
    }

    template<class T>
    void RegExp<T>::optTail(std::vector< std::pair<wchar_t, wchar_t> >& ranges)
    {
        if (ch >= trie.KEY_L && ch <= trie.KEY_R && ch != L']' && ch != L'-')
        {
            optItem(ranges);
            optTail(ranges);
        }
        else if (ch == ']')
        {
            return;
        }
        else if (ch == '-')
        {
            throw RegExpError(L"'-' is in the wrong place", ch);
        }
        else if (ch > trie.KEY_R)
        {
            throw RegExpError(L"Unexpected character", ch);
        }
        else
        {
            throw RegExpError(L"RegExp end unexpectedly", ch);
        }
    }

    template<class T>
    void RegExp<T>::optItem(std::vector< std::pair<wchar_t, wchar_t> >& ranges)
    {
        if (ch >= trie.KEY_L && ch <= trie.KEY_R && ch != L']' && ch != L'-')
        {
            wchar_t optCh = optChar();
            ranges.emplace_back(optCh, range(optCh));
        }
        else if (ch == '-')
        {
            throw RegExpError(L"'-' is in the wrong place", ch);
        }
        else if (ch > trie.KEY_R)
        {
            throw RegExpError(L"Unexpected character", ch);
        }
        else
        {
            throw RegExpError(L"RegExp end unexpectedly", ch);
        }
    }

    template<class T>
    wchar_t RegExp<T>::range(wchar_t optCh)
    {
        if (ch == '-')
        {
            ch = nextChar();
            return optChar();
        }
        else if (ch < trie.KEY_L)
        {
            throw RegExpError(L"RegExp end unexpectedly", ch);
        }
        else if (ch > trie.KEY_R)
        {
            throw RegExpError(L"Unexpected character", ch);
        }
        return optCh;
    }

    template<class T>
    wchar_t RegExp<T>::itemChar()
    {
        wchar_t res;
        if (ch == '\\')
        {
            res = nextChar();
            ch = nextChar();
            return res;
        }
        else if (ch >= trie.KEY_L && ch <= trie.KEY_R && ch != L'[' && ch != L']' && ch != L'*' && ch != L'|')
        {
            res = ch;
            ch = nextChar();
            return res;
        }
        else
        {
            throw RegExpError(L"Unexpected character", ch);
        }
    }

    template<class T>
    wchar_t RegExp<T>::optChar()
    {
        wchar_t res;
        if (ch == '\\')
        {
            res = nextChar();
            ch = nextChar();
            return res;
        }
        else if (ch >= trie.KEY_L && ch <= trie.KEY_R && ch != L']' && ch != L'-')
        {
            res = ch;
            ch = nextChar();
            return res;
        }
        else
        {
            throw RegExpError(L"Unexpected character", ch);
        }
    }
}

#endif // _SCC_REGEXP_TCC_
