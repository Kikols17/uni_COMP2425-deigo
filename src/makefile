


# build compiler
lexer: gocompiler.l lex.yy.c
	gcc lex.yy.c -o lexer


# build lex.yy.c
lex.yy.c: gocompiler.l
	lex gocompiler.l


# run compiler
run: lexer
	./lexer



# zip gocompiler.l
zip:
	zip ../gocompiler.zip gocompiler.l



# clean
clean:
	rm -f lexer lex.yy.c
	rm -f ../gocompiler.zip