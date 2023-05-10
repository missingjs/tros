# tros: An Unix-like OS training project

## What's this
Tros is an Unix-like OS training project. 
* The idea was inspired by an amazing book (written in Chinese) <https://book.douban.com/subject/26745156/>
* And the code was originated from <https://github.com/yifengyou/os-elephant>

### New features
Some interesting features have been added:
* dynamic kernel entry point
* condition variable
* make reading/writing of pipe as blocking operation
* virtual file structure
* signals
* concurrent piped commands in shell
* environment variable
* a tiny c runtime library
    * stdio
    * setjmp
* a simple tty layer
    * support CTRL-C, CTRL-D, CTRL-U
* some utility user program, like `cat`, `sort`, `sh`, `seq`, etc.

### Bug fixes
* TLB entry flush
* clear BSS segment when kernel loading
* syscall execve
    * release all memory allocated before load new image of executable
    * load segment defined in ELF header by memory size, not file size
    * clear BSS segment after loaded into memory
* change the type of `task_struct.self_kstack` to `void*`


## How to build & run

### System requirements

* Ubuntu 20.04 or above
* python 3.8+

### Virtual Machine

bochs 2.6.2 (recommended) <https://bochs.sourceforge.io/>

### Building steps

install dependencies

```
sudo apt install build-essential nasm
```

set `BOCHS_HOME` to bochs install dir

```bash
export BOCHS_HOME=/PATH/TO/BOCHS
```

create virtual hard drive files

```bash
make create-hd
```

build loader and kernel

```bash
make
```

build utility executables
```bash
./system/build.sh
```

launch bochs
```bash
./run-bochs.sh
```

Then have fun!
