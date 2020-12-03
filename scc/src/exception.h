#pragma once

#ifndef _SCC_EXCEPTION_H_
#define _SCC_EXCEPTION_H_

#include <stdexcept>

class WRuntimeError : public std::runtime_error
{
protected:

    const wchar_t *msg;

public:

    explicit WRuntimeError(const wchar_t *what_arg);

    virtual const wchar_t* wwhat() const noexcept;
};

class NoSuchFileError : public WRuntimeError
{
    using WRuntimeError::WRuntimeError;
};

class InvalidArgumentError : public WRuntimeError
{
    using WRuntimeError::WRuntimeError;
};

#endif // _SCC_EXCEPTION_H_
