# Sub C Compiler (SCC)

Compiler for the Sub C Programing Language defined by DUT in 2020.

针对大连理工大学在2020年定义的 Sub C 程序设计语言的编译器。

## Build 构建方法

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
