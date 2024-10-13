%{

    /*
     * Trabalho realizado por:
     *      - Francisco Amado Lapa Marques Silva - uc2022213583
     *      - Miguel Moital Rodrigues Cabral Martins - uc2022213951
     */


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
    
    int verbose = 0;


    int token_begincolumn = 0;  // } indicates the beginning of the token
    int token_beginline = 1;    // }
    int token_endcolumn = 0;        // } indicates the end of the token
    int token_endline = 1;          // }

    int add_semicolon = 0;


    int commentblock_beginline = 0;
    int commentblock_begincolumn = 0;


    char strlit_buf[4096];
    int strlit_buf_len = 0;
    int strlit_error = 0;

    int strlit_beginline = 0;
    int strlit_begincolumn = 0;

    void add_to_strlit_buf(char c) {
        if (strlit_buf_len < 4096) {
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
e           [eE]

hex         [0-9a-fA-F]
hexsuf      ("0x"|"0X")

octal       [0-7]
noctal    [89]
octalsuf    [0]

letter      [a-zA-Z_]

escape      [fnrt\\\"]

newline     ("\n"|"\r\n"|"\r")

reserved    "++"|"--"|"break"|"case"|"chan"|"const"|"continue"|"default"|"defer"|"fallthrough"|"go"|"goto"|"import"|"interface"|"map"|"range"|"select"|"struct"|"switch"|"type"



%X COMMENT_BLOCK
%X COMMENT_LINE
%X STRING
%%
{whitespace}*                           { ; }

{reserved}                              { add_semicolon = 0; if (verbose) { printf("RESERVED(%s)\n", yytext); } }

";"                                     { add_semicolon = 0; if (verbose) { printf("SEMICOLON\n"); } }
","                                     { add_semicolon = 0; if (verbose) { printf("COMMA\n"); } }
"_"                                     { add_semicolon = 0; if (verbose) { printf("BLANKID\n"); } }

"="                                     { add_semicolon = 0; if (verbose) { printf("ASSIGN\n"); } }
"*"                                     { add_semicolon = 0; if (verbose) { printf("STAR\n"); } }
"/"                                     { add_semicolon = 0; if (verbose) { printf("DIV\n"); } }
"-"                                     { add_semicolon = 0; if (verbose) { printf("MINUS\n"); } }
"+"                                     { add_semicolon = 0; if (verbose) { printf("PLUS\n"); } }

"=="                                    { add_semicolon = 0; if (verbose) { printf("EQ\n"); } }
">="                                    { add_semicolon = 0; if (verbose) { printf("GE\n"); } }
">"                                     { add_semicolon = 0; if (verbose) { printf("GT\n"); } }
"<="                                    { add_semicolon = 0; if (verbose) { printf("LE\n"); } }
"<"                                     { add_semicolon = 0; if (verbose) { printf("LT\n"); } }

"{"                                     { add_semicolon = 0; if (verbose) { printf("LBRACE\n"); } }
"("                                     { add_semicolon = 0; if (verbose) { printf("LPAR\n"); } }
"["                                     { add_semicolon = 0; if (verbose) { printf("LSQ\n"); } }
"}"                                     { add_semicolon = 1; if (verbose) { printf("RBRACE\n"); } }
")"                                     { add_semicolon = 1; if (verbose) { printf("RPAR\n"); } }
"]"                                     { add_semicolon = 1; if (verbose) { printf("RSQ\n"); } }

"%"                                     { add_semicolon = 0; if (verbose) { printf("MOD\n"); } }
"!="                                    { add_semicolon = 0; if (verbose) { printf("NE\n"); } }
"!"                                     { add_semicolon = 0; if (verbose) { printf("NOT\n"); } }
"&&"                                    { add_semicolon = 0; if (verbose) { printf("AND\n"); } }
"||"                                    { add_semicolon = 0; if (verbose) { printf("OR\n"); } }

"package"                               { add_semicolon = 0; if (verbose) { printf("PACKAGE\n"); } }
"return"                                { add_semicolon = 1; if (verbose) { printf("RETURN\n"); } }
"else"                                  { add_semicolon = 0; if (verbose) { printf("ELSE\n"); } }
"for"                                   { add_semicolon = 0; if (verbose) { printf("FOR\n"); } }
"if"                                    { add_semicolon = 0; if (verbose) { printf("IF\n"); } }

"var"                                   { add_semicolon = 0; if (verbose) { printf("VAR\n"); } }
"int"                                   { add_semicolon = 0; if (verbose) { printf("INT\n"); } }
"float32"                               { add_semicolon = 0; if (verbose) { printf("FLOAT32\n"); } }
"bool"                                  { add_semicolon = 0; if (verbose) { printf("BOOL\n"); } }
"string"                                { add_semicolon = 0; if (verbose) { printf("STRING\n"); } }
"fmt.Println"                           { add_semicolon = 0; if (verbose) { printf("PRINT\n"); } }
"strconv.Atoi"                          { add_semicolon = 0; if (verbose) { printf("PARSEINT\n"); } }
"func"                                  { add_semicolon = 0; if (verbose) { printf("FUNC\n"); } }
"os.Args"                               { add_semicolon = 0; if (verbose) { printf("CMDARGS\n"); } }



{digit}+"."{digit}*{e}("-"|"+")?{digit}+    { add_semicolon = 1; if (verbose) { printf("DECIMAL(%s)\n", yytext); } }
{digit}*"."{digit}+{e}("-"|"+")?{digit}+    { add_semicolon = 1; if (verbose) { printf("DECIMAL(%s)\n", yytext); } }
{digit}+{e}("-"|"+")?{digit}+           { add_semicolon = 1; if (verbose) { printf("DECIMAL(%s)\n", yytext); } }
{digit}*"."{digit}+                     { add_semicolon = 1; if (verbose) { printf("DECIMAL(%s)\n", yytext); } }
{digit}+"."{digit}*                     { add_semicolon = 1; if (verbose) { printf("DECIMAL(%s)\n", yytext); } }


{hexsuf}({hex})+                        { add_semicolon = 1; if (verbose) { printf("NATURAL(%s)\n", yytext); } }
{octalsuf}({octal})+                    { add_semicolon = 1; if (verbose) { printf("NATURAL(%s)\n", yytext); } }
{octalsuf}{octal}*{noctal}+({octal}|{noctal})*  { add_semicolon = 1; printf("Line %d, column %d: invalid octal constant (%s)\n", token_beginline, token_begincolumn, yytext); }
{digit}+                                { add_semicolon = 1; if (verbose) { printf("NATURAL(%s)\n", yytext); } }


{letter}({letter}|{digit})*             { add_semicolon = 1; if (verbose) { printf("IDENTIFIER(%s)\n", yytext); } }
{newline}                               { if (add_semicolon && verbose) { printf("SEMICOLON\n"); } add_semicolon = 0; }



"/*"                                    { BEGIN(COMMENT_BLOCK); commentblock_beginline = token_beginline; commentblock_begincolumn = token_begincolumn; }
<COMMENT_BLOCK>{newline}                { ; }
<COMMENT_BLOCK>.                        { ; }
<COMMENT_BLOCK><<EOF>>                  { BEGIN(INITIAL); printf("Line %d, column %d: unterminated comment\n", commentblock_beginline, commentblock_begincolumn); return 0; }
<COMMENT_BLOCK>"*/"                     { BEGIN(INITIAL); }



"//"                                    { BEGIN(COMMENT_LINE); if (add_semicolon && verbose) { printf("SEMICOLON\n"); add_semicolon = 0; } }
<COMMENT_LINE>.                         { ; }
<COMMENT_LINE>{newline}                 { BEGIN(INITIAL); }



"\""                                    { add_semicolon = 0; BEGIN(STRING); strlit_beginline = token_beginline; strlit_begincolumn = token_begincolumn; strlit_buf_len = 0; strlit_error = 0; }
<STRING>"\""                            { add_semicolon = 1; BEGIN(INITIAL); if (strlit_error==1) { add_semicolon = 0; } if (verbose) { print_strlit_buf(); } }
<STRING>"\\"{escape}                    { add_semicolon = 0; add_to_strlit_buf('\\'); add_to_strlit_buf(yytext[1]); }
<STRING>"\\".                           { add_semicolon = 0; strlit_error = 1; printf("Line %d, column %d: invalid escape sequence (%s)\n", token_beginline, token_begincolumn, yytext); }
<STRING>"\\"                            { add_semicolon = 0; strlit_error = 1; printf("Line %d, column %d: invalid escape sequence (%s)\n", token_beginline, token_begincolumn, yytext); }
<STRING>"\n"                            { add_semicolon = 0; BEGIN(INITIAL); strlit_error = 1; printf("Line %d, column %d: unterminated string literal\n", strlit_beginline, strlit_begincolumn); }
<STRING><<EOF>>                         { add_semicolon = 0; BEGIN(INITIAL); strlit_error = 1; printf("Line %d, column %d: unterminated string literal\n", strlit_beginline, strlit_begincolumn); return 0; }
<STRING>.                               { add_semicolon = 0; add_to_strlit_buf(yytext[0]); }



<<EOF>>                                 { if (add_semicolon && verbose) { printf("SEMICOLON\n"); } add_semicolon = 0; return 0; }
.                                       { printf("Line %d, column %d: illegal character (%s)\n", token_beginline, token_begincolumn, yytext); }

%%



extern int yylex();
int main(int argc, char **argv) {
    if (argc > 1) {
        if (strcmp(argv[1], "-l") == 0) {
            verbose = 1;
        }
    }
    yylex();    /* run the lexical analysis automaton */
    return 0;
}
int yywrap() {  /* called on EOF, return 1 to terminate */
    return 1;
}
