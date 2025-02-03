# Documentation

To see the complete html doxygen documentation open docs/html/index.html

# Objectives

This project implements the analysis step of a C- compiler. The whole analysis is performed in a single pass, printing any found errors along the way.

# File structure

```
.
├── buffer.c
├── buffer.h
├── context.c
├── context.h
├── dgenerate
│   ├── dfa.h
│   ├── dgen.c
│   ├── dgen.h
│   ├── dgen.py
│   ├── main
│   ├── main.c
│   ├── makefile
│   ├── mapping.pkl
│   ├── object
│   └── states_dfa.h
├── docs
│   ├── Doxyfile
│   ├── html
│   └── theme
├── g--
├── g--.c
├── ggenerate
│   ├── ggen.py
│   ├── log.txt
│   └── production_table.h
├── makefile
├── object
│   ├── buffer.o
│   ├── context.o
│   ├── g--.o
│   ├── parser.o
│   ├── scanner.o
│   └── semantis.o
├── parser.c
├── parser.h
├── README.md
├── scanner.c
├── scanner.h
├── semantis.c
├── semantis.h
└── test1.txt
```

- root: contains the .c, .h source files, the executable g-- and a sample file test1.txt
- docs: doxygen configuration/generation
- object: contains the compiled object files
- dgenerate: scripts for generating/customizing the scanner's DFA
- ggenerate: scripts for generating/customizing the parsers's production table (check: ggen.py to see the grammar used)

# Executing

` make && ./g- [options] file`
- `-l` or `-L`: print lexer information
- `-p` or `-P`: print parser information
- `-s` or `-S`: print semantic analyzer information

# Building the documentation

`cd docs/ && doxygen`