#ifndef _SEMANTICS_H
#define _SEMANTICS_H

#include <stdbool.h>
#include "ast.h"

//void check_expression(struct node *expression, struct symbol_list *scope);
//void check_parameters(struct node *parameters, struct symbol_list *scope);
//void check_FuncDecl(struct node *decleration);
//void check_VarDecl(struct node *decleration);
int check_program(struct node *program);

struct symbol_list {
    char *identifier;
    enum type type;
    struct node *node;
    bool is_param;
    bool is_invalid;
    struct symbol_list *child_scope;
    struct symbol_list *parent_scope;
    struct symbol_list *next;
};

struct symbol_list *insert_symbol(struct symbol_list *symbol_table, char *identifier, enum type type, struct node *node);
struct symbol_list *search_symbol(struct symbol_list *symbol_table, char *identifier, int depth);
void show_symbol_table();

char *show_functionparameters(struct symbol_list *symbol);

#endif
