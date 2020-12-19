# Sub C Compiler (SCC)

Compiler for the Sub C Programing Language defined by CG.

针对希冀平台定义的 Sub C 程序设计语言的编译器。

## Build & Installation & Test | 构建、安装、自测方法

### Linux / macOS

首先确保您已安装`make`，然后在终端中输入如下命令：

``` bash
make release
```

将在`release`目录中生成可执行文件`scc`和默认的动态词法定义文件`sc.lang`。运行时请确保上述两个文件被放置在同一个目录下。

若要安装到操作系统的程序目录下，可以输入如下命令：

``` bash
sudo make install
```

或

``` bash
su -c make install
```

在上述安装命令中，默认安装路径为`/usr/local/bin`，可用环境变量`DESTDIR`指定安装的目标路径，可将`install`替换为`uninstall`来卸载。

若要生成强制固定输入输出文件名为希冀平台要求的文件名（而不是根据命令行参数）、且不支持读取动态词法定义的可执行程序，可用环境变量`CG`指定希冀平台的题号（范围为1～5）。例如：

``` bash
make release CG=1
```

将生成只进行词法分析的、输入输出文件固定的、词法类型名和正则表达式固定的可执行程序。

若不指定`CG`变量，则默认生成正常的、功能完整的可执行程序。

若要生成能够提交到希冀平台的压缩包，首先确保您已安装`zip`，然后可输入如下命令：

``` bash
make zip CG=?
```

其中`?`的范围为1～5。该命令将生成一个能够提交到希冀平台的zip压缩包。

> 建议使用上述命令打包，而不要自行打包。  
> 若一定要自行打包代码提交，请先使用`make release CG=?`命令（`?`的范围为1～5）编译一次，然后再打包，并请注意不要将`*.pyc`文件打包进压缩包。

若要执行本程序自带的自我测试脚本，首先确保您已安装`python3`，然后使用如下命令安装`pytest`（如已安装可以跳过）：

``` bash
python -m pip install --upgrade pip
python -m pip install -r requirements.txt
```

在某些操作系统中，您可能需要将`python`替换为`python3`。安装`pytest`后，可输入如下命令执行自我测试脚本：

``` bash
make test
```

若命令执行成功，则说明测试成功。

上述自测命令仅用于使用自带测试用例自测，若要使用其它测试用例请使用`make release`命令或`make release CG=?`命令构建后测试。

### Windows

首先确保您已安装 MinGW ，并已将 MinGW 的 bin 目录添加至环境变量Path中，然后在 Git Bash （或其它 Linux Shell 中）输入如下命令：

``` bash
mingw32-make release
```

将在`release`目录中生成可执行文件`scc.exe`和默认的动态词法定义文件`sc.lang`。运行时请确保上述两个文件被放置在同一个目录下。


> 如果您在使用 Windows 系统并使用 Visual Studio Code ，强烈建议您将它的默认 Shell 改为 Git Bash 。  
> 修改方法可参考：https://code.visualstudio.com/docs/editor/integrated-terminal#_windows

若要生成强制固定输入输出文件名为希冀平台要求的文件名（而不是根据命令行参数）、且不支持读取动态词法定义的可执行程序，可用环境变量`CG`指定希冀平台的题号（范围为1～5）。例如：

``` bash
mingw32-make release CG=1
```

将生成只进行词法分析的、输入输出文件固定的、词法类型名和正则表达式固定的可执行程序。

若不指定`CG`变量，则默认生成正常的、功能完整的可执行程序。

> 若要打包代码提交，请先使用`mingw32-make release CG=?`命令（`?`的范围为1～5）编译一次，然后再打包，并请注意不要将`*.pyc`文件打包进压缩包。

若要执行本程序自带的自我测试脚本，首先确保您已安装`python3`，然后使用如下命令安装`pytest`（如已安装可以跳过）：

``` bash
python -m pip install --upgrade pip
python -m pip install -r requirements.txt
```

安装`pytest`后，可输入如下命令执行自我测试脚本：

``` bash
mingw32-make test
```

若命令执行成功，则说明测试成功。

上述自测命令仅用于使用自带测试用例自测，若要使用其它测试用例请使用`make release`命令或`make release CG=?`命令构建后测试。

## Project Structure | 项目结构

目前本项目分为3个子项目，其中在每个子项目中 src 为源码目录， build 为编译生成的目标文件目录，test为自动测试目录， Makefile 为编译脚本。代码的编译由 Makefile 管理。

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
| define    | 包含各种编译选项的宏定义 |
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

|  类型码  |       正则表达式       |
| -------- | ---------------------- |
| INTCON   | 0\|[1-9][0-9]*         |
| CHARCON  | '[+\\-*/_a-zA-Z0-9]'   |
| STRCON   | "[ !#-~]*"             |
| CONSTTK  | const                  |
| INTTK    | int                    |
| CHARTK   | char                   |
| VOIDTK   | void                   |
| MAINTK   | main                   |
| IFTK     | if                     |
| ELSETK   | else                   |
| DOTK     | do                     |
| WHILETK  | while                  |
| FORTK    | for                    |
| SCANFTK  | scanf                  |
| PRINTFTK | printf                 |
| RETURNTK | return                 |
| PLUS     | +                      |
| MINU     | -                      |
| MULT     | \\*                    |
| DIV      | /                      |
| LSS      | <                      |
| LEQ      | <=                     |
| GRE      | >                      |
| GEQ      | >=                     |
| EQL      | ==                     |
| NEQ      | !=                     |
| ASSIGN   | =                      |
| SEMICN   | ;                      |
| COMMA    | ,                      |
| LPARENT  | (                      |
| RPARENT  | )                      |
| LBRACK   | \\[                    |
| RBRACK   | \\]                    |
| LBRACE   | {                      |
| RBRACE   | }                      |
| IDENFR   | [_a-zA-Z][_a-zA-Z0-9]* |

## Grammar definition | 语法定义

Sub C 程序设计语言的EBNF语法定义如下：

```
           <str> -> STRCON
       <program> -> [<constBlock>] [<varBlock>] {<funDef> | <voidFunDef>} <mainFun>
    <constBlock> -> CONSTTK <constDef> SEMICN {CONSTTK <constDef> SEMICN}
      <constDef> -> INTTK IDENFR ASSIGN <integer> {COMMA IDENFR ASSIGN <integer>} | CHARTK IDENFR ASSIGN CHARCON {COMMA IDENFR ASSIGN CHARCON}
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
        <loopSt> -> WHILETK LPARENT <condition> RPARENT <statement> | DOTK <statement> WHILETK LPARENT <condition> RPARENT | FORTK LPARENT IDENFY ASSIGN <expression> SEMICN <condition> SEMICN IDENFY ASSIGN IDENFY (PLUS | MINU) <step> RPARENT <statement>
          <step> -> <uinteger>
       <funCall> -> IDENFR LPARENT <paramVal> RPARENT
   <voidFunCall> -> IDENFR LPARENT <paramVal> RPARENT
      <paramVal> -> <expression> {COMMA <expression>} | ɛ
<statementBlock> -> {<statement>}
        <readSt> -> SCANFTK LPARENT IDENFR {COMMA IDENFR} RPARENT
       <writeSt> -> PRINTFTK LPARENT <str> COMMA <expression> RPARENT | PRINTFTK LPARENT <str> RPARENT | PRINTFTK LPARENT <expression> RPARENT
      <returnSt> -> RETURNTK [LPARENT <expression> RPARENT]
```
