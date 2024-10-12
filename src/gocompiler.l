%{
    /* place here any C variables and definitions */
    #define YY_USER_ACTION \
        token_begincolumn = token_endcolumn; \
        token_beginline = token_endline; \
        if (yytext[yyleng-1] == '\n') { \
            token_endline++; \
            token_endcolumn = 0; \
        } else { \
            token_endcolumn += yyleng; \
            token_begincolumn += 1; \
        }


    int token_begincolumn = 0;  // } indicates the beginning of the token
    int token_beginline = 1;    // }
    int token_endcolumn = 0;        // } indicates the end of the token
    int token_endline = 1;          // }

    int add_semicolon = 0;


    int commentblock_beginline = 0;
    int commentblock_begincolumn = 0;


    char strlit_buf[1024];
    int strlit_buf_len = 0;
    int strlit_error = 0;

    int strlit_beginline = 0;
    int strlit_begincolumn = 0;

    void add_to_strlit_buf(char c) {
        if (strlit_buf_len < 1024) {
            strlit_buf[strlit_buf_len++] = c;
        }
    }
    void print_strlit_buf() {
        if (strlit_error==1) {
            strlit_buf_len = 0;
            strlit_error = 0;
            return;
        }
        printf("STRLIT(\"");
        for (int i = 0; i < strlit_buf_len; i++) {
            printf("%c", strlit_buf[i]);
        }
        printf("\")\n");
        strlit_buf_len = 0;
    }
%}




whitespace  [ \t]
digit       [0-9]
hex         [0-9a-fA-F]
hex_base    [xX]
octal       [0-7]
octal_base  [0]
letter      [a-zA-Z_]
escape      [fnrt\\\"]
newline     "\n"
reserved    "++"|"--"|"break"|"case"|"chan"|"const"|"continue"|"default"|"defer"|"fallthrough"|"goto"|"import"|"interface"|"map"|"range"|"select"|"struct"|"switch"|"type"

%X COMMENT_BLOCK
%X COMMENT_LINE
%X STRING
%%
{whitespace}*                   { ; }

{reserved}                      { add_semicolon = 0; printf("RESERVED(%s)\n", yytext); }


";"                             { add_semicolon = 0; printf("SEMICOLON\n"); }
","                             { add_semicolon = 0; printf("COMMA\n"); }
"_"                             { add_semicolon = 0; printf("BLANKID\n"); }

"="                             { add_semicolon = 0; printf("ASSIGN\n"); }
"*"                             { add_semicolon = 0; printf("STAR\n"); }
"/"                             { add_semicolon = 0; printf("DIV\n"); }
"-"                             { add_semicolon = 0; printf("MINUS\n"); }
"+"                             { add_semicolon = 0; printf("PLUS\n"); }

"=="                            { add_semicolon = 0; printf("EQ\n"); }
">="                            { add_semicolon = 0; printf("GE\n"); }
">"                             { add_semicolon = 0; printf("GT\n"); }
"<="                            { add_semicolon = 0; printf("LE\n"); }
"<"                             { add_semicolon = 0; printf("LT\n"); }

"{"                             { add_semicolon = 0; printf("LBRACE\n"); }
"("                             { add_semicolon = 0; printf("LPAR\n"); }
"["                             { add_semicolon = 0; printf("LSQ\n"); }
"}"                             { add_semicolon = 1; printf("RBRACE\n"); }
")"                             { add_semicolon = 1; printf("RPAR\n"); }
"]"                             { add_semicolon = 1; printf("RSQ\n"); }

"%"                             { add_semicolon = 0; printf("MOD\n"); }
"!="                            { add_semicolon = 0; printf("NE\n"); }
"!"                             { add_semicolon = 0; printf("NOT\n"); }
"&&"                            { add_semicolon = 0; printf("AND\n"); }
"||"                            { add_semicolon = 0; printf("OR\n"); }

"package"                       { add_semicolon = 0; printf("PACKAGE\n"); }
"return"                        { add_semicolon = 1; printf("RETURN\n"); }
"else"                          { add_semicolon = 0; printf("ELSE\n"); }
"for"                           { add_semicolon = 0; printf("FOR\n"); }
"if"                            { add_semicolon = 0; printf("IF\n"); }

"var"                           { add_semicolon = 0; printf("VAR\n"); }
"int"                           { add_semicolon = 0; printf("INT\n"); }
"float32"                       { add_semicolon = 0; printf("FLOAT32\n"); }
"bool"                          { add_semicolon = 0; printf("BOOL\n"); }
"string"                        { add_semicolon = 0; printf("STRING\n"); }
"fmt.Println"                   { add_semicolon = 0; printf("PRINT\n"); }
"strconv.Atoi"                  { add_semicolon = 0; printf("PARSEINT\n"); }
"func"                          { add_semicolon = 0; printf("FUNC\n"); }
"os.Args"                       { add_semicolon = 0; printf("CMDARGS\n"); }



{digit}*"."{digit}+             { add_semicolon = 1; printf("DECIMAL(%s)\n", yytext); }
{digit}+"."{digit}*             { add_semicolon = 1; printf("DECIMAL(%s)\n", yytext); }


"0"{hex_base}({hex})+           { add_semicolon = 1; printf("NATURAL(%s)\n", yytext); }
"0"{hex_base}.                  { add_semicolon = 1; printf("Line %d, column %d: invalid hex constant (%s)\n", token_beginline, token_begincolumn, yytext); }
"0"{octal_base}({octal})+       { add_semicolon = 1; printf("NATURAL(%s)\n", yytext); }
"0"{octal_base}.                { add_semicolon = 1; printf("Line %d, column %d: invalid octal constant (%s)\n", token_beginline, token_begincolumn, yytext); }
{digit}+                        { add_semicolon = 1; printf("NATURAL(%s)\n", yytext); }

{letter}({letter}|{digit})*     { add_semicolon = 1; printf("IDENTIFIER(%s)\n", yytext); }
{newline}                       { if (add_semicolon) { printf("SEMICOLON\n"); } add_semicolon = 0; }


"/*"                            { BEGIN(COMMENT_BLOCK); commentblock_beginline = token_beginline; commentblock_begincolumn = token_begincolumn; }
<COMMENT_BLOCK>{newline}        { ; }
<COMMENT_BLOCK>.                { ; }
<COMMENT_BLOCK><<EOF>>          { BEGIN(INITIAL); printf("Line %d, column %d: unterminated comment\n", commentblock_beginline, commentblock_begincolumn); return 0; }
<COMMENT_BLOCK>"*/"             { BEGIN(INITIAL); }

"//"                            { BEGIN(COMMENT_LINE); if (add_semicolon) { printf("SEMICOLON\n"); add_semicolon = 0; } }
<COMMENT_LINE>.                 { ; }
<COMMENT_LINE>{newline}         { BEGIN(INITIAL); }

"\""                            { add_semicolon = 0; BEGIN(STRING); strlit_beginline = token_beginline; strlit_begincolumn = token_begincolumn; strlit_buf_len = 0; strlit_error = 0; }
<STRING>"\""                    { add_semicolon = 1; BEGIN(INITIAL);if (strlit_error==1) { add_semicolon = 0;} print_strlit_buf(); }
<STRING>"\\"{escape}            { add_semicolon = 0; add_to_strlit_buf('\\'); add_to_strlit_buf(yytext[1]); }
<STRING>"\\".                   { add_semicolon = 0; strlit_error = 1; printf("Line %d, column %d: invalid escape sequence (%s)\n", token_beginline, token_begincolumn, yytext); }
<STRING>"\\"                    { add_semicolon = 0; strlit_error = 1; printf("Line %d, column %d: invalid escape sequence (%s)\n", token_beginline, token_begincolumn, yytext); }
<STRING>"\n"                    { add_semicolon = 0; BEGIN(INITIAL); strlit_error = 1; printf("Line %d, column %d: unterminated string literal\n", strlit_beginline, strlit_begincolumn); }
<STRING><<EOF>>                 { add_semicolon = 0; BEGIN(INITIAL); strlit_error = 1; printf("Line %d, column %d: unterminated string literal\n", strlit_beginline, strlit_begincolumn); return 0; }
<STRING>.                       { add_semicolon = 0; add_to_strlit_buf(yytext[0]); }


.                               { printf("Line %d, column %d: illegal character (%s)\n", token_beginline, token_begincolumn, yytext); }
<<EOF>>                         { if (add_semicolon) { printf("SEMICOLON\n"); } add_semicolon = 0; return 0; }
%%




extern int yylex();
int main() {
    yylex();    /* run the lexical analysis automaton */
    return 0;
}
int yywrap() {  /* called on EOF, return 1 to terminate */
    return 1;
}
