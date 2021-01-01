/*
    Exceptions of SCC.
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

#ifndef _SCC_EXCEPTION_H_
#define _SCC_EXCEPTION_H_

#include <stdexcept>

class RuntimeError : public std::runtime_error
{
protected:

    static const char* CMD_NAME;

    static const char* ERROR_PREFIX;

    static const char* FATAL_ERROR_PREFIX;

public:

    using std::runtime_error::runtime_error;

    static void setCmdName(const char* cmdName);

    /**
     * print error message
     */
    virtual void print(FILE* fp) const noexcept;
};

class FileError : public RuntimeError
{
private:

    const char* fileName;

    const char* fileType;

    const int errnum;

public:
    
    FileError(const char* fileName, const char* fileType);

    /**
     * print error message
     */
    virtual void print(FILE* fp) const noexcept;
};

class InvalidArgumentError : public RuntimeError
{
private:

    const char* arg;

    const char* help;

public:

    InvalidArgumentError(const char* what_arg, const char* arg, const char* help = nullptr);

    /**
     * print error message
     */
    virtual void print(FILE* fp) const noexcept;
};

class RegExpError : public RuntimeError
{
private:

    const char* typeName;

    char ch;

public:

    RegExpError(const char *what_arg, char ch);

    void setTypeName(const char* typeName)
    {
        this->typeName = typeName;
    }

    /**
     * print error message
     */
    virtual void print(FILE* fp) const noexcept;
};

class ParsingError : public RuntimeError
{
    using RuntimeError::RuntimeError;
};

class InstructionError : public RuntimeError
{
private:

    unsigned fbin;

    const char* ftext;

public:

    InstructionError(const char *what_arg, unsigned fbin);

    InstructionError(const char *what_arg, const char* ftext);

    /**
     * print error message
     */
    virtual void print(FILE* fp) const noexcept;
};

class InvalidFormatError : public RuntimeError
{
private:

    const char* fileName;

    const char* fileType;

public:

    InvalidFormatError(const char* fileName, const char* fileType);

    /**
     * print error message
     */
    virtual void print(FILE* fp) const noexcept;
};

class NullPointerError : public RuntimeError
{
    using RuntimeError::RuntimeError;
};

class OutOfRangeError : public RuntimeError
{
    using RuntimeError::RuntimeError;
};

#endif // _SCC_EXCEPTION_H_
