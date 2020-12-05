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
    scc::LangReader reader;
    reader.open(config.langFileName);
    reader.read(false);
    reader.close();

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
            throw NoSuchFileError(L"Unable to open lexical analysis result file");
        }

        // Analyze
        scc::Word word;
        while ((word = lexer.nextWord()).type != scc::WordType::NONE)
        {
            fwprintf(fp, L"%ls %ls\n", scc::typeName[int(word.type)], word.val);
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
        fwprintf(stderr, ERROR_PREFIX L"invalid argument: %ls\n", e.wwhat());
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
            fwprintf(stderr, ERROR_PREFIX L"No such file error: %ls\n", e.wwhat());
            exit(1); // TODO
        }
    }
    else
    {
        // TODO
    }
    return 0;
}
