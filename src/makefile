


# build compiler
../gocompiler: gocompiler.yy.c gocompiler.tab.c ast.c ast.h
	cc -g gocompiler.yy.c -o ../gocompiler gocompiler.tab.c ast.c -Wall -Wno-unused-function


# build lex.yy.c
gocompiler.yy.c: gocompiler.l
	lex gocompiler.l
	mv lex.yy.c gocompiler.yy.c

gocompiler.tab.c: gocompiler.y
	yacc -d -v -t -g --report=all gocompiler.y
	mv y.tab.c gocompiler.tab.c
	mv y.tab.h gocompiler.tab.h


# run compiler
run: ../gocompiler
	./../gocompiler

#run compiler with -l
runl: ../gocompiler
	./../gocompiler -l

#run compiler with -t
runt: ../gocompiler
	./../gocompiler -t


# run all tests
test_all: test_1 test_2
	@echo "All tests ran"

# run tests at test_cases folder
test_1: ../gocompiler
	../test_cases/test.sh -b ../gocompiler -m 1

test_2: ../gocompiler
	../test_cases/test.sh -b ../gocompiler -m 2



# zip gocompiler.l
zip: gocompiler.l
	zip ../gocompiler.zip gocompiler.l



# clean
clean:
	rm -f ../gocompiler.zip
	rm -f ../gocompiler gocompiler.yy.c
	rm -f gocompiler.tab.c gocompiler.tab.h
	rm -f y.output y.gv