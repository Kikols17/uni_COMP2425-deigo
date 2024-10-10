


# build compiler
compiler: gocompiler.l
	lex gocompiler.l && gcc lex.yy.c -o lexer


# run compiler
run: compiler
	./lexer



# zip gocompiler.l
zip:
	zip ../gocompiler.zip gocompiler.l



# clean
clean:
	rm -f lexer lex.yy.c
	rm -f ../gocompiler.zip