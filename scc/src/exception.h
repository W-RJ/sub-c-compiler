#pragma once

#ifndef _SCC_EXCEPTION_H_
#define _SCC_EXCEPTION_H_

#include <stdexcept>

class WRuntimeError : public std::runtime_error
{
protected:

    const wchar_t *msg;

    static const wchar_t* CMD_NAME;

    static const wchar_t* ERROR_PREFIX;

    static const wchar_t* FATAL_ERROR_PREFIX;

public:

    explicit WRuntimeError(const wchar_t *what_arg);

    /**
     * @return error message
     */
    virtual const wchar_t* wwhat() const noexcept;

    /**
     * @return nullptr
     *
     * @deprecated use wwhat instead
     */
    virtual const char* what() const noexcept override;

    /**
     * print error message
     */
    virtual void print(FILE* fp) const noexcept;
};

class NoSuchFileError : public WRuntimeError
{
private:

    const char* fileName;

    const wchar_t* fileType;

    static const wchar_t* MSG;

public:
    
    NoSuchFileError(const char* fileName, const wchar_t* fileType);

    /**
     * print error message
     */
    virtual void print(FILE* fp) const noexcept;
};

class InvalidArgumentError : public WRuntimeError
{
public:

    using WRuntimeError::WRuntimeError;

    /**
     * print error message
     */
    virtual void print(FILE* fp) const noexcept;
};

class RegExpError : public WRuntimeError
{
private:

    const wchar_t* typeName;

public:

    using WRuntimeError::WRuntimeError;

    void setTypeName(const wchar_t* typeName)
    {
        this->typeName = typeName;
    }

    /**
     * print error message
     */
    virtual void print(FILE* fp) const noexcept;
};

#endif // _SCC_EXCEPTION_H_
