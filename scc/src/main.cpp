#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <clocale>

#include "lexer.h"
#include "parser.h"

#include "config.h"
#include "../../common/src/exception.h"

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
        while (word.type != scc::WordType::NONE);
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
        while (word.type != scc::WordType::NONE)
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

    scc::Parser* parser = new scc::RecursiveParser;
    parser->setLexer(lexer);

    parser->open(config.lexFileName, config.parserFileName);

    parser->parse();

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
    }
    return 0;
}
