/*
    Handling the configuration of SCC.
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

#ifndef _SCC_CONFIG_H_
#define _SCC_CONFIG_H_

/**
 * Config manager
 */
class Config
{
private:

    static const char* const HELP;

    static char* splitPath(char* path);

    static const char* splitPath(const char* path);

public:

    static const char* cmdName;

    static const char* const VERSION;

    const char *langFileName;

    const char *inputFileName;

    bool lexOnly;

    const char *lexFileName;

    const char *parserFileName;

    const char *errFileName;

    bool bin;

    const char *objectFileName;

    const char *mipsFileName;

    bool optimize;

    Config();

    /**
     * Set options according to arguments
     * 
     * @param argc: count of arguments
     * @param argv: values of arguments
     * 
     * @exception throw InvalidArgumentError if fail
     */
    void set(int argc, char **argv);
};

#endif // _SCC_CONFIG_H_
