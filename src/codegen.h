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


void codegen_statement(struct node *statement, int ind);
void codegen_program(struct node *program);


#endif