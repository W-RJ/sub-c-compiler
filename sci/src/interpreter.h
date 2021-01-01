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

#pragma once

#ifndef _SCI_INTERPRETER_H_
#define _SCI_INTERPRETER_H_

#include <vector>

#include "../../common/src/pcode.h"

namespace sci
{
    class Interpreter
    {
    protected:

        std::vector<int> st;

        int ip;

        int top;

        int sp;

    public:

        Interpreter();

        virtual void read(const char* fileName) = 0;

        virtual void run() = 0;

        virtual ~Interpreter() = default;
    };

    class BInterpreter : public Interpreter
    {
    private:

        BPcode* codes;

        bool del;

    public:

        BInterpreter();

        void set(BPcode* codes);

        virtual void read(const char* fileName) override;

        virtual void run() override;

        virtual ~BInterpreter() override;
    };

    class TInterpreter : public Interpreter
    {
    private:

        std::vector<TPcode> codes;

    public:

        virtual void read(const char* fileName) override;

        virtual void run() override;
    };

} // namespace sci

#endif // _SCI_INTERPRETER_H_
