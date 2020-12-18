#include <stdexcept>
#include <cstring>
#include <cerrno>

#include "config.h"
#include "exception.h"

// class WRuntimeError

const wchar_t* WRuntimeError::ERROR_PREFIX = L": error: ";

const wchar_t* WRuntimeError::FATAL_ERROR_PREFIX = L": fatal error: ";

WRuntimeError::WRuntimeError(const wchar_t *what_arg) : runtime_error(""), msg(what_arg)
{
}

const wchar_t* WRuntimeError::wwhat() const noexcept
{
    return msg;
}

const char* WRuntimeError::what() const noexcept
{
    return nullptr;
}

void WRuntimeError::print(FILE* fp) const noexcept
{
    fwprintf(fp, L"%s%ls%ls\n", Config::cmdName, ERROR_PREFIX, msg);
}

// class FileError

FileError::FileError(const char* fileName, const wchar_t* fileType) :
        WRuntimeError(fileType), fileName(fileName), fileType(fileType), errnum(errno)
{
}

void FileError::print(FILE* fp) const noexcept
{
    fwprintf(fp, L"%s%ls%s: %s\n", Config::cmdName, ERROR_PREFIX, fileName, strerror(errnum));
    fwprintf(fp, L"%s%lsUnable to open %ls files\n", Config::cmdName, FATAL_ERROR_PREFIX, fileType);
}

// class InvalidArgumentError

InvalidArgumentError::InvalidArgumentError(const wchar_t* what_arg, const char* arg, const wchar_t* help) : WRuntimeError(what_arg), arg(arg), help(help)
{
}

void InvalidArgumentError::print(FILE* fp) const noexcept
{
    if (arg != nullptr)
    {
        fwprintf(fp, L"%s%ls%ls '%s'\n", Config::cmdName, FATAL_ERROR_PREFIX, msg, arg); // TODO
    }
    else
    {
        fwprintf(fp, L"%s%ls%ls\n", Config::cmdName, FATAL_ERROR_PREFIX, msg); // TODO
    }
    if (help != nullptr)
    {
        fwprintf(fp, L"%ls", help);
    }
}

// class RegExpError

RegExpError::RegExpError(const wchar_t *what_arg, wchar_t ch) : WRuntimeError(what_arg), typeName(nullptr), ch(ch)
{
}

void RegExpError::print(FILE* fp) const noexcept
{
    fwprintf(fp, L"%s%ls In the definition of '%ls': %ls (near %lc)\n", Config::cmdName, ERROR_PREFIX, typeName, msg, ch);
}
