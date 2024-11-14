%{

    /*
     * Trabalho realizado por:
     *      - Francisco Amado Lapa Marques Silva - uc2022213583
     *      - Miguel Moital Rodrigues Cabral Martins - uc2022213951
     */


    /* place here any C variables and definitions */
    #include "ast.h"
    #include "gocompiler.tab.h"
    #define YY_USER_ACTION \
        token_begincolumn = token_endcolumn; \
        token_beginline = token_endline; \
        token_endcolumn += yyleng; 

    #define PASS_TOKEN() yylval.lexeme = strdup(yytext);


    /* modes */
    int lex_mode = 0;   // meta1
    int yacc_mode = 0;  // meta2


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
    


    // macro YY_USER_ACTION, which updates 'lex_column', is executed before every action
    int yyparse(void);
    void yyerror(char *);

    int errors = 0;

    extern struct node *program;



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
noctal      [89]
octalsuf    [0]

letter      [a-zA-Z_]

escape      [fnrt\\\"]

newline     ("\n"|"\r\n"|"\r")

reserved    "++"|"--"|"break"|"case"|"chan"|"const"|"continue"|"default"|"defer"|"fallthrough"|"go"|"goto"|"import"|"interface"|"map"|"range"|"select"|"struct"|"switch"|"type"



%X COMMENT_BLOCK
%X COMMENT_LINE
%X STRINGLIT
%%
{whitespace}*                               { ; }

{reserved}                                  { add_semicolon = 0; if (lex_mode) { printf("RESERVED(%s)\n", yytext); }            if (yacc_mode) { PASS_TOKEN();          return RESERVED; } }

";"                                         { add_semicolon = 0; if (lex_mode) { printf("SEMICOLON\n"); }                       if (yacc_mode) {                        return SEMICOLON; } }
","                                         { add_semicolon = 0; if (lex_mode) { printf("COMMA\n"); }                           if (yacc_mode) {                        return COMMA; } }
"_"                                         { add_semicolon = 0; if (lex_mode) { printf("BLANKID\n"); }                         if (yacc_mode) {                        return BLANKID; } }

"="                                         { add_semicolon = 0; if (lex_mode) { printf("ASSIGN\n"); }                          if (yacc_mode) {                        return ASSIGN; } }
"*"                                         { add_semicolon = 0; if (lex_mode) { printf("STAR\n"); }                            if (yacc_mode) {                        return STAR; } }
"/"                                         { add_semicolon = 0; if (lex_mode) { printf("DIV\n"); }                             if (yacc_mode) {                        return DIV; } }
"-"                                         { add_semicolon = 0; if (lex_mode) { printf("MINUS\n"); }                           if (yacc_mode) {                        return MINUS; } }
"+"                                         { add_semicolon = 0; if (lex_mode) { printf("PLUS\n"); }                            if (yacc_mode) {                        return PLUS; } }

"=="                                        { add_semicolon = 0; if (lex_mode) { printf("EQ\n"); }                              if (yacc_mode) {                        return EQ; } }
">="                                        { add_semicolon = 0; if (lex_mode) { printf("GE\n"); }                              if (yacc_mode) {                        return GE; } }
">"                                         { add_semicolon = 0; if (lex_mode) { printf("GT\n"); }                              if (yacc_mode) {                        return GT; } }
"<="                                        { add_semicolon = 0; if (lex_mode) { printf("LE\n"); }                              if (yacc_mode) {                        return LE; } }
"<"                                         { add_semicolon = 0; if (lex_mode) { printf("LT\n"); }                              if (yacc_mode) {                        return LT; } }

"{"                                         { add_semicolon = 0; if (lex_mode) { printf("LBRACE\n"); }                          if (yacc_mode) {                        return LBRACE; } }
"("                                         { add_semicolon = 0; if (lex_mode) { printf("LPAR\n"); }                            if (yacc_mode) {                        return LPAR; } }
"["                                         { add_semicolon = 0; if (lex_mode) { printf("LSQ\n"); }                             if (yacc_mode) {                        return LSQ; } }
"}"                                         { add_semicolon = 1; if (lex_mode) { printf("RBRACE\n"); }                          if (yacc_mode) {                        return RBRACE; } }
")"                                         { add_semicolon = 1; if (lex_mode) { printf("RPAR\n"); }                            if (yacc_mode) {                        return RPAR; } }
"]"                                         { add_semicolon = 1; if (lex_mode) { printf("RSQ\n"); }                             if (yacc_mode) {                        return RSQ; } }

"%"                                         { add_semicolon = 0; if (lex_mode) { printf("MOD\n"); }                             if (yacc_mode) {                        return MOD; } }
"!="                                        { add_semicolon = 0; if (lex_mode) { printf("NE\n"); }                              if (yacc_mode) {                        return NE; } }
"!"                                         { add_semicolon = 0; if (lex_mode) { printf("NOT\n"); }                             if (yacc_mode) {                        return NOT; } }
"&&"                                        { add_semicolon = 0; if (lex_mode) { printf("AND\n"); }                             if (yacc_mode) {                        return AND; } }
"||"                                        { add_semicolon = 0; if (lex_mode) { printf("OR\n"); }                              if (yacc_mode) {                        return OR; } }

"package"                                   { add_semicolon = 0; if (lex_mode) { printf("PACKAGE\n"); }                         if (yacc_mode) {                        return PACKAGE; } }
"return"                                    { add_semicolon = 1; if (lex_mode) { printf("RETURN\n"); }                          if (yacc_mode) {                        return RETURN; } }
"else"                                      { add_semicolon = 0; if (lex_mode) { printf("ELSE\n"); }                            if (yacc_mode) {                        return ELSE; } }
"for"                                       { add_semicolon = 0; if (lex_mode) { printf("FOR\n"); }                             if (yacc_mode) {                        return FOR; } }
"if"                                        { add_semicolon = 0; if (lex_mode) { printf("IF\n"); }                              if (yacc_mode) {                        return IF; } }

"var"                                       { add_semicolon = 0; if (lex_mode) { printf("VAR\n"); }                             if (yacc_mode) {                        return VAR; } }
"int"                                       { add_semicolon = 0; if (lex_mode) { printf("INT\n"); }                             if (yacc_mode) {                        return INT; } }
"float32"                                   { add_semicolon = 0; if (lex_mode) { printf("FLOAT32\n"); }                         if (yacc_mode) {                        return FLOAT32; } }
"bool"                                      { add_semicolon = 0; if (lex_mode) { printf("BOOL\n"); }                            if (yacc_mode) {                        return BOOL; } }
"string"                                    { add_semicolon = 0; if (lex_mode) { printf("STRING\n"); }                          if (yacc_mode) {                        return STRING; } }
"fmt.Println"                               { add_semicolon = 0; if (lex_mode) { printf("PRINT\n"); }                           if (yacc_mode) {                        return PRINT; } }
"strconv.Atoi"                              { add_semicolon = 0; if (lex_mode) { printf("PARSEINT\n"); }                        if (yacc_mode) {                        return PARSEINT; } }
"func"                                      { add_semicolon = 0; if (lex_mode) { printf("FUNC\n"); }                            if (yacc_mode) {                        return FUNC; } }
"os.Args"                                   { add_semicolon = 0; if (lex_mode) { printf("CMDARGS\n"); }                         if (yacc_mode) {                        return CMDARGS; } }



{digit}+"."{digit}*{e}("-"|"+")?{digit}+    { add_semicolon = 1; if (lex_mode) { printf("DECIMAL(%s)\n", yytext); }             if (yacc_mode) { PASS_TOKEN();          return DECIMAL; } }
{digit}*"."{digit}+{e}("-"|"+")?{digit}+    { add_semicolon = 1; if (lex_mode) { printf("DECIMAL(%s)\n", yytext); }             if (yacc_mode) { PASS_TOKEN();          return DECIMAL; } }
{digit}+{e}("-"|"+")?{digit}+               { add_semicolon = 1; if (lex_mode) { printf("DECIMAL(%s)\n", yytext); }             if (yacc_mode) { PASS_TOKEN();          return DECIMAL; } }
{digit}+"."{digit}*                         { add_semicolon = 1; if (lex_mode) { printf("DECIMAL(%s)\n", yytext); }             if (yacc_mode) { PASS_TOKEN();          return DECIMAL; } }
{digit}*"."{digit}+                         { add_semicolon = 1; if (lex_mode) { printf("DECIMAL(%s)\n", yytext); }             if (yacc_mode) { PASS_TOKEN();          return DECIMAL; } }


{octalsuf}({octal})+                        { add_semicolon = 1; if (lex_mode) { printf("NATURAL(%s)\n", yytext); }             if (yacc_mode) { PASS_TOKEN();          return NATURAL; } }
{hexsuf}({hex})+                            { add_semicolon = 1; if (lex_mode) { printf("NATURAL(%s)\n", yytext); }             if (yacc_mode) { PASS_TOKEN();          return NATURAL; } }
{octalsuf}{octal}*{noctal}+({octal}|{noctal})*  { add_semicolon = 1; printf("Line %d, column %d: invalid octal constant (%s)\n", token_beginline, token_begincolumn, yytext); }
{digit}+                                    { add_semicolon = 1; if (lex_mode) { printf("NATURAL(%s)\n", yytext); }             if (yacc_mode) { PASS_TOKEN();          return NATURAL; } }


{letter}({letter}|{digit})*                 { add_semicolon = 1; if (lex_mode) { printf("IDENTIFIER(%s)\n", yytext);}           if (yacc_mode) { PASS_TOKEN();          return IDENTIFIER; } }
{newline}                                   {token_endline++; token_endcolumn = 1; if(add_semicolon){ add_semicolon = 0; if (lex_mode) { printf("SEMICOLON\n"); }    if (yacc_mode) {                        return SEMICOLON; } } }



"/*"                                        { BEGIN(COMMENT_BLOCK); commentblock_beginline = token_beginline; commentblock_begincolumn = token_begincolumn; }
<COMMENT_BLOCK>{newline}                    { token_endcolumn = 1; token_endline++; }
<COMMENT_BLOCK>.                            { ; }
<COMMENT_BLOCK><<EOF>>                      { BEGIN(INITIAL); printf("Line %d, column %d: unterminated comment\n", commentblock_beginline, commentblock_begincolumn); add_semicolon = 0; }
<COMMENT_BLOCK>"*/"                         { BEGIN(INITIAL); }



"//"                                        { BEGIN(COMMENT_LINE); if (add_semicolon) { add_semicolon = 0; if (lex_mode) { printf("SEMICOLON\n"); } if (yacc_mode) {    return SEMICOLON; } } }
<COMMENT_LINE>.                             { ; }
<COMMENT_LINE>{newline}                     { BEGIN(INITIAL); token_endcolumn = 1; token_endline++; }



"\""                                        { add_semicolon = 0; BEGIN(STRINGLIT); strlit_beginline = token_beginline; strlit_begincolumn = token_endcolumn; strlit_buf_len = 0; strlit_error = 0; }
<STRINGLIT>"\""                             { add_semicolon = 1; BEGIN(INITIAL); if (strlit_error==1) { add_semicolon = 0; }
                                              else if (lex_mode) { print_strlit_buf(); }                                        if (yacc_mode) { yylval.lexeme = strdup(strlit_buf); return STRLIT; } }
<STRINGLIT>"\\"{escape}                     { add_semicolon = 0; add_to_strlit_buf('\\'); add_to_strlit_buf(yytext[1]); }
<STRINGLIT>"\\".                            { add_semicolon = 0; strlit_error = 1; printf("Line %d, column %d: invalid escape sequence (%s)\n", token_beginline, token_endcolumn - yyleng, yytext); if (yacc_mode) { yylval.lexeme = strdup(strlit_buf); return STRLIT; } }
<STRINGLIT>"\\"                             { add_semicolon = 0; strlit_error = 1; printf("Line %d, column %d: invalid escape sequence (%s)\n", token_beginline, token_endcolumn - yyleng, yytext); if (yacc_mode) { yylval.lexeme = strdup(strlit_buf); return STRLIT; } }
<STRINGLIT>{newline}                        { add_semicolon = 0; BEGIN(INITIAL); token_endline++; token_beginline++; token_begincolumn = 1; token_endcolumn = 1; strlit_error = 1; printf("Line %d, column %d: unterminated string literal\n", strlit_beginline, strlit_begincolumn - yyleng); }
<STRINGLIT><<EOF>>                          { add_semicolon = 0; BEGIN(INITIAL); strlit_error = 1; printf("Line %d, column %d: unterminated string literal\n", strlit_beginline, strlit_begincolumn - yyleng); }
<STRINGLIT>.                                { add_semicolon = 0; add_to_strlit_buf(yytext[0]); }



<<EOF>>                                     { YY_USER_ACTION;if (add_semicolon) { add_semicolon = 0; if (lex_mode) { printf("SEMICOLON\n"); }       if (yacc_mode) {    return SEMICOLON; } } return 0; }
.                                           { printf("Line %d, column %d: illegal character (%s)\n", token_beginline, token_begincolumn, yytext); }

%%



extern int yylex();
int main(int argc, char **argv) {

    for (int i=1; i<argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            lex_mode = 1;
            yylex();
        } else if (strcmp(argv[i], "-t") == 0) {
            yacc_mode = 1;
            yyparse();
            if (errors==0) {
                //show(program, 0);
            }
        }
    }
    return 0;
}

void yyerror(char *error) {
    errors++;
    if ((yylval.lexeme != NULL) && (strlit_buf_len!=0)  ) {
        // string errors (need to print string int lexeme)
        printf("Line %d, column %d: %s: \"%s\"\n", token_beginline, token_begincolumn-strlit_buf_len-1, error, yylval.lexeme);
    } else {
        // normal errors
        printf("Line %d, column %d: %s: %s\n", token_beginline, token_begincolumn, error, yytext);
    }
    //printf("Found %s '%s' (%d,%d)\n", error, yytext, token_beginline, token_begincolumn);
}
int yywrap() {  /* called on EOF, return 1 to terminate */
    return 1;
}
