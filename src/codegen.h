#ifndef _CODEGEN_H
#define _CODEGEN_H

#include "ast.h"


// type                 { bool_type,  int_type,  float32_type,  string_type,  undef_type,  none_type,  EMPTY     };   para referencia
#define TYPE_TO_LLVM    { "i1",       "i32",     "double",      "i8*",        "ERROR_u",   "ERROR_n",  "ERROR_e" }

// category                 { "Program", "VarDecl", "FuncDecl", "FuncHeader", "FuncParams", "FuncBody", "ParamDecl", "Assign", "Int", "Natural", "Decimal", "Float32", "Bool", "String", "StrLit", "Identifier", "For", "If", "Block", "Call", "Return", "Print", "ParseArgs",           "Or", "And",  "Eq", "Ne",  "Lt", "Le", "Gt", "Ge",  "Add", "Sub", "Mul", "Div", "Mod",  "Not", "Minus", "Plus",     "TEMP", "None" };
#define CATEGORY_TO_LLVM    { "",        "",        "",         "",           "",           "",         "",          "",       "",    "",        "",        "",        "",     "",       "",       "",           "",    "",   "",      "",     "",       "",      "",                    "",   "",     "",   "",    "",   "",   "",   "",    "add", "sub", "mul", "div", "mod",  "",    "",      "",         "",     ""     }


void codegen_statement(struct node *statement, int ind);
void codegen_program(struct node *program);


#endif