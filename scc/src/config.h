#pragma once

#ifndef _SCC_CONFIG_H_
#define _SCC_CONFIG_H_

#define CG 1

class Config
{
public:

    const char *inputFileName;

    bool lexOnly;
    const char *lexFileName;

    const char *parserFileName;
    const char *errFileName;
    const char *mipsFileName;

    bool optimize;

    Config(int argc, char **argv);
};

#endif // _SCC_CONFIG_H_
