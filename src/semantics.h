#ifndef _SEMANTICS_H
#define _SEMANTICS_H

#include <stdbool.h>
#include "ast.h"

struct symbol_list {
    char *identifier;
    enum type type;
    struct node *node;
    bool is_function;
    bool is_param;
    bool is_invalid;
    struct symbol_list *child_scope;
    struct symbol_list *parent_scope;
    struct symbol_list *next;
};

//void check_expression(struct node *expression, struct symbol_list *scope);
//void check_parameters(struct node *parameters, struct symbol_list *scope);
//void check_FuncDecl(struct node *decleration);
//void check_VarDecl(struct node *decleration);

void check_If(struct node *if_node, struct symbol_list *symbol_scope);
void check_Return(struct node *return_node, struct symbol_list *symbol_scope);
void check_Assign(struct node *assign, struct symbol_list *symbol_scope);
void check_Call(struct node *call, struct symbol_list *symbol_list);

void check_Statement(struct node *statement, struct symbol_list *symbol_list);

void check_VarDecl(struct node *vardecl, struct symbol_list *symbol_func);

void check_FuncDecl(struct node *declaration, struct symbol_list *symbol_global_scope);
void check_FuncParams(struct node *params, struct symbol_list *symbol_func);
void check_FuncBody(struct node *body, struct symbol_list *symbol_func);

void check_Statement(struct node *statement, struct symbol_list *symbol_list);

int check_program(struct node *program);



struct symbol_list *insert_symbol(struct symbol_list *symbol_table, char *identifier, enum type type, struct node *node);
struct symbol_list *search_symbol(struct symbol_list *symbol_table, char *identifier, int depth, bool is_function);
void show_symbol_table();

char *show_functionparameters(struct symbol_list *symbol);

#endif
