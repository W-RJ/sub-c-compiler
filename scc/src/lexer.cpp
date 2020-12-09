#include <cstdio>
#include <unordered_map>
#include <cwchar>
#include <cstring>
#include <string>
#include <cwctype>
#include <cassert>

#include "lexer.h"
#include "regexp"
#include "trie"
#include "config.h"
#include "exception.h"
using std::wstring;
using std::unordered_map;

#ifdef CG
#include "cg.h"
#endif

namespace scc
{
    wchar_t typeName[unsigned(WordType::END)][TYPE_NAME_MAX] = {{'\0'}};

    Trie<WordType> lexTrie;

    void readLang(const char* fileName, bool buildTrie)
    {
        FILE* fp;
#ifdef CG
        fp = fopen(fileName, "w");
        fwprintf(fp, L"%ls", lang);
        fclose(fp);
#endif
        fp = fopen(fileName, "r");
        if (fp == nullptr)
        {
            throw FileError(fileName, L"lang");
        }
        RegExp<WordType> regExp(lexTrie);
        const int BUFFER_MAX = 256;
        wchar_t buffer[BUFFER_MAX];
        int n = int(WordType::END);
        for (int i = 0; i < n; i++)
        {
            fwscanf(fp, L"%l[^,],", typeName[i]);
            if (buildTrie) // TODO: none & //
            {
                if (fgetwc(fp) == '/')
                {
                    fgetwc(fp);
                    fgetws(buffer, BUFFER_MAX, fp);
                    try
                    {
                        regExp.analyze(buffer, WordType(i));
                    }
                    catch(RegExpError& e)
                    {
                        e.setTypeName(typeName[i]);
                        e.print(stderr);
                    }

                    // TODO
                }
            }
            else
            {
                while (fgetwc(fp) != L'\n'); // TODO
            }
        }
        fclose(fp);
    }

    //judge letter 
    bool IsLetter(const wchar_t x)
    {
        return (x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') || x == '_';
    }

    //judge number
    bool IsNumber(const wchar_t x)
    {
        return x >= '0' && x <= '9';
    }

    // judge singSign
    bool IsSingSign(const wchar_t ch)
    {
        return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == ';' || ch == ',' || ch == '[' || ch == ']';
    }

    // struct unordered_map
    void set_key(unordered_map<wstring,WordType> & key)
    {
        key[L"const"] = WordType::CONSTTK;
        key[L"int"] = WordType::INTTK;
        key[L"char"] = WordType::CHARTK;
        key[L"void"] = WordType::VOIDTK;
        key[L"main"] = WordType::MAINTK;
        key[L"if"] = WordType::IFTK;
        key[L"else"] = WordType::ELSETK;
        key[L"do"] = WordType::DOTK;
        key[L"while"] = WordType::WHILETK;
        key[L"for"] = WordType::FORTK;
        key[L"scanf"] = WordType::SCANFTK;
        key[L"printf"] = WordType::PRINTFTK;
        key[L"return"] = WordType::RETURNTK;
        key[L"+"] = WordType::PLUS;
        key[L"-"] = WordType::MINU;
        key[L"*"] = WordType::MULT;
        key[L"/"] = WordType::DIV;
        key[L"<"] = WordType::LSS;
        key[L"<="] = WordType::LEQ;
        key[L">"] = WordType::GRE;
        key[L">="] = WordType::GEQ;
        key[L"=="] = WordType::EQL;
        key[L"!="] = WordType::NEQ;
        key[L"="] = WordType::ASSIGN;
        key[L";"] = WordType::SEMICN;
        key[L","] = WordType::COMMA;
        key[L"("] = WordType::LPARENT;
        key[L")"] = WordType::RPARENT;
        key[L"["] = WordType::LBRACK;
        key[L"]"] = WordType::RBRACK;
        key[L"{"] = WordType::LBRACE;
        key[L"}"] = WordType::RBRACE;
    }

    // class Lexer

    Lexer::Lexer() : fp(nullptr)
    {
    }

    Lexer::~Lexer()
    {
        if (fp != nullptr)
        {
            fclose(fp);
        }
    }

    void Lexer::open(const char *fileName)
    {
        if (strcmp(fileName, "-") == 0)
        {
            fp = stdin;
        }
        else
        {
            fp = fopen(fileName, "r");
        }
        if (fp == nullptr)
        {
            throw FileError(fileName, L"input");
        }
        ch = fgetwc(fp);
    }

    void Lexer::close()
    {
        if (fp != nullptr)
        {
            fclose(fp);
            fp = nullptr;
        }
    }

    // class TrieLexer

    Word TrieLexer::nextWord()
    {
        assert(fp != nullptr);
        int p = 0;
        Word word;
        buffer.clear();
        while (ch != wchar_t(EOF) && (ch <= ' ' || isspace(ch)))
        {
            ch = fgetwc(fp);
        }
        if (ch == wchar_t(EOF))
        {
            return word;
        }
        while (true)
        {
            if (ch < lexTrie.KEY_L || ch == wchar_t(EOF)) // TODO: merge
            {
                word.type = lexTrie.nodes[p].data;
                if (word.type == WordType::CHARCON || word.type == WordType::STRCON)
                {
                    buffer.pop_back();
                    word.val = buffer.c_str() + 1;
                }
                else
                {
                    word.val = buffer.c_str();
                }
                return word; // TODO: ERROR
            }
            else if (ch > lexTrie.KEY_R)
            {
                return word; // TODO: ERROR
            }
            else if (lexTrie.nodes[p].son[ch - lexTrie.KEY_L] == 0)
            {
                word.type = lexTrie.nodes[p].data;
                if (word.type == WordType::CHARCON || word.type == WordType::STRCON)
                {
                    buffer.pop_back();
                    word.val = buffer.c_str() + 1;
                }
                else
                {
                    word.val = buffer.c_str();
                }
                return word; // TODO: ERROR
            }
            else
            {
                p = lexTrie.nodes[p].son[ch - lexTrie.KEY_L];
            }
            buffer.push_back(ch);
            ch = fgetwc(fp);
        }
        return word;
    }

    // class DFALexer

    Word DFALexer::nextWord()
    {
        Word word;
        int i=0; //control stemp lengthg
        wchar_t ctemp;
        wchar_t stemp[256];
        unordered_map<wstring,WordType> key;
        set_key(key);
        //wstring strtemp;
        if (fp != nullptr)
        {
            // TODO: DFA
            // fgetc -> fgetwc
            // char -> wchar_t
            /* yet to ignore Code annotation */
            /* yet to wirte error tips */
            while ((ctemp = fgetwc(fp)) != EOF)
            {
                while (iswspace(ctemp))
                    ctemp = fgetwc(fp);
                /* char con */
                if (ctemp == '\'') {
                    i = 0;
                    ctemp = fgetwc(fp);
                    if (!IsLetter(ctemp) && !IsNumber(ctemp)
                            && ctemp != L'+' && ctemp != L'-' && ctemp != '*' && ctemp != '/')
                    {
                        word.type = WordType::NONE;
                        return word;
                    }
                    else
                    {
                        stemp[i++] = ctemp;
                    }
                    ctemp = fgetwc(fp);
                    if(ctemp == '\'') // read '\''
                    {
                        stemp[i] = '\0';
                        word.type = WordType::CHARCON;
                        word.val = stemp;
                        return word;
                    }
                    else
                    {
                        word.type = WordType::NONE;
                        return word;
                    }
                }
                /* str con */
                else if(ctemp == '\"')
                {
                    i = 0;
                    
                    while ((ctemp = fgetwc(fp)) != EOF)
                    {
                        if (ctemp != '\"' && ctemp >= 32 && ctemp < 127)
                        {
                            stemp[i++] = ctemp;
                        }
                        else if (ctemp == '\"')
                        {
                            break;
                        }
                        else
                        {
                            word.type = WordType::NONE;
                            return word;
                        }
                    }
                    if(ctemp == '\"')
                    {
                        stemp[i] = '\0';
                        word.type = WordType::STRCON;
                        word.val = stemp;
                        return word; 
                    }
                    else
                    {
                        word.type = WordType::NONE; 
                        return word;
                    }
                }
                /* keyType & ID */
                else if (IsLetter(ctemp))
                {
                    i = 0;
                    do
                    {
                        stemp[i++] = ctemp;
                        ctemp = fgetwc(fp);
                    } while (IsLetter(ctemp) || IsNumber(ctemp));
                    ungetwc(ctemp, fp);
                    stemp[i] = '\0';
                    wstring strtemp = stemp;
                    
                    if (key.find(strtemp) != key.end())
                    {
                        word.type = key[strtemp];
                        word.val = stemp;
                    }
                    else
                    {
                        word.val = stemp;
                        word.type = WordType::IDENFR;
                    } 
                    return word;
                }
                /* INT Number */
                else if (IsNumber(ctemp))
                {
                    i = 0;
                    if (ctemp != '0')
                    {
                        do
                        {
                            stemp[i++] = ctemp;
                        } while ((ctemp = fgetwc(fp)) != EOF && IsNumber(ctemp));
                        ungetwc(ctemp, fp);
                        stemp[i] = '\0';
                        word.val = stemp;
                        word.type = WordType::INTCON;
                        return word;
                    }
                    else
                    {
                        if (IsNumber(ctemp = fgetwc(fp)))
                        {
                            ungetc(ctemp, fp);
                            word.type = WordType::NONE;
                            return word;
                        }
                        else
                        {
                            ungetc(ctemp, fp);
                            word.type = WordType::INTCON;
                            stemp[0] = '0';
                            stemp[1] = '\0';
                            word.val = stemp;
                            return word;
                        }
                    }
                }
                /* sign operator */
                else
                {   
                    if (IsSingSign(ctemp))
                    {
                        stemp[0] = ctemp;
                        stemp[1] = '\0';
                        word.val = stemp;
                        word.type = key[stemp];
                    }
                    else if (ctemp == '>' || ctemp =='<' || ctemp == '='|| ctemp =='!') //strcpy(word.type,signType[k-1]);
                    {
                        i = 0;
                        stemp[i++] = ctemp;
                        ctemp = fgetwc(fp);
                        if (ctemp == '=')
                        {
                            stemp[i++] = ctemp;
                        }
                        else
                        {
                            ungetwc(ctemp, fp); //if no '='  then back                          
                        }
                        stemp[i] = '\0';  
                        wstring strtemp = stemp;
                        if (key.find(strtemp) != key.end())   
                        {
                            word.val = stemp;
                            word.type = key[strtemp];  
                        }
                        else
                        {
                            word.type = WordType::NONE;
                        } 
                    }
                    return word;
                }
            }
        }
        return word;
    }
}
