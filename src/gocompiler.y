
/*
 * Trabalho realizado por:
 *      - Francisco Amado Lapa Marques Silva - uc2022213583
 *      - Miguel Moital Rodrigues Cabral Martins - uc2022213951
 */

%{

    #include "ast.h"
    #include <stddef.h>   // preciso para o NULL (n sei pq nunca foi preciso)

    int yylex(void);
    void yyerror(char *);

    struct node *program = NULL;

    // global node pointer to organize the VarSpec
    #ifndef VARSPEC_HEAD
    #define VARSPEC_HEAD
    struct node *varspec_head = NULL;
    #endif

    //int yydebug = 1;

%}

%debug

// union: a C structure that can hold any of a number of different types of data
%union{
    char *lexeme;
    struct node *node;
}

// tokens for lexer's symbols that have no associated value
%token SEMICOLON COMMA BLANKID ASSIGN STAR DIV MINUS PLUS EQ GE GT LE LT LBRACE LPAR LSQ RBRACE RPAR RSQ MOD NE NOT AND OR PACKAGE RETURN ELSE FOR IF VAR INT FLOAT32 BOOL STRING PRINT PARSEINT FUNC CMDARGS

// tokens for lexer's symbols that have associated value
%token <lexeme> RESERVED DECIMAL NATURAL IDENTIFIER STRLIT

// symbols
%type <node> Program Declarations VarDeclaration FuncDeclaration VarSpec Type Parameters FuncBody VarsAndStatements Statement Expr FuncInvocation ParseArgs COMMAIdentifier_s StatementSEMICOLON_s ExprCOMMA_s parameters_q type_q else_q expr_q

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

//%debug



%%

/* PACKAGE IDENTIFIER SEMICOLON Declarations */
Program             : PACKAGE IDENTIFIER SEMICOLON Declarations             { 
                        $$=program=newnode(Program,NULL);
                        addchild($$, $4);
                    }
                    ;



/* {VarDeclaration SEMICOLON | FuncDeclaration SEMICOLON} */
Declarations        : Declarations VarDeclaration SEMICOLON                 {
                        $$ = $1;

                        struct node* aux_vardeclaration = newnode(TEMP, NULL);
                        addchild($$, aux_vardeclaration);
                        addchild(aux_vardeclaration, $2);
                    }
                    | Declarations FuncDeclaration SEMICOLON                {
                        $$ = $1;
                        addchild($1, $2);
                    }
                    |                                                       {
                        $$ = newnode(TEMP, NULL);
                    }
                    ;



VarDeclaration      : VAR VarSpec                                           {
                        $$ = $2;
                    }
                    | VAR LPAR VarSpec SEMICOLON RPAR                       {
                        $$ = $3;
                    }
                    ;



/* IDENTIFIER {COMMA IDENTIFIER} Type */
VarSpec             : IDENTIFIER COMMAIdentifier_s Type                     {
                        //struct node* vardecl = newnode(VarDecl, NULL);      // this is the VarDecl that HAS to exist (all others are opt)
                        //addchild($$, vardecl);
                        //addchild(vardecl, varspec_head);                          // VarDecl abaixo
                        //addchild(vardecl, newnode(Identifier, $1)); // this Identifier


                        // adicionar $3 a todos os filhos VarDecl de $$ (recursivo) (incluindo vardecl)
                        reversenode(varspec_head);


                        if (varspec_head == NULL) {
                            varspec_head = newnode(TEMP, NULL);
                            struct node* vardecl = newnode(VarDecl, NULL);
                            addchild(varspec_head, vardecl);
                            addchild(vardecl, newnode(Identifier, $1));
                            addchild(vardecl, $3);
                            $$ = varspec_head;
                        } else {
                            struct node* vardecl = newnode(VarDecl, NULL);
                            addchild(varspec_head, vardecl);
                            addchild(vardecl, newnode(Identifier, $1));
                            addchild(vardecl, $3);
                            $$ = varspec_head;
                        }


                        addchild_allchildren(varspec_head, $3);


                        // agora é preciso rodar toda a arvore a partir de varspec_head, por causa da ordem
                        reversenode(varspec_head);





                        varspec_head = NULL;        // reset varspec_head for next time :)))
                    }
                    ;

COMMAIdentifier_s   : COMMAIdentifier_s COMMA IDENTIFIER                    {
                        if (varspec_head == NULL) {
                            varspec_head = newnode(TEMP, NULL);
                            struct node* vardecl = newnode(VarDecl, NULL);
                            addchild(varspec_head, vardecl);
                            addchild(vardecl, newnode(Identifier, $3));
                            $$ = varspec_head;
                        } else {
                            struct node* vardecl = newnode(VarDecl, NULL);
                            addchild(varspec_head, vardecl);
                            addchild(vardecl, newnode(Identifier, $3));
                            $$ = varspec_head;
                        }

                        //$$ = newnode(VarDecl, NULL);
                        //addchild($$, $1);
                        //addchild($$, newnode(Identifier, $3));

                        //$$ = $1;
                        //struct node* vardecl = newnode(VarDecl, NULL);
                        //addchild(vardecl, newnode(Identifier, $3));
                        //addchild($$, vardecl);
                    }
                    |                                                       {
                        if (varspec_head == NULL) {
                            varspec_head = newnode(TEMP, NULL);
                            addchild(varspec_head, newnode(TEMP, NULL));
                            $$ = varspec_head;
                        } else {
                            addchild(varspec_head, newnode(TEMP, NULL));
                            $$ = varspec_head;
                        }
                    }
                    ;



/* INT | FLOAT32 | BOOL | STRING */
Type                : INT                                                   {
                        $$ = newnode(Int, NULL);
                    }
                    | FLOAT32                                               {
                        $$ = newnode(Float32, NULL);
                    }
                    | BOOL                                                  {
                        $$ = newnode(Bool, NULL);
                    }
                    | STRING                                                {
                        $$ = newnode(String, NULL);
                    }
                    ;


/* FUNC IDENTIFIER LPAR [Parameters] RPAR [Type] FuncBody 
FuncDeclaration     : FUNC IDENTIFIER LPAR RPAR FuncBody                    { ; }
                    | FUNC IDENTIFIER LPAR Parameters RPAR FuncBody         { ; }
                    | FUNC IDENTIFIER LPAR RPAR Type FuncBody               { ; }
                    | FUNC IDENTIFIER LPAR Parameters RPAR Type FuncBody    { ; }
                    ;
*/
// acima não funciona, talves tentar isto:
FuncDeclaration     : FUNC IDENTIFIER LPAR parameters_q RPAR type_q FuncBody    {
                        $$ = newnode(FuncDecl, NULL);

                        struct node* header = newnode(FuncHeader, NULL);
                        addchild($$, header);
                        addchild(header, newnode(Identifier, $2));
                        addchild(header, $6);
                        addchild(header, $4);   // because order

                        addchild($$, $7);
                    }
                    ;

parameters_q       : Parameters                                            {
                        $$ = $1;
                    }
                    |                                                       {
                        $$ = newnode(FuncParams, NULL);
                    }
                    ;

type_q             : Type                                                  {
                        $$ = $1;
                    }
                    |                                                       {
                        $$ = newnode(TEMP, NULL);
                    }
                    ;




/* IDENTIFIER Type {COMMA IDENTIFIER Type} */
Parameters          : Parameters COMMA IDENTIFIER Type                      {

                        $$ = $1;
                        struct node* paramdecl = newnode(ParamDecl, NULL);
                        addchild($$, paramdecl);
                        addchild(paramdecl, $4);
                        addchild(paramdecl, newnode(Identifier, $3));
                    }
                    | IDENTIFIER Type                                       {
                        $$ = newnode(FuncParams, NULL);
                        struct node* paramdecl = newnode(ParamDecl, NULL);
                        addchild($$, paramdecl);
                        addchild(paramdecl, $2);
                        addchild(paramdecl, newnode(Identifier, $1));
                    }
                    ;



/* LBRACE VarsAndStatements RBRACE */
FuncBody            : LBRACE VarsAndStatements RBRACE                       {
                        $$ = newnode(FuncBody, NULL);
                        addchild($$, $2);
                    }
                    | LBRACE error RBRACE                                   {
                        $$ = newnode(TEMP, NULL);
                    }
                    ;



/* VarsAndStatements [VarDeclaration | Statement] SEMICOLON */
VarsAndStatements   : VarsAndStatements SEMICOLON                           {
                        $$ = $1;
                    }
                    | VarsAndStatements VarDeclaration SEMICOLON            {
                        $$ = $1;
                        addchild($$, $2);
                    }
                    | VarsAndStatements Statement SEMICOLON                 {
                        $$ = $1;
                        addchild($$, $2);
                    }
                    |                                                       {
                        $$ = newnode(TEMP, NULL);
                    }
                    ;



Statement           : IDENTIFIER ASSIGN Expr                                {
                        $$ = newnode(Assign, NULL);
                        addchild($$, newnode(Identifier, $1));
                        addchild($$, $3);
                    }
                    /*
                    | IDENTIFIER LSQ Expr RSQ ASSIGN Expr                    {
                        $$ = newnode(TEMP, NULL);
                    }
                    */
                    | LBRACE StatementSEMICOLON_s RBRACE                    {
                        if (countchildren($2) <= 1) {
                            $$ = $2;
                        } else {
                            $$ = newnode(Block, NULL);
                            addchild($$, $2);
                        }
                    }

                    /* IF Expr LBRACE {Statement SEMICOLON} RBRACE [ELSE LBRACE {Statement SEMICOLON} RBRACE] */
                    | IF Expr LBRACE StatementSEMICOLON_s
                      RBRACE else_q %prec LOW     {
                        $$ = newnode(If, NULL);
                        addchild($$, $2);

                        struct node* if_block = newnode(Block, NULL);
                        addchild(if_block, $4);
                        addchild($$, if_block);

                        struct node* else_block = newnode(Block, NULL);
                        addchild(else_block, $6);
                        addchild($$, else_block);
                    }
                    /* FOR [Expr] LBRACE {Statement SEMICOLON} RBRACE */
                    | FOR expr_q LBRACE StatementSEMICOLON_s RBRACE           {
                        $$ = newnode(For, NULL);
                        addchild($$, $2);
                        struct node* for_block = newnode(Block, NULL);
                        addchild(for_block, $4);
                        addchild($$, for_block);
                    }
                    //| FOR LBRACE StatementSEMICOLON_s RBRACE                {
                    //    $$ = newnode(For, NULL);
                    //    addchild($$, $3);
                    //    $$ = newnode(TEMP, NULL);
                    //}
                    /* RETURN [Expr] */
                    | RETURN expr_q                                           {
                        $$ = newnode(Return, NULL);
                        addchild($$, $2);
                    }
                    //| RETURN                                                {
                    //    $$ = newnode(Return, NULL);
                    //    addchild($$, newnode(TEMP, NULL));
                    //}
                    | FuncInvocation                                        {
                        $$ = $1;
                    }
                    | ParseArgs                                             {
                        $$ = $1;
                    }

                    /* PRINT LPAR (Expr | STRLIT) RPAR */
                    | PRINT LPAR Expr RPAR                                  {
                        $$ = newnode(Print, NULL);
                        addchild($$, $3);
                    }
                    | PRINT LPAR STRLIT RPAR                                {
                        $$ = newnode(Print, NULL);
                        addchild($$, newnode(StrLit, $3));
                    }
                    | error                                                 {
                        $$ = newnode(TEMP, NULL);
                    }
                    ;

expr_q             : Expr                                                  {
                        $$ = $1;
                    }
                    |                                                       {
                        $$ = newnode(TEMP, NULL);
                    }

else_q             : ELSE LBRACE StatementSEMICOLON_s RBRACE               {
                        $$ = $3;
                    }
                    |                                                       {
                        $$ = newnode(TEMP, NULL);
                    }

StatementSEMICOLON_s: StatementSEMICOLON_s Statement SEMICOLON              {
                        $$ = $1;
                        addchild($$, $2);
                    }
                    |                                                       {
                        $$ = newnode(TEMP, NULL);
                    }
                    ;



                    /* IDENTIFIER COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ Expr RSQ RPAR */
ParseArgs           : IDENTIFIER COMMA BLANKID ASSIGN PARSEINT
                      LPAR CMDARGS LSQ Expr RSQ RPAR                        {
                        $$ = newnode(ParseArgs, NULL);
                        addchild($$, newnode(Identifier, $1));
                        addchild($$, $9);
                      }
                      /* IDENTIFIER COMMA BLANKID ASSIGN PARSEINT LPAR error RPAR */
                    | IDENTIFIER COMMA BLANKID ASSIGN PARSEINT
                      LPAR error RPAR                                       {
                        $$ = newnode(TEMP, NULL);
                    }
                    ;



                    /* IDENTIFIER LPAR [Expr {COMMA Expr}] RPAR */
FuncInvocation      : IDENTIFIER LPAR ExprCOMMA_s RPAR                      {
                        $$ = newnode(Call, NULL);
                        addchild($$, newnode(Identifier, $1));

                        struct node *aux_params = newnode(TEMP, NULL);
                        addchild($$, aux_params);
                        addchild($$, $3);
                    }
                    /* IDENTIFIER LPAR error RPAR */
                    | IDENTIFIER LPAR error RPAR                            {
                        $$ = newnode(TEMP, NULL);
                    }
                    ;

/* [Expr {COMMA Expr}] */
ExprCOMMA_s         : Expr                                                  {
                        //$$ = $1;
                        $$ = newnode(TEMP, NULL);
                        addchild($$, $1);
                    }
                    | Expr COMMA ExprCOMMA_s                                {
                        $$ = newnode(TEMP, NULL);
                        addchild($$, $1);
                        addchild($$, $3);
                    }
                    |                                                       {
                        $$ = newnode(TEMP, NULL);
                    }
                    ;



                    /* Expr (OR | AND) Expr */
Expr                : Expr OR Expr                                          {
                        $$ = newnode(Or, NULL);
                        addchild($$, $1);
                        addchild($$, $3);
                    }
                    | Expr AND Expr                                         {
                        $$ = newnode(And, NULL);
                        addchild($$, $1);
                        addchild($$, $3);
                    }

                    /* Expr (LT | GT | EQ | NE | LE | GE) Expr */
                    | Expr LT Expr                                          {
                        $$ = newnode(Lt, NULL);
                        addchild($$, $1);
                        addchild($$, $3);
                    }
                    | Expr GT Expr                                          {
                        $$ = newnode(Gt, NULL);
                        addchild($$, $1);
                        addchild($$, $3);
                    }
                    | Expr EQ Expr                                          {
                        $$ = newnode(Eq, NULL);
                        addchild($$, $1);
                        addchild($$, $3);
                    }
                    | Expr NE Expr                                          {
                        $$ = newnode(Ne, NULL);
                        addchild($$, $1);
                        addchild($$, $3);
                    }
                    | Expr LE Expr                                          {
                        $$ = newnode(Le, NULL);
                        addchild($$, $1);
                        addchild($$, $3);
                    }
                    | Expr GE Expr                                          {
                        $$ = newnode(Ge, NULL);
                        addchild($$, $1);
                        addchild($$, $3);
                    }

                    /* Expr (PLUS | MINUS | STAR | DIV | MOD) Expr */
                    | Expr PLUS Expr                                        {
                        $$ = newnode(Add, NULL);
                        addchild($$, $1);
                        addchild($$, $3);
                    }
                    | Expr MINUS Expr                                       {
                        $$ = newnode(Sub, NULL);
                        addchild($$, $1);
                        addchild($$, $3);
                    }
                    | Expr STAR Expr                                        {
                        $$ = newnode(Mul, NULL);
                        addchild($$, $1);
                        addchild($$, $3);
                    }
                    | Expr DIV Expr                                         {
                        $$ = newnode(Div, NULL);
                        addchild($$, $1);
                        addchild($$, $3);
                    }
                    | Expr MOD Expr                                         {
                        $$ = newnode(Mod, NULL);
                        addchild($$, $1);
                        addchild($$, $3);
                    }

                    /* (NOT | MINUS | PLUS) Expr (CUIDADO COM ORGEM)*/
                    | MINUS Expr %prec NOT                                  {
                        $$ = newnode(Minus, NULL);
                        addchild($$, $2);
                    }
                    | NOT Expr %prec NOT                                    {
                        $$ = newnode(Not, NULL);
                        addchild($$, $2);
                    }
                    | PLUS Expr %prec NOT                                   {
                        $$ = newnode(Plus, NULL);
                        addchild($$, $2);
                    }

                    /* NATURAL | DECIMAL | IDENTIFIER | FuncInvocation | LPAR Expr RPAR */
                    | NATURAL                                               {
                        $$ = newnode(Natural, $1);
                    }
                    | DECIMAL                                               {
                        $$ = newnode(Decimal, $1);
                    }
                    | IDENTIFIER                                            {
                        $$ = newnode(Identifier, $1);
                    }
                    | FuncInvocation                                        {
                        $$ = $1;
                    } 
                    | LPAR Expr RPAR                                        {
                        $$ = $2;
                    }

                    /* LPAR error RPAR */
                    | LPAR error RPAR                                       {
                        $$ = newnode(TEMP, NULL);
                    }
                    ;

%%