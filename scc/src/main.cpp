#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>

#include "lexer.h"
#include "parser.h"

#include "config.h"
#include "exception.h"

void lexerOnly(const Config& config)
{
    try
    {
        scc::readLang(config.langFileName, true);
    }
    catch(const NoSuchFileError& e)
    {
        e.print(stderr);
        exit(1);
    }

    scc::DFALexer lexer;
    lexer.open(config.inputFileName);

    if (config.lexFileName == nullptr)
    {
        while (lexer.nextWord().type != scc::WordType::NONE);
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
            throw NoSuchFileError(config.lexFileName, L"lexical analysis result");
        }

        // Analyze
        scc::Word word;
        while ((word = lexer.nextWord()).type != scc::WordType::NONE)
        {
            fwprintf(fp, L"%ls %ls\n", scc::typeName[unsigned(word.type)], word.val);
        }

        fclose(fp);
    }
    lexer.close();
}

int main(int argc, char **argv)
{
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

    if (config.lexOnly)
    {
        try
        {
            lexerOnly(config);
        }
        catch(const NoSuchFileError& e)
        {
            e.print(stderr);
            exit(1); // TODO
        }
    }
    else
    {
        // TODO
    }
    return 0;
}
