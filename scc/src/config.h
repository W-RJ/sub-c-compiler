#pragma once

#ifndef _SCC_CONFIG_H_
#define _SCC_CONFIG_H_

/**
 * Config manager
 */
class Config
{
public:

    static const char* cmdName;

    const char *langFileName;

    const char *inputFileName;

    bool lexOnly;
    const char *lexFileName;

    const char *parserFileName;
    const char *errFileName;
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
