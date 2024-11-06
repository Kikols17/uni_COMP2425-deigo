%{

    /*
     * Trabalho realizado por:
     *      - Francisco Amado Lapa Marques Silva - uc2022213583
     *      - Miguel Moital Rodrigues Cabral Martins - uc2022213951
     */


    #include "ast.h"

    int yylex(void);
    void yyerror(char *);

    struct node *program;

%}

// union: a C structure that can hold any of a number of different types of data
%union{
    char *lexeme;
    struct node *node;
}

// tokens for lexer's symbols that have no associated value
%token SEMICOLON COMMA BLANKID ASSIGN STAR DIV MINUS PLUS EQ GE GT LE LT LBRACE LPAR LSQ RBRACE RPAR RSQ MOD NE NOT AND OR PACKAGE RETURN ELSE FOR IF VAR INT FLOAT32 BOOL STRING PRINT PARSEINT FUNC CMDARGS

// tokens for lexer's symbols that have associated value
%token <lexeme> RESERVED DECIMAL NATURAL IDENTIFIER STRLIT

// non-terminal symbols
%type <node> Declarations VarDeclaration FuncDeclaration VarSpec Type Parameters FuncBody VarsAndStatements Statement Expr FuncInvocation ParseArgs

%type <node> COMMAIdentifier_s StatementSEMICOLON_s ExprCOMMA_s


%left       LOW
%nonassoc   IF ELSE FOR
%left       COMMA
%right      ASSIGN
%left       OR
%left       AND
%left       LT GT LE GE EQ NE
%left       PLUS MINUS
%left       STAR DIV MOD
%left       LBRACE RBRACE LPAR RPAR LSQ RSQ
%right      NOT




%%

/* PACKAGE IDENTIFIER SEMICOLON Declarations */
program             : PACKAGE IDENTIFIER SEMICOLON Declarations             { ; }
                    ;



/* {VarDeclaration SEMICOLON | FuncDeclaration SEMICOLON} */
Declarations        : Declarations VarDeclaration SEMICOLON                 { ; }
                    | Declarations FuncDeclaration SEMICOLON                { ; }
                    ;



VarDeclaration      : VAR VarSpec                                           { ; }
                    | VAR LPAR VarSpec SEMICOLON RPAR                       { ; }
                    ;



/* IDENTIFIER {COMMA IDENTIFIER} Type */
VarSpec             : IDENTIFIER COMMAIdentifier_s Type                     { ; }
                    ;

COMMAIdentifier_s   : COMMA IDENTIFIER                                      { ; }
                    |                                                       { ; }
                    ;



/* INT | FLOAT32 | BOOL | STRING */
Type                : INT                                                   { ; }
                    | FLOAT32                                               { ; }
                    | BOOL                                                  { , }
                    | STRING                                                { ; }
                    ;


/* FUNC IDENTIFIER LPAR [Parameters] RPAR [Type] FuncBody */
FuncDeclaration     : FUNC IDENTIFIER LPAR Parameters RPAR Type FuncBody    { ; }
                    | FUNC IDENTIFIER LPAR RPAR Type FuncBody               { ; }
                    | FUNC IDENTIFIER LPAR Parameters RPAR FuncBody         { ; }
                    | FUNC IDENTIFIER LPAR RPAR FuncBody                    { ; }
                    ;



/* IDENTIFIER Type {COMMA IDENTIFIER Type} */
Parameters          : IDENTIFIER Type                                       { ; }
                    | Parameters COMMA IDENTIFIER Type                      { ; }
                    ;



/* LBRACE VarsAndStatements RBRACE */
FuncBody            : LBRACE VarsAndStatements RBRACE                       { ; }
                    ;



/* VarsAndStatements −→ VarsAndStatements [VarDeclaration | Statement] SEMICOLON */
VarsAndStatements   : VarsAndStatements SEMICOLON                           { ; }
                    | VarsAndStatements VarDeclaration SEMICOLON            { ; }
                    | VarsAndStatements Statement SEMICOLON                 { ; }
                    ;



Statement           : IDENTIFIER ASSIGN Expr                                { ; }
                    | LBRACE StatementSEMICOLON_s RBRACE                    { ; }

                    /* IF Expr LBRACE {Statement SEMICOLON} RBRACE [ELSE LBRACE {Statement SEMICOLON} RBRACE] */
                    | IF Expr LBRACE StatementSEMICOLON_s RBRACE
                      ELSE LBRACE StatementSEMICOLON_s RBRACE %prec LOW     { ; }
                    | IF Expr LBRACE StatementSEMICOLON_s RBRACE %prec LOW  { ; }

                    /* FOR [Expr] LBRACE {Statement SEMICOLON} RBRACE */
                    | FOR Expr LBRACE StatementSEMICOLON_s RBRACE           { ; }
                    | FOR LBRACE StatementSEMICOLON_s RBRACE                { ; }
                    
                    /* RETURN [Expr] */
                    | RETURN Expr                                           { ; }
                    | RETURN                                                { ; }

                    | FuncInvocation                                        { ; }
                    | ParseArgs                                             { ; }

                    /* PRINT LPAR (Expr | STRLIT) RPAR */
                    | PRINT LPAR Expr RPAR                                  { ; }
                    | PRINT LPAR STRLIT RPAR                                { ; }
                    ;

StatementSEMICOLON_s: StatementSEMICOLON_s Statement SEMICOLON              { ; }
                    |                                                       { ; }
                    ;



/* IDENTIFIER COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ Expr RSQ RPAR */
ParseArgs           : IDENTIFIER COMMA BLANKID ASSIGN PARSEINT
                      LPAR CMDARGS LSQ Expr RSQ RPAR                        { ; }
                    ;



/* IDENTIFIER LPAR [Expr {COMMA Expr}] RPAR */
FuncInvocation      : IDENTIFIER LPAR ExprCOMMA_s RPAR                      { ; }
                    ;

/* [Expr {COMMA Expr}] */
ExprCOMMA_s         : Expr                                                  { ; }
                    | Expr COMMA ExprCOMMA_s                                { ; }
                    |                                                       { ; }
                    ;



                    /* Expr (OR | AND) Expr */
Expr                : Expr OR Expr                                          { ; }
                    | Expr AND Expr                                         { ; }

                    /* Expr (LT | GT | EQ | NE | LE | GE) Expr */
                    | Expr LT Expr                                          { ; }
                    | Expr GT Expr                                          { ; }
                    | Expr EQ Expr                                          { ; }
                    | Expr NE Expr                                          { ; }
                    | Expr LE Expr                                          { ; }
                    | Expr GE Expr                                          { ; }

                    /* Expr (PLUS | MINUS | STAR | DIV | MOD) Expr */
                    | Expr PLUS Expr                                        { ; }
                    | Expr MINUS Expr                                       { ; }
                    | Expr STAR Expr                                        { ; }
                    | Expr DIV Expr                                         { ; }
                    | Expr MOD Expr                                         { ; }

                    /* (NOT | MINUS | PLUS) Expr */
                    | NOT Expr %prec LOW                                    { ; }
                    | MINUS Expr %prec LOW                                  { ; }
                    | PLUS Expr %prec LOW                                   { ; }

                    /* NATURAL | DECIMAL | IDENTIFIER | FuncInvocation | LPAR Expr RPAR */
                    | NATURAL                                               { ; }
                    | DECIMAL                                               { ; }
                    | IDENTIFIER                                            { ; }
                    | FuncInvocation                                        { ; } 
                    | LPAR Expr RPAR                                        { ; }
                    ;

%%