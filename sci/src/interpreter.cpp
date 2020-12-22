#include <cstdio>
#include <cstring>

#include "../../common/src/bpcode.h"
#include "../../common/src/exception.h"
#include "interpreter.h"

namespace sci
{
    // class Interpreter

    Interpreter::Interpreter() : ip(-1), top(-1), sp(0)
    {
    }

    // class BInterpreter

    BInterpreter::BInterpreter() : codes(nullptr), del(false)
    {
    }

    void BInterpreter::set(BPcode* codes)
    {
        this->codes = codes;
        del = false;
    }

    void BInterpreter::read(const char* fileName)
    {
        FILE* fp;
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
            throw FileError(fileName, "input");
        }
        del = true;
        // TODO
    }

    void BInterpreter::run()
    {
        while (true)
        {
            switch (codes[++ip].f)
            {
            case 0000:
                top -= codes[ip].a;
                continue;

            case 0010:
                if (++top < static_cast<int>(st.size()))
                {
                    st[top] = codes[ip].a;
                    continue;
                }
                else
                {
                    st.push_back(codes[ip].a);
                    continue;
                }

            case 0020:
                if (++top < static_cast<int>(st.size()))
                {
                    st[top] = st[sp + codes[ip].a];
                    continue;
                }
                else
                {
                    st.push_back(st[sp + codes[ip].a]);
                    continue;
                }

            case 0021:
                if (++top < static_cast<int>(st.size()))
                {
                    st[top] = st[codes[ip].a];
                    continue;
                }
                else
                {
                    st.push_back(st[codes[ip].a]);
                    continue;
                }

            case 0030:
                st[sp + codes[ip].a] = st[top--];
                continue;

            case 0031:
                st[codes[ip].a] = st[top--];
                continue;

            case 0032:
                st[sp + codes[ip].a] = st[top];
                continue;

            case 0033:
                st[codes[ip].a] = st[top];
                continue;

            case 0040:
                if ((top += 2) >= static_cast<int>(st.size()))
                {
                    st.resize(top + 1);
                }
                st[top] = ip;
                st[top - 1] = sp;
                ip = codes[ip].a - 1;
                sp = top - 1;
                continue;

            case 0042:
                if ((top += 3) >= static_cast<int>(st.size()))
                {
                    st.resize(top + 1);
                }
                st[top] = ip;
                st[top - 1] = sp;
                ip = codes[ip].a - 1;
                sp = top - 1;
                continue;

            case 0050:
                if ((top += codes[ip].a) >= static_cast<int>(st.size()))
                {
                    st.resize(top + 1);
                }
                continue;

            case 0060:
                ip = codes[ip].a - 1;
                continue;

            case 0070:
                if (!st[top--])
                {
                    ip = codes[ip].a - 1;
                }
                continue;

            case 0100:
                switch (codes[ip].a)
                {
                case 0:
                    top = sp - 1;
                    ip = st[sp + 1];
                    sp = st[sp];
                    if (sp == 0)
                    {
                        return;
                    }
                    continue;

                case 1:
                    st[top] = -st[top];
                    continue;

                case 2:
                    st[top - 1] += st[top];
                    --top;
                    continue;

                case 3:
                    st[top - 1] -= st[top];
                    --top;
                    continue;

                case 4:
                    st[top - 1] *= st[top];
                    --top;
                    continue;

                case 5:
                    st[top - 1] /= st[top];
                    --top;
                    continue;

                case 6:
                    st[top] &= 1;
                    continue;

                case 7:
                    printf("%s", reinterpret_cast<char*>(st.data() + st[top--]));
                    continue;

                case 8:
                    --top;
                    st[top] = st[top] < st[top + 1];
                    continue;

                case 9:
                    --top;
                    st[top] = st[top] <= st[top + 1];
                    continue;

                case 10:
                    --top;
                    st[top] = st[top] > st[top + 1];
                    continue;

                case 11:
                    --top;
                    st[top] = st[top] >= st[top + 1];
                    continue;

                case 12:
                    --top;
                    st[top] = st[top] == st[top + 1];
                    continue;

                case 13:
                    --top;
                    st[top] = st[top] != st[top + 1];
                    continue;

                case 14:
                    printf("%d", st[top--]); // TODO: Optimize
                    continue;

                case 15:
                    puts("");
                    continue;

                case 16:
                    if (++top >= static_cast<int>(st.size()))
                    {
                        st.resize(top + 1);
                    }
                    scanf("%d", st.data() + top);
                    while (getchar() != '\n');
                    continue;

                case 17:
                    if (++top >= static_cast<int>(st.size()))
                    {
                        st.resize(top + 1);
                    }
                    st[top] = getchar();
                    while (getchar() != '\n');
                    continue;

                default:
                    throw InstuctionError(""); // TODO
                    continue;
                };
                continue;

            case 0110:
                st[top] = st[sp + codes[ip].a + st[top]];
                break;

            case 0111:
                st[top] = st[codes[ip].a + st[top]];
                break;

            case 0120:
                st[sp + codes[ip].a + st[top - 1]] = st[top]; // TODO: Optimize
                top -= 2;
                break;

            case 0121:
                st[codes[ip].a + st[top - 1]] = st[top]; // TODO: Optimize
                top -= 2;
                break;

            default:
                throw InstuctionError(""); // TODO
                continue;
            }
        }

    }

    BInterpreter::~BInterpreter()
    {
        if (del)
        {
            delete[] codes;
        }
    }

} // namespace sci
