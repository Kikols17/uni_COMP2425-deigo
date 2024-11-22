


# build compiler
gocompiler: src/gocompiler.yy.c src/gocompiler.tab.c src/ast.c src/ast.h
	cc -Wall -Wextra -Wno-unused-function -g src/gocompiler.yy.c -o gocompiler src/gocompiler.tab.c src/ast.c


# build lex.yy.c
src/gocompiler.yy.c: src/gocompiler.l
	lex src/gocompiler.l
	mv lex.yy.c src/gocompiler.yy.c

src/gocompiler.tab.c: src/gocompiler.y
	yacc -d -v -t -g --report=all src/gocompiler.y
	mv y.tab.c src/gocompiler.tab.c
	mv y.tab.h src/gocompiler.tab.h
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


# run all tests
test_all: test_1 test_2
	@echo "All tests ran"

# run tests at test_cases folder
test_1: gocompiler
	test_cases/test.sh -b gocompiler -m 1

test_2: gocompiler
	test_cases/test.sh -b gocompiler -m 2



# zip gocompiler.l
zip_1: src/gocompiler.l
	zip gocompiler.zip src/gocompiler.l

zip_2: src/gocompiler.l src/gocompiler.y src/ast.c src/ast.h
	zip gocompiler.zip src/gocompiler.l src/gocompiler.y src/ast.c src/ast.h



# clean
clean:
	rm -f gocompiler.zip
	rm -f gocompiler src/gocompiler.yy.c
	rm -f src/gocompiler.tab.c src/gocompiler.tab.h
	rm -f src/y.output src/y.gv