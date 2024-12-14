#ifndef _CODEGEN_H
#define _CODEGEN_H

#include "semantics.h"
#include "ast.h"

/*
 * Trabalho realizado por:
 *      - Francisco Amado Lapa Marques Silva - uc2022213583
 *      - Miguel Moital Rodrigues Cabral Martins - uc2022213951
 */

// type                 { bool_type,  int_type,  float32_type,  string_type,                                                                    undef_type,  none_type,  EMPTY    };   para referencia
#define TYPE_TO_LLVM    { "i1",       "i32",     "double",      "i8*",                                                                          "ERROR_u",   "void",    "ERROR_e" }
#define EMPTY_TYPE_LLVM { "0",        "0",       "0.0",         "getelementptr inbounds ([1 x i8], [1 x i8]* @.str.empty_str, i32 0, i32 0)",   "ERROR_u",   "",        "ERROR_e" }

// category                 { "Program", "VarDecl", "FuncDecl", "FuncHeader", "FuncParams", "FuncBody", "ParamDecl", "Assign", "Int", "Natural", "Decimal", "Float32", "Bool", "String", "StrLit", "Identifier", "For", "If", "Block", "Call", "Return", "Print", "ParseArgs",           "Or", "And",  "Eq", "Ne",  "Lt", "Le", "Gt", "Ge",  "Add", "Sub", "Mul", "Div", "Mod",  "Not", "Minus", "Plus",     "TEMP", "None" };
#define CATEGORY_TO_LLVM    { "",        "",        "",         "",           "",           "",         "",          "",       "",    "",        "",        "",        "",     "",       "",       "",           "",    "",   "",      "",     "",       "",      "",                    "or", "and",  "eq", "ne",  "lt", "le", "gt", "ge",  "add", "sub", "mul", "div", "rem", "",    "",      "",         "",     ""     }


void codegen_indent(int ind);

void codegen_globalvar(struct node *vardecl, int ind);
void codegen_localvar(struct node *vardecl, int ind);

void codegen_expression(struct node *expression, int ind);
void codegen_block(struct node *block, int ind);

void codegen_call(struct node *return_node, int ind);
void codegen_assign(struct node *assign_node, int ind);
void codegen_return(struct node *return_node, int ind);
void codegen_print(struct node *print_node, int ind);
void codegen_if(struct node *if_node, int ind);
void codegen_for(struct node *for_node, int ind);
void codegen_parseargs(struct node *parseargs, int ind);
void codegen_statement(struct node *statement, int ind);

void codegen_funcheaderparams(struct node *params);
void codegen_funcheader_localvars(struct node *params_node, int ind);
struct node *codegen_funcheader_main(struct node *funcheader, int ind);
struct node *codegen_funcheader(struct node *funcheader, int ind);
void codegen_funcbody(struct node *funcbody, int ind);
void codegen_function(struct node *node, int ind);

int codegen_recur_stringdecl(struct node *node, int ind, int str_count);

void codegen_program(struct node *program);


#endif