#include <stdexcept>

#include "exception.h"

WRuntimeError::WRuntimeError(const wchar_t *what_arg) : runtime_error(""), msg(what_arg)
{
}

const wchar_t* WRuntimeError::wwhat() const noexcept
{
    return msg;
}
