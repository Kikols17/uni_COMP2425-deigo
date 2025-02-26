# build compiler
gocompiler: src/lex.yy.c src/y.tab.c src/ast.c src/ast.h
	clang -Wall -Wextra -Wno-unused-function -g src/lex.yy.c -o gocompiler src/y.tab.c src/ast.c

# build lex.yy.c
src/lex.yy.c: src/gocompiler.l
	lex src/gocompiler.l
	mv lex.yy.c src/lex.yy.c

src/y.tab.c: src/gocompiler.y
	yacc -d -v -t -g --report=all src/gocompiler.y
	mv y.tab.c src/y.tab.c
	mv y.tab.h src/y.tab.h
	mv y.output src/y.output
	mv y.gv src/y.gv


# run compiler
run: gocompiler
	./gocompiler

#run compiler with -l
runl: gocompiler
	./gocompiler -l

#run compiler with -t
runt: gocompiler
	./gocompiler -t

#run compiler with -s
runs: gocompiler
	./gocompiler -s


# run all tests
test_all: test_1 test_2 test_3
	@echo "All tests ran"

# run tests at test_cases folder
test_1: gocompiler
	test_cases/test.sh -b gocompiler -m 1

test_2: gocompiler
	test_cases/test.sh -b gocompiler -m 2

test_3: gocompiler
	test_cases/test.sh -b gocompiler -m 3


# zip gocompiler.l
zip_1: src/gocompiler.l
	cd src && zip gocompiler.zip gocompiler.l
	mv src/gocompiler.zip gocompiler.zip

zip_2: src/gocompiler.l src/gocompiler.y src/ast.c src/ast.h
	cd src && zip gocompiler.zip gocompiler.l gocompiler.y ast.c ast.h
	mv src/gocompiler.zip gocompiler.zip


# clean
clean:
	rm -f gocompiler.zip
	rm -f gocompiler src/lex.yy.c
	rm -f src/y.tab.c src/y.tab.h
	rm -f src/y.output src/y.gv
