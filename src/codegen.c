#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantics.h"
#include "codegen.h"

const char *type_to_llvm3[] = TYPE_TO_LLVM;
const enum type category_to_type3[] = CATEGORY_TO_TYPE;

int temporary;   // sequence of temporary registers in a function
int if_count = 0;
int for_count = 0;


void codegen_indent(int ind) {
    for (int i=0; i<ind; i++) {
        printf(" ");
    }
}



void codegen_globalvar(struct node *vardecl, struct symbol_list *symbol, int ind) {
    struct node *type_node = getchild(vardecl, 0);
    struct node *id_node = getchild(vardecl, 1);
    codegen_indent(ind);
    printf("%s @%s\n", type_to_llvm3[category_to_type3[type_node->category]], id_node->token);
}



void codegen_funcheaderparams(struct node *params, struct symbol_list *symbol_scope) {
    struct node *cur_param;
    int curr = 0;
    while((cur_param = getchild(params, curr++)) != NULL) {
        struct node *type_node = getchild(cur_param, 0);
        struct node *id_node = getchild(cur_param, 1);
        if (curr > 1) {
            printf(", ");
        }
        printf("%s %%%s", type_to_llvm3[category_to_type3[type_node->category]], id_node->token);
    }
}


void codegen_funcheader(struct node *funcheader, struct symbol_list *symbol_scope, int ind) {

    struct node *id_node, *type_node, *params_node;
    id_node = getchild(funcheader, 0);
    params_node = getchild(funcheader, 2);
    if (params_node!=NULL) {
        type_node = getchild(funcheader, 1);
    } else {
        type_node = NULL;
        params_node = getchild(funcheader, 1);
    }

    codegen_indent(ind);
    if (type_node!=NULL) {
        printf("define %s @_%s(", type_to_llvm3[category_to_type3[type_node->category]], id_node->token);
    } else {
        printf("define void @_%s(", id_node->token);
    }
    codegen_funcheaderparams(params_node, symbol_scope);
    printf(")");
}

void codegen_funcbody(struct node *funcbody, struct symbol_list *symbol_scope, int ind) {

}


void codegen_function(struct node *node, struct symbol_list *symbol, int ind) {
    // declare the function
    struct symbol_list *func_scope = symbol->child_scope;

    struct node *funcheader, *funcbody;
    funcheader = getchild(node, 0);
    funcbody = getchild(node, 1);

    codegen_funcheader(funcheader, func_scope, ind);
    codegen_indent(ind); printf("{\n");
    codegen_funcbody(funcbody, func_scope, ind+1);
    codegen_indent(ind); printf("}\n");
}


void codegen_program(struct node *program, struct symbol_list *table) {
    printf("; bem fixe este programa\n");

    printf("; ----- Global variables -----\n");
    // setup all the global variables
    struct symbol_list *symbol;
    for(symbol = table->next; symbol != NULL; symbol = symbol->next) {
        if (!symbol->is_function) {
            codegen_globalvar(symbol->node, symbol, 0);
        }
    }
    printf("\n\n; ----- Functions -----\n");

    // setup all the functions
    for(symbol = table->next; symbol != NULL; symbol = symbol->next) {
        if (symbol->is_function) {
            codegen_function(symbol->node, symbol, 0);
            printf("\n");
        }
    }

    // add entry point
    printf("\n; ----- Entry point -----\n"
           "define i32 @main() {\n"
           "  %%1 = call i32 @_main(i32 0)\n"
           "  ret i32 %%1\n"
           "}\n");

}