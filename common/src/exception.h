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

class InstuctionError : public RuntimeError
{
    using RuntimeError::RuntimeError; // TODO
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
