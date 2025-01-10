# deiGo Compiler 2024

Compiler made with lex, yacc, and C.

See makefile for commands:
- `make` (compile everything)

<br>


- `run` (run compiler with no flags)
- `runl` (run compiler with `-l` flag Meta1)
- `runt` (run compiler with `-t` flag Meta2)
- `runs` (run compiler with `-s` flag Meta3)
- `runll` (run compiler with no flags, pipe the result to "program.ll" and interpret with `lli`)

<br>


- `make test_{n}` [n = 1, 2, 3, 4] (run tests for Meta{n})
- `make test_all` (run all tests)

<br>


- `zip_{n}`[n = 1, 2, 3, 4] (zip files for Meta{n})

<br>


- `make clean`


Branches:  
- `Meta1`&nbsp;&nbsp;(meta1)  
- `main`&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;(meta2)  
- `Meta3`&nbsp;&nbsp;(meta3)  
- `Meta4`&nbsp;&nbsp;(meta4)  
