


# build compiler
gocompiler: gocompiler.l gocompiler.yy.c
	gcc gocompiler.yy.c -o ../gocompiler


# build lex.yy.c
gocompiler.yy.c: gocompiler.l
	lex gocompiler.l
	mv lex.yy.c gocompiler.yy.c


# run compiler
run: ../gocompiler
	./../gocompiler

# run tests at test_cases folder
test_1: gocompiler
	../test_cases/test.sh ../src/gocompiler.l -m 1



# zip gocompiler.l
zip:
	zip ../gocompiler.zip gocompiler.l



# clean
clean:
	rm -f ../gocompiler gocompiler.yy.c
	rm -f ../gocompiler.zip