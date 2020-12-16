# Sub C Compiler (SCC)

Compiler for the Sub C Programing Language defined by DUT in 2020.

针对大连理工大学在2020年定义的 Sub C 程序设计语言的编译器。

## Build | 构建方法

### Linux / macOS

首先确保您已安装 make ，然后在终端中输入如下命令：

``` bash
make
```

将在`build`目录中生成可执行文件`scc`。

### Windows

首先确保您已安装 MinGW ，并已将 MinGW 的 bin 目录添加至环境变量Path中，然后在 Git Bash （或其它 Linux Shell 中）输入如下命令：

``` bash
mingw32-make
```

将在`build`目录中生成可执行文件`scc.exe`。

> 如果您在使用 Windows 系统并使用 Visual Studio Code ，强烈建议您将它的默认 Shell 改为 Git Bash 。
> 
> 修改方法可参考：https://code.visualstudio.com/docs/editor/integrated-terminal#_windows

## Project Structure | 项目结构

目前本项目分为3个子项目，其中在每个子项目中 src 为源码目录， build 为编译生成的目标文件目录， Makefile 为编译脚本。代码的编译由 Makefile 管理。

### SCC

是 Sub C 程序设计语言的编译器部分。各源文件的功能如下：

|  源文件   | 主要功能 |
| :-------: | -------- |
| main      | 程序入口 |
| lexer     | 包含各个词法分析类，包含 **字典树(Trie)** 方法的词法分析与 **普通DFA** 方法的词法分析 |
| parser    | 包含各个语法分析类，包含递归子程序法的语法分析、语义分析 |
| regexp    | 包含对 **正则表达式** 的词法、语法、语义分析和字典树的生成，支持范型 |
| trie      | 包含字典树数据结构，支持范型 |
| sc.lang   | 包含各词法类型的类型码和正则表达式，支持 **在运行时动态修改** |
| exception | 包含各种异常类，支持不同等级的运行时异常 |
| config    | 包含对各种预定义参数与命令行参数的处理 |
| cg        | 包含对希冀系统不支持 Makefile 的问题的处理 |

### IDE

是配套的集成开发环境（IDE）部分。

### Tools

是相关的工具部分。

## Language Definition | 语言定义

### RegExp Definition | 正则表达式定义

本项目中使用的正则表达式的EBNF定义如下：

```
  <regexp> -> <expr>{'|'<expr>}
    <expr> -> <item>{<item>}
    <item> -> <itemChar>|'['<opt>']'['*']
     <opt> -> <optItem>{<optItem>}
 <optItem> -> <optChar>['-'<optChar>]
<itemChar> -> 除'[', ']', '*', '|', '\'外的ASCII在32~126之间的字符|'\'任意ASCII在32~126之间的字符
 <optChar> -> 除']', '-', '\'外的ASCII在32~126之间的字符|'\'任意ASCII在32~126之间的字符
```

### Lexical Definition | 词法定义

利用上述正则表达式文法，将 Sub C 程序设计语言的词法定义如下：

| 类型码 | 正则表达式 |
| ------ | ---------- |
|INTCON|0\|[1-9][0-9]*|
|CHARCON|'[+\\-*/_a-zA-Z0-9]'|
|STRCON|"[ !#-~]*"|
|CONSTTK|const|
|INTTK|int|
|CHARTK|char|
|VOIDTK|void|
|MAINTK|main|
|IFTK|if|
|ELSETK|else|
|DOTK|do|
|WHILETK|while|
|FORTK|for|
|SCANFTK|scanf|
|PRINTFTK|printf|
|RETURNTK|return|
|PLUS|+|
|MINU|-|
|MULT|\\*|
|DIV|/|
|LSS|<|
|LEQ|<=|
|GRE|>|
|GEQ|>=|
|EQL|==|
|NEQ|!=|
|ASSIGN|=|
|SEMICN|;|
|COMMA|,|
|LPARENT|(|
|RPARENT|)|
|LBRACK|\\[|
|RBRACK|\\]|
|LBRACE|{|
|RBRACE|}|
|IDENFR|[_a-zA-Z][_a-zA-Z0-9]*|

## Grammar definition | 语法定义

Sub C 程序设计语言的语法定义如下：

```
           <str> -> STRCON
    <constBlock> -> CONSTTK <constDef> SEMICN {CONSTTK <constDef> SEMICN}
      <constDef> -> INTTK IDENFR ASSIGN INTCON {COMMA IDENFR ASSIGN INTCON} | CHARTK IDENFR  ASSIGN INTCON {COMMA IDENFR ASSIGN INTCON}
      <uinteger> -> INTCON
       <integer> -> [PLUS | MINU] <uinteger>
   <declareHead> -> INTTK IDENFR | CHARTK IDENFR
      <varBlock> -> <varDef> SEMICN {<varDef> SEMICN}
        <varDef> -> (INTTK | CHARTK) (IDENFR | IDENFR LBRACK <uinteger> RBRACK) {COMMA (IDENFR | IDENFR LBRACK <uinteger> RBRACK)}
        <funDef> -> <declareHead> LPARENT <param> RPARENT LBRACE <compoundSt> RBRACE
    <voidFunDef> -> VOIDTK IDENFR LPARENT <param> RPARENT LBRACE <compoundSt> RBRACE
    <compoundSt> -> [<constBlock>] [<varBlock>] <statementBlock>
         <param> -> (INTTK | CHARTK) IDENFR {COMMA (INTTK | CHARTK) IDENFR} | ɛ
       <mainFun> -> VOIDTK MAINTK LPARENT RPARENT LBRACE <compoundSt> RBRACE
    <expression> -> [PLUS | MINU] <item> {(PLUS | MINU) <item>}
          <item> -> <factor> {(MULT | DIV) <factor>}
        <factor> -> IDENFR | IDENFR LBRACK <expression> RBRACK | LPARENT <expression> RPARENT | <integer> | CHARCON | <funCall>
     <statement> -> <conditionSt> | <loopSt> | LBRACE <statementBlock> RBRACE | <funCall> SEMICN | <voidFunCall> SEMICN | <assignSt> SEMICN | <readSt> SEMICN | <writeSt> SEMICN | SEMICN | <returnSt> SEMICN
      <assignSt> -> IDENFR ASSIGN <expression> | IDENFR LBRACK <expression> RBRACK ASSIGN <expression>
   <conditionSt> -> IFTK LPARENT <condition> RPARENT <statement> [ELSETK <statement>]
     <condition> -> <expression> (LSS | LEQ | GRE | GEQ | EQL | NEQ) <expression> | <expression>
        <loopSt> -> WHILETK LPARENT <condition> RPARENT <statement> | DOTK <statement> WHILETK LPARENT <condition> RPARENT | for LPARENT IDENFY ASSIGN <expression> SEMICN <condition> SEMICN IDENFY ASSIGN IDENFY (PLUS | MINU) <step> RPARENT <statement>
          <step> -> <uinteger>
       <funCall> -> IDENFR LPARENT <paramVal> RPARENT
   <voidFunCall> -> IDENFR LPARENT <paramVal> RPARENT
      <paramVal> -> <expression> {COMMA <expression>} | ɛ
<statementBlock> -> {<statement>}
        <readSt> -> SCANFTK LPARENT IDENFR {COMMA IDENFR} RPARENT
       <writeSt> -> PRINTFTK LPARENT <str> COMMA <expression> RPARENT | PRINTFTK LPARENT <str> RPARENT | PRINTFTK LPARENT <expression> RPARENT
      <returnSt> -> RETURNTK [LPARENT <expression> RPARENT]
```