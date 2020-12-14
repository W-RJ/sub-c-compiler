#include <stdexcept>
#include <cstring>
#include <cerrno>

#include "config.h"
#include "exception.h"

// class WRuntimeError

const wchar_t* WRuntimeError::CMD_NAME = L"scc: ";

const wchar_t* WRuntimeError::ERROR_PREFIX = L"error: ";

const wchar_t* WRuntimeError::FATAL_ERROR_PREFIX = L"fatal error: ";

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
    fwprintf(fp, L"%ls%ls%ls\n", CMD_NAME, ERROR_PREFIX, msg);
}

// class FileError

FileError::FileError(const char* fileName, const wchar_t* fileType) :
        WRuntimeError(fileType), fileName(fileName), fileType(fileType), errnum(errno)
{
}

void FileError::print(FILE* fp) const noexcept
{
    fwprintf(fp, L"%ls%ls%s: %s\n", CMD_NAME, ERROR_PREFIX, fileName, strerror(errnum));
    fwprintf(fp, L"%ls%lsUnable to open %ls files\n", CMD_NAME, FATAL_ERROR_PREFIX, fileType);
}

// class InvalidArgumentError

void InvalidArgumentError::print(FILE* fp) const noexcept
{
    fwprintf(fp, L"%ls%ls%ls\n", CMD_NAME, FATAL_ERROR_PREFIX, msg); // TODO
}

// class RegExpError

RegExpError::RegExpError(const wchar_t *what_arg, wchar_t ch) : WRuntimeError(what_arg), ch(ch)
{
}

void RegExpError::print(FILE* fp) const noexcept
{
    fwprintf(fp, L"%ls%ls In the definition of '%ls': %ls (near %lc)\n", CMD_NAME, ERROR_PREFIX, typeName, msg, ch);
}
