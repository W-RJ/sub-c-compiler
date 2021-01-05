/*
    Entrance of SCC.
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

#include "lexer.h"
#include "parser.h"

#include "define.h"
#include "config.h"

#include "../../common/src/exception.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <clocale>

#if defined(CG) && CG == 4
void runBin(const char* fileName);
#endif

void lexerOnly(const Config& config)
{
    scc::TrieLexer lexer;
    try
    {
        lexer.open(config.inputFileName);
    }
    catch(const FileError& e)
    {
        e.print(stderr);
        exit(1);
    }

    scc::Token token;
    if (config.lexFileName == nullptr)
    {
        do
        {
            token.type = scc::TokenType::NONE;
            token.val.clear();
            lexer.nextToken(token);
        }
        while (token.type != scc::TokenType::FEOF);
    }
    else
    {
        // Open lexical analysis result file
        FILE *fp;
        if (strcmp(config.lexFileName, "-") == 0)
        {
            fp = stdout;
        }
        else
        {
            fp = fopen(config.lexFileName, "w");
        }
        if (fp == nullptr)
        {
            throw FileError(config.lexFileName, "lexical analysis result");
        }

        // Analyze
        lexer.nextToken(token);
        while (token.type != scc::TokenType::FEOF)
        {
            fprintf(fp, "%s %s\n", scc::typeName[static_cast<unsigned>(token.type)], token.val.c_str());
            token.type = scc::TokenType::NONE;
            token.val.clear();
            lexer.nextToken(token);
        }

        fclose(fp);
    }
    lexer.close();
}

bool compile(const Config& config)
{
    scc::Lexer* lexer = new scc::TrieLexer;
    lexer->open(config.inputFileName);

    scc::Parser* parser = new scc::RecursiveParser(config.optimize);
    parser->setLexer(lexer);

    parser->open(config.lexFileName, config.parserFileName, config.errFileName);

    bool success = parser->parse();

    if (success && config.objectFileName != nullptr)
    {
        if (config.bin)
        {
            parser->writeBin(config.objectFileName);
        }
        else
        {
            parser->writeText(config.objectFileName);
        }
    }

    parser->close();
    delete parser;
    lexer->close();
    delete lexer;

    return success;
}

int main(int argc, char **argv)
{
    setlocale(LC_ALL, "zh_CN.UTF-8"); // TODO: Windows

    // Set options according to arguments
    Config config;
    try
    {
        config.set(argc, argv);
    }
    catch (const InvalidArgumentError& e)
    {
        e.print(stderr);
        exit(1); // TODO
    }

    try
    {
        scc::readLang(config.langFileName, true);
    }
    catch (const FileError& e)
    {
        e.print(stderr);
        exit(1);
    }

    if (config.lexOnly)
    {
        try
        {
            lexerOnly(config);
        }
        catch (const FileError& e)
        {
            e.print(stderr);
            exit(1); // TODO
        }
    }
    else
    {
        try
        {
            if (!compile(config))
            {
#ifndef CG
                exit(1);
#endif
            }
        }
        catch (const FileError& e)
        {
            e.print(stderr);
            exit(1); // TODO
        }
        // TODO

#if defined(CG) && CG == 4
        runBin(config.objectFileName);
#endif

    }
    return 0;
}
