#ifndef _SEMANTICS_H
#define _SEMANTICS_H

#include "ast.h"

//void check_expression(struct node *expression, struct symbol_list *scope);
//void check_parameters(struct node *parameters, struct symbol_list *scope);
int check_program(struct node *program);

struct symbol_list {
	char *identifier;
	enum type type;
	struct node *node;
	struct symbol_list *next;
};

struct symbol_list *insert_symbol(struct symbol_list *symbol_table, char *identifier, enum type type, struct node *node);
struct symbol_list *search_symbol(struct symbol_list *symbol_table, char *identifier);
void show_symbol_table();

#endif
