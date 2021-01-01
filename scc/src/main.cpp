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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <clocale>

#include "lexer.h"
#include "parser.h"

#include "define.h"
#include "config.h"

#include "../../common/src/exception.h"

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

    scc::Word word;
    if (config.lexFileName == nullptr)
    {
        do
        {
            word.type = scc::WordType::NONE;
            word.val.clear();
            lexer.nextWord(word);
        }
        while (word.type != scc::WordType::FEOF);
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
        lexer.nextWord(word);
        while (word.type != scc::WordType::FEOF)
        {
            fprintf(fp, "%s %s\n", scc::typeName[static_cast<unsigned>(word.type)], word.val.c_str());
            word.type = scc::WordType::NONE;
            word.val.clear();
            lexer.nextWord(word);
        }

        fclose(fp);
    }
    lexer.close();
}

void compile(const Config& config)
{
    scc::Lexer* lexer = new scc::TrieLexer;
    lexer->open(config.inputFileName);

    scc::Parser* parser = new scc::RecursiveParser(config.optimize);
    parser->setLexer(lexer);

    parser->open(config.lexFileName, config.parserFileName, config.errFileName);

    parser->parse();

    if (config.objectFileName != nullptr)
    {
        if (config.bin)
        {
            parser->write(config.objectFileName);
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
            compile(config);
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
