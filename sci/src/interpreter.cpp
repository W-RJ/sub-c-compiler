/*
    Binary interpreter & Textual interpreter of SC Interpreter.
    Copyright (C) 2020-2021 Renjian Wang

    This file is part of SCC.

    SCC is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SCC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SCC.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <cstdio>
#include <cstring>

#include "interpreter.h"

#include "../../common/src/pcode.h"
#include "../../common/src/exception.h"

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

    int BInterpreter::nextBlock(FILE *fp, BPcodeBlockType type)
    {
        BPcodeBlockType curType;
        int size;
        while (true)
        {
            size = -1;
            fread(&curType, sizeof(BPcodeBlockType), 1, fp);
            fread(&size, sizeof(int), 1, fp);
            if (curType == type || size < 0)
            {
                return size;
            }
            fseek(fp, size * sizeof(int), SEEK_CUR);
        }
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
            fp = fopen(fileName, "rb");
        }
        if (fp == nullptr)
        {
            throw FileError(fileName, "input");
        }

        char prefix[sizeof(BPCODE_PREFIX)];
        fread(prefix, sizeof(BPCODE_PREFIX), 1, fp);
        if (memcmp(prefix, BPCODE_PREFIX, sizeof(BPCODE_PREFIX)) != 0)
        {
            fclose(fp);
            throw InvalidFormatError(fileName, "binary pcode");
        }

        // version
        unsigned version;
        fread(&version, sizeof(unsigned), 1, fp);
        if (version > BPCODE_VERSION)
        {
            fclose(fp);
            throw InvalidFormatError(fileName, "binary pcode"); // TODO
        }

        fread(&version, sizeof(unsigned), 1, fp);

        // TODO: verify

        int size, globalSize;

        // general
        if ((size = nextBlock(fp, BPcodeBlockType::GENERAL)) < 0)
        {
            fclose(fp);
            throw InvalidFormatError(fileName, "binary pcode"); // TODO
        }
        fread(&globalSize, sizeof(int), 1, fp);
        fseek(fp, (size - 1) * sizeof(int), SEEK_CUR);

        // str
        size = nextBlock(fp, BPcodeBlockType::STR);
        if (globalSize < 0 || size < 0)
        {
            fclose(fp);
            throw InvalidFormatError(fileName, "binary pcode"); // TODO
        }
        top = globalSize + size - 1;
        st.resize(top + 1);

        fread(st.data() + globalSize, sizeof(int), size, fp);

        // code
        if ((size = nextBlock(fp, BPcodeBlockType::CODE)) < 0)
        {
            fclose(fp);
            throw InvalidFormatError(fileName, "binary pcode"); // TODO
        }

        codes = new BPcode[size];

        if (static_cast<int>(fread(codes, sizeof(BPcode), size, fp)) != size)
        {
            fclose(fp);
            throw InvalidFormatError(fileName, "binary pcode");
        }

        // TODO
        fclose(fp);
        del = true;
    }

    void BInterpreter::run()
    {
        sp = -1;
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
                    if (sp != -1)
                    {
                        continue;
                    }
                    return;

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
                    st[top] = !st[top];
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
                    printf("%d\n", st[top--]); // TODO: Optimize
                    fflush(stdout);
                    continue;

                case 15:
                    puts("");
                    fflush(stdout);
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

                case 18:
                    printf("%s", reinterpret_cast<char*>(st.data() + st[top--]));
                    continue;

                case 19:
                    putchar(st[top--]);
                    puts("");
                    fflush(stdout);
                    continue;

                default:
                    throw InstructionError("no such instruction", codes[ip].f); // TODO
                };

            case 0110:
                st[top] = st[sp + codes[ip].a + st[top]];
                continue;

            case 0111:
                st[top] = st[codes[ip].a + st[top]];
                continue;

            case 0120:
                st[sp + codes[ip].a + st[top - 1]] = st[top]; // TODO: Optimize
                top -= 2;
                continue;

            case 0121:
                st[codes[ip].a + st[top - 1]] = st[top]; // TODO: Optimize
                top -= 2;
                continue;

            default:
                throw InstructionError("no such instruction", codes[ip].f); // TODO
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

    // class TInterpreter

    void TInterpreter::read(const char* fileName)
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

        char buffer[8];
        fscanf(fp, "%8s", buffer);
        if (memcmp(buffer, TPCODE_DATA, sizeof(TPCODE_DATA) / sizeof(char)) == 0)
        {
            int size;
            char *strBase;
            while (true)
            {
                fscanf(fp, "%8s", buffer);

                if (memcmp(buffer, fs[013].name, 4) == 0)
                {
                    if (fscanf(fp, " %*d %d ", &size) != 1)
                    {
                        fclose(fp);
                        throw InvalidFormatError(fileName, "text pcode");
                    }
                    st.resize(top + size + 2);
                    strBase = reinterpret_cast<char *>(st.data() + top + 1);
                    fgets(strBase, size * sizeof(int) + 1, fp);
                    strBase[strlen(strBase) - 1] = '\0';
                    top += size;
                }
                else if (memcmp(buffer, fs[005].name, 4) == 0)
                {
                    if (fscanf(fp, " %*d %d", &size) != 1)
                    {
                        fclose(fp);
                        throw InvalidFormatError(fileName, "text pcode");
                    }
                    top += size;
                }
                else
                {
                    break;
                }
            }

            st.resize(top + 2);
        }

        if (memcmp(buffer, TPCODE_CODE, sizeof(TPCODE_CODE) / sizeof(char)) != 0)
        {
            fclose(fp);
            throw InvalidFormatError(fileName, "text pcode");
        }

        TPcode code;
        while (fscanf(fp, "%3s %d %d", code.f.name, &code.l, &code.a) >= 0)
        {
            codes.push_back(code);
        }

        // TODO
        fclose(fp);
    }

    void TInterpreter::run()
    {
        sp = -1;

        while (true)
        {
            switch (codes[++ip].f.id)
            {
            case 0x504f50: // POP
                top -= codes[ip].a;
                continue;

            case 0x54494c: // LIT
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

            case 0x444f4c: // LOD
                if (codes[ip].l == 0)
                {
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
                }
                else
                {
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
                }

            case 0x4f5453: // STO
                switch (codes[ip].l)
                {
                case 0:
                    st[sp + codes[ip].a] = st[top--];
                    continue;

                case 1:
                    st[codes[ip].a] = st[top--];
                    continue;

                case 2:
                    st[sp + codes[ip].a] = st[top];
                    continue;

                case 3:
                    st[codes[ip].a] = st[top];
                    continue;

                default:
                    throw InstructionError("no such instruction", codes[ip].f.name); // TODO
                }

            case 0x4c4143: // CAL
                if (codes[ip].l == 0)
                {
                    if ((top += 2) >= static_cast<int>(st.size()))
                    {
                        st.resize(top + 1);
                    }
                }
                else
                {
                    if ((top += 3) >= static_cast<int>(st.size()))
                    {
                        st.resize(top + 1);
                    }
                }
                st[top] = ip;
                st[top - 1] = sp;
                ip = codes[ip].a - 1;
                sp = top - 1;
                continue;

            case 0x544e49: // INT
                if ((top += codes[ip].a) >= static_cast<int>(st.size()))
                {
                    st.resize(top + 1);
                }
                continue;

            case 0x504d4a: // JMP
                ip = codes[ip].a - 1;
                continue;

            case 0x43504a: // JPC
                if (!st[top--])
                {
                    ip = codes[ip].a - 1;
                }
                continue;

            case 0x52504f: // OPR
                switch (codes[ip].a)
                {
                case 0:
                    top = sp - 1;
                    ip = st[sp + 1];
                    sp = st[sp];
                    if (sp != -1)
                    {
                        continue;
                    }
                    return;

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
                    st[top] = !st[top];
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
                    printf("%d\n", st[top--]); // TODO: Optimize
                    fflush(stdout);
                    continue;

                case 15:
                    puts("");
                    fflush(stdout);
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

                case 18:
                    printf("%s", reinterpret_cast<char*>(st.data() + st[top--]));
                    continue;

                case 19:
                    putchar(st[top--]);
                    puts("");
                    fflush(stdout);
                    continue;

                default:
                    throw InstructionError("no such instruction", codes[ip].f.name); // TODO
                };

            case 0x41444c: // LDA
                if (codes[ip].l == 0)
                {
                    st[top] = st[sp + codes[ip].a + st[top]];
                    continue;
                }
                else
                {
                    st[top] = st[codes[ip].a + st[top]];
                    continue;
                }

            case 0x415453: // STA
                if (codes[ip].l == 0)
                {
                    st[sp + codes[ip].a + st[top - 1]] = st[top]; // TODO: Optimize
                    top -= 2;
                    continue;
                }
                else
                {
                    st[codes[ip].a + st[top - 1]] = st[top]; // TODO: Optimize
                    top -= 2;
                    continue;
                }

            default:
                throw InstructionError("no such instruction", codes[ip].f.name); // TODO
            }
        }

    }

} // namespace sci
