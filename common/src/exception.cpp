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

#include <stdexcept>
#include <cstring>
#include <cerrno>

#include "exception.h"

// class RuntimeError

const char* RuntimeError::CMD_NAME = nullptr;

const char* RuntimeError::ERROR_PREFIX = ": error: ";

const char* RuntimeError::FATAL_ERROR_PREFIX = ": fatal error: ";

void RuntimeError::setCmdName(const char* cmdName)
{
    CMD_NAME = cmdName;
}

void RuntimeError::print(FILE* fp) const noexcept
{
    fprintf(fp, "%s%s%s\n", CMD_NAME, ERROR_PREFIX, what());
}

// class FileError

FileError::FileError(const char* fileName, const char* fileType) :
        RuntimeError(fileType), fileName(fileName), fileType(fileType), errnum(errno)
{
}

void FileError::print(FILE* fp) const noexcept
{
    fprintf(fp, "%s%s%s: %s\n", CMD_NAME, ERROR_PREFIX, fileName, strerror(errnum));
    fprintf(fp, "%s%sUnable to open %s file\n", CMD_NAME, FATAL_ERROR_PREFIX, fileType);
}

// class InvalidArgumentError

InvalidArgumentError::InvalidArgumentError(const char* what_arg, const char* arg, const char* help) : RuntimeError(what_arg), arg(arg), help(help)
{
}

void InvalidArgumentError::print(FILE* fp) const noexcept
{
    if (arg != nullptr)
    {
        fprintf(fp, "%s%s%s '%s'\n", CMD_NAME, FATAL_ERROR_PREFIX, what(), arg); // TODO
    }
    else
    {
        fprintf(fp, "%s%s%s\n", CMD_NAME, FATAL_ERROR_PREFIX, what()); // TODO
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
    fprintf(fp, "%s%s In the definition of '%s': %s (near %c)\n", CMD_NAME, ERROR_PREFIX, typeName, what(), ch);
}

// class InstructionError

InstructionError::InstructionError(const char *what_arg, unsigned fbin) : RuntimeError(what_arg), fbin(fbin), ftext(nullptr)
{
}

InstructionError::InstructionError(const char *what_arg, const char* ftext) : RuntimeError(what_arg), ftext(ftext)
{
}

void InstructionError::print(FILE* fp) const noexcept
{
    if (ftext != nullptr)
    {
        fprintf(fp, "%s%s%s: %s\n", CMD_NAME, ERROR_PREFIX, ftext, what());
    }
    else
    {
        fprintf(fp, "%s%s%04o: %s\n", CMD_NAME, ERROR_PREFIX, fbin, what());
    }
}


// class InvalidFormatError

InvalidFormatError::InvalidFormatError(const char* fileName, const char* fileType) : RuntimeError(fileType), fileName(fileName), fileType(fileType)
{
}

void InvalidFormatError::print(FILE* fp) const noexcept
{
    fprintf(fp, "%s%s%s: Not a %s file\n", CMD_NAME, FATAL_ERROR_PREFIX, fileName, fileType);
}
