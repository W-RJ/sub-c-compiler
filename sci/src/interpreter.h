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
