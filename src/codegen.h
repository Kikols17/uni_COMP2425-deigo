#ifndef _CODEGEN_H
#define _CODEGEN_H

#include "ast.h"


// type                 { bool_type,  int_type,  float32_type,  string_type,  undef_type,  none_type,  EMPTY     };   para referencia
#define TYPE_TO_LLVM    { "i1",       "i32",     "double",      "i8*",        "ERROR_u",   "ERROR_n",  "ERROR_e" }

void codegen_program(struct node *program);

#endif