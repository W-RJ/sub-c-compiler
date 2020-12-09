# Sub C Compiler (SCC)

Compiler for the Sub C Programing Language defined by DUT in 2020.

针对大连理工大学在2020年定义的 Sub C 程序设计语言的编译器。

-----------

## Build | 构建方法

### Linux / macOS

请在终端中输入如下命令：

``` bash
cd scc
make
```

将在`scc/build`文件夹中生成可执行文件`scc`。

### Windows

首先确保您已安装 MinGW ，并已将 MinGW 的 bin 文件夹路径添加至环境变量Path中，然后在 Git Bash （或其它 Linux Shell 中）输入如下命令：

``` bash
cd scc
mingw32-make
```

将在`scc\build`文件夹中生成可执行文件`scc.exe`。

> 如果您在 Windows 下用 Visual Studio Code 开发，强烈建议您将 Visual Studio Code 的默认 Shell 改为 Git Bash 。
> 
> 修改方法可参考：
> 
> https://code.visualstudio.com/docs/editor/integrated-terminal#_windows

-----------

## Language Definition | 语言定义

### Lexical Definition | 词法定义

| 类型码 | 正则表达式 |
| ------ | ---------- |
|INTCON|0\|[1-9][0-9]*|
|CHARCON|'[+\-*/_a-zA-Z0-9]'|
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
|MULT|\*|
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
|LBRACK|\[|
|RBRACK|\]|
|LBRACE|{|
|RBRACE|}|
|IDENFR|[_a-zA-Z][_a-zA-Z0-9]*|
