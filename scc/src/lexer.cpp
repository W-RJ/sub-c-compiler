#include <cstdio>
#include <unordered_map>
#include <cwchar>
#include <cstring>
#include <string>
#include "lexer.h"
#include "exception.h"
#include <cwctype>
using std::wstring;
using std::unordered_map;
namespace scc
{
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

    wchar_t typeName[int(WordType::END)][TYPE_NAME_MAX] = {{'\0'}};

    // class LangReader

    LangReader::LangReader() : fp(nullptr)
    {
    }

    LangReader::~LangReader()
    {
        if (fp != nullptr)
        {
            fclose(fp);
        }
    }

    void LangReader::open(const char* fileName)
    {
        fp = fopen(fileName, "r");
        if (fp == nullptr)
        {
            throw NoSuchFileError(L"Unable to open input fileA");
        }
    }

    void LangReader::close()
    {
        if (fp != nullptr)
        {
            fclose(fp);
            fp = nullptr;
        }
    }

    wchar_t LangReader::nextChar()
    {
        if (fp == nullptr)
        {
            return wchar_t(EOF);
        }
        else
        {
            return fgetwc(fp);
        }
    }

    void LangReader::read(bool buildTrie)
    {
        int n = int(WordType::END);
        wchar_t buffer[100];
        for (int i = 0; i < n; i++)
        {
            fwscanf(fp, L"%l[^,],", typeName[i]);
            //wprintf(L"%ls\n",typeName[i]);
            if (buildTrie)
            {
                // TODO
            }
            else
            {
                fgetws(buffer, 100, fp); // TODO
            }
        }
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
        fp = fopen(fileName, "r");
        if (fp == nullptr)
        {
            throw NoSuchFileError(L"Unable to open input fileB");
        }
    }

    void Lexer::close()
    {
        if (fp != nullptr)
        {
            fclose(fp);
            fp = nullptr;
        }
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
