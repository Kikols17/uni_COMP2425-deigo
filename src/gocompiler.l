%{
    /* place here any C variables and definitions */
    #define YY_USER_ACTION \
        for (int i=0; i<yyleng; i++) { \
            if (yytext[i] == '\n') { \
                cur_line++; \
                cur_column = 0; \
            } else { \
                cur_column++; \
            } \
        }


    int cur_column = 0;
    int cur_line = 1;
%}




whitespace  [ \t\n]
digit       [0-9]
letter      [a-zA-Z_]
escape      [fnrt\"]

%X COMMENT
%X STRING
%%
{whitespace}*                   { ; }


";"                             { printf("SEMICOLON\n"); }
","                             { printf("COMMA\n"); }
"_"                             { printf("BLANKID\n"); }

"="                             { printf("ASSIGN\n"); }
"*"                             { printf("TIMES\n"); }
"/"                             { printf("DIV\n"); }
"-"                             { printf("MINUS\n"); }
"+"                             { printf("PLUS\n"); }

"=="                            { printf("EQ\n"); }
">="                            { printf("GE\n"); }
">"                             { printf("GT\n"); }
"<="                            { printf("LE\n"); }
"<"                             { printf("LT\n"); }

"{"                             { printf("LBRACE\n"); }
"("                             { printf("LPAR\n"); }
"["                             { printf("LSQ\n"); }
"}"                             { printf("RBRACE\n"); }
")"                             { printf("RPAR\n"); }
"]"                             { printf("RSQ\n"); }

"%"                             { printf("MOD\n"); }
"!="                            { printf("NE\n"); }
"!"                             { printf("NOT\n"); }
"&&"                            { printf("AND\n"); }
"||"                            { printf("OR\n"); }

"package"                       { printf("PACKAGE\n"); }
"return"                        { printf("RETURN\n"); }
"else"                          { printf("ELSE\n"); }
"for"                           { printf("FOR\n"); }
"if"                            { printf("IF\n"); }

"var"                           { printf("VAR\n"); }
"int"                           { printf("INT\n"); }
"float32"                       { printf("FLOAT32\n"); }
"bool"                          { printf("BOOL\n"); }
"string"                        { printf("STRING\n"); }
"fmt.Println"                   { printf("PRINT\n"); }
"strconv.Atoi"                  { printf("PARSEINT\n"); }
"func"                          { printf("FUNC\n"); }
"os.Args"                       { printf("CMDARGS\n"); }



{digit}+                        { printf("NATURAL(%s)\n", yytext); }
{digit}*"."{digit}+             { printf("DECIMAL(%s)\n", yytext); }
{letter}({letter}|{digit})*     { printf("IDENTIFIER(%s)\n", yytext); }




"/*"                            { printf("COMMENT(\""); BEGIN(COMMENT); }
<COMMENT>\n                     { printf(" "); }
<COMMENT>.                      { ECHO; }
<COMMENT>"*/"                   { printf("\")\n"); BEGIN(INITIAL); }

"\""                            { printf("STRING(\""); BEGIN(STRING); }
<STRING>"\\"{escape}            { printf("ESCAPE(%s)", yytext); }
<STRING>"\\".                   { printf("ERROR: line %d, collumn %d: invalid escape \"%s\"\n", cur_line, cur_column, yytext); }
<STRING>"\""                    { printf("\")\n"); BEGIN(INITIAL); }


.                               { printf("ERROR: line %d, collumn %d: unrecognized token \"%s\"\n", cur_line, cur_column, yytext); }
%%




extern int yylex();
int main() {
    yylex();    /* run the lexical analysis automaton */
    return 0;
}
int yywrap() {  /* called on EOF, return 1 to terminate */
    return 1;
}
