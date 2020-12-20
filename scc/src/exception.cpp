#include <stdexcept>
#include <cstring>
#include <cerrno>

#include "config.h"
#include "exception.h"

// class RuntimeError

const char* RuntimeError::ERROR_PREFIX = ": error: ";

const char* RuntimeError::FATAL_ERROR_PREFIX = ": fatal error: ";

void RuntimeError::print(FILE* fp) const noexcept
{
    fprintf(fp, "%s%s%s\n", Config::cmdName, ERROR_PREFIX, what());
}

// class FileError

FileError::FileError(const char* fileName, const char* fileType) :
        RuntimeError(fileType), fileName(fileName), fileType(fileType), errnum(errno)
{
}

void FileError::print(FILE* fp) const noexcept
{
    fprintf(fp, "%s%s%s: %s\n", Config::cmdName, ERROR_PREFIX, fileName, strerror(errnum));
    fprintf(fp, "%s%sUnable to open %s files\n", Config::cmdName, FATAL_ERROR_PREFIX, fileType);
}

// class InvalidArgumentError

InvalidArgumentError::InvalidArgumentError(const char* what_arg, const char* arg, const char* help) : RuntimeError(what_arg), arg(arg), help(help)
{
}

void InvalidArgumentError::print(FILE* fp) const noexcept
{
    if (arg != nullptr)
    {
        fprintf(fp, "%s%s%s '%s'\n", Config::cmdName, FATAL_ERROR_PREFIX, what(), arg); // TODO
    }
    else
    {
        fprintf(fp, "%s%s%s\n", Config::cmdName, FATAL_ERROR_PREFIX, what()); // TODO
    }
    if (help != nullptr)
    {
        fprintf(fp, "%s", help);
    }
}

// class RegExpError

RegExpError::RegExpError(const char *what_arg, char ch) : RuntimeError(what_arg), typeName(nullptr), ch(ch)
{
}

void RegExpError::print(FILE* fp) const noexcept
{
    fprintf(fp, "%s%s In the definition of '%s': %s (near %c)\n", Config::cmdName, ERROR_PREFIX, typeName, what(), ch);
}
