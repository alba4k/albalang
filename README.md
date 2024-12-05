# albalang

This project represents my high school final exam work.

It contains the code needed to build an interpreter for a programming language I created (as part of the work), written in C. It should work on Linux, macOS, Windows or any other "major" operating system.

All you should need to build it is [make](https://www.gnu.org/software/make/) and [clang](https://clang.llvm.org/) (by default, see the build instructions for more info); the build instructions are as follow

# Use Instructions

Clone this repository
```shell
$ git clone https://github.com/alba4k/albalang
```

Compile the code to obtain the interpreter binary
```shell
$ make
```
This builds using clang by default. You can specify another compiler, in the example `gcc`, by using
```shell
$ make CC=gcc
```

You should now have a working interpreter inside of the `build` directory. You can now run any file containing albalang code by executing something like
```shell
$ build/albalang /path/to/file.al
```

The use instructions for the language itself only exist in italian for now (inside of my thesis, that I will not publish for the time being), meaning that the only way to get any useful information on how to write any albalang code is reading the codebase for the interpreter (which should be decently readable). Good luck.

---

Aaron Blasko

Lavoro di Maturità

Liceo di Lugano 1

Anno Scolastico 2024 - 2025