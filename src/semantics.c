#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantics.h"

int semantic_errors = 0;

struct symbol_list *symbol_table;

const char *category_names2[] = CATEGORY_NAMES;
const char *type_names2[] = TYPE_NAMES;
const enum type category_to_type2[] = CATEGORY_TO_TYPE;


void check_expression(struct node *expression, struct symbol_list *symbol_scope) {
    char token_buff = '\0';
    switch(expression->category) {
        case Identifier:
            // check if has been declared
            ;   // ?? WHY
            struct symbol_list *declaration = search_symbol(symbol_scope, expression->token, -1, false);
            if (declaration==NULL) {
                printf("Line %d, column %d: Cannot find symbol %s\n", expression->token_line, expression->token_column, expression->token);
                expression->type = undef_type;
            } else {
                expression->type = declaration->type;
            }
            
            break;

        case Int:
        case Natural:
        case Float32:
        case Decimal:
        case Bool:
        case String:
        case StrLit:
            expression->type = category_to_type2[expression->category];
            break;
        
        case Add:
            if (token_buff=='\0') { token_buff = '+'; }
        case Sub:
            if (token_buff=='\0') { token_buff = '-'; }
        case Mul:
            if (token_buff=='\0') { token_buff = '*'; }
        case Div:
            if (token_buff=='\0') { token_buff = '/'; }
        case Mod:
            if (token_buff=='\0') { token_buff = '%'; }
            ;       // ????
            struct node *left_expr = getchild(expression, 0);
            struct node *right_expr = getchild(expression, 1);

            check_expression(left_expr, symbol_scope);
            check_expression(right_expr, symbol_scope);

            if (left_expr->type == right_expr->type) {
                expression->type = left_expr->type;
            } else {
                printf("Line %d, column %d: Operator %c cannot be applied to types %s, %s\n", expression->token_line, expression->token_column, token_buff, type_names2[left_expr->type], type_names2[right_expr->type]);
                expression->type = undef_type;
            }
            break;

        default:
            check_Statement(expression, symbol_scope);
            break;
    }
}

//void check_parameters(struct node *parameters, struct symbol_list *scope) {
//    struct node_list *parameter = parameters->children;
//    while((parameter = parameter->next) != NULL) {
//        struct node *id = getchild(parameter->node, 1);
//        enum type type = category_type(getchild(parameter->node, 0)->category);
//        if(search_symbol(symbol_table, id->token) == NULL) {
//            insert_symbol(symbol_table, id->token, type, parameter->node);
//            insert_symbol(scope, id->token, type, parameter->node);
//        } else {
//            printf("Identifier %s (%d:%d) already declared\n", id->token, id->token_line, id->token_column);
//            semantic_errors++;
//        }
//    }
//}

//void check_function(struct node *function) {
//    struct node *id = getchild(function, 0);
//    if(search_symbol(symbol_table, id->token) == NULL) {
//        insert_symbol(symbol_table, id->token, none_type, function);
//    } else {
//        printf("Identifier %s (%d:%d) already declared\n", id->token, id->token_line, id->token_column);
//        semantic_errors++;
//    }
//    struct symbol_list *scope = (struct symbol_list *) malloc(sizeof(struct symbol_list));
//    scope->next = NULL;
//    check_parameters(getchild(function, 1), scope);
//    check_expression(getchild(function, 2), scope);
//    /* ToDo: scope should be free'd */
//}

void check_If(struct node *if_node, struct symbol_list *symbol_scope) {

}

void check_Print(struct node *print_node, struct symbol_list *symbol_scope) {
    struct symbol_list *definition;
    if (getchild(print_node, 0)->category == Identifier) {
        // if is identifier, make sure it has been defined
        definition = search_symbol(symbol_scope, getchild(print_node, 0)->token, -1, false);
        if (definition==NULL) {
            // was not defined
            getchild(print_node, 0)->type = undef_type;
            printf("Line %d, column %d: Cannot find symbol %s\n", getchild(print_node, 0)->token_line, getchild(print_node, 0)->token_column, getchild(print_node, 0)->token);
            return;
        }
        getchild(print_node, 0)->type = definition->type;
    } else {
        //getchild(print_node, 0)->type = category_to_type2[getchild(print_node, 0)->category];
        check_expression(getchild(print_node, 0), symbol_scope);
    }
}

void check_Return(struct node *return_node, struct symbol_list *symbol_scope) {
    struct symbol_list *symbol_return = search_symbol(symbol_scope, "return", 1, false);
    symbol_return->node = return_node;
    // assume there is always a return symbol (already goofed up if not)

    if (getchild(return_node, 0)==NULL) {
        if (symbol_return->type != none_type) {
            printf("Line %d, column %d: Incompatible type void in return statement\n", return_node->token_line, return_node->token_column);
        }
    } else {
        check_expression(getchild(return_node, 0), symbol_scope);
        if (symbol_return->type != getchild(return_node, 0)->type) {
            printf("Line %d, column %d: Incompatible type %s in return statement\n", getchild(return_node, 0)->token_line, getchild(return_node, 0)->token_column, type_names2[getchild(return_node, 0)->type]);
        }
    }
}

void check_Assign(struct node *assign, struct symbol_list *symbol_scope) {
    // check variable
    struct symbol_list *definition = search_symbol(symbol_scope, getchild(assign, 0)->token, -1, false);
    if (definition!=NULL) {
        getchild(assign, 0)->type = definition->type;
    } else {
        getchild(assign, 0)->type = undef_type;
        printf("Line %d, column %d: Cannot find symbol %s\n", getchild(assign, 0)->token_line, getchild(assign, 0)->token_column, getchild(assign, 0)->token);
    }

    // check statement
    check_expression(getchild(assign, 1), symbol_scope);
    
    // check if assign is legal
    if (getchild(assign, 0)->type != getchild(assign, 1)->type) {
        printf("Line %d, column %d: Operator = cannot be applied to types %s, %s\n", assign->token_line, assign->token_column, type_names2[getchild(assign, 0)->type], type_names2[getchild(assign, 1)->type]);
        assign->type = undef_type;

    } else {
        assign->type = getchild(assign, 0)->type;
    }

}

void check_Call(struct node *call, struct symbol_list *symbol_list) {
    struct symbol_list *definition = search_symbol(symbol_list, getchild(call, 0)->token, -1, true);
    struct node_list *call_param = call->children->next;
    while ((call_param = call_param->next) != NULL) {
        check_expression(call_param->node, symbol_list);
    }

    bool match = true;
    if (definition!=NULL) {
        // check function call

        // check if parameters are right in number and type
        // we will be comparing the call_param node's type with the entries on the defenition's symbol table
        call_param = call->children->next->next;
        struct symbol_list *table_entry = definition->child_scope->next;
        for(table_entry; table_entry!=NULL; table_entry=table_entry->next) {
            if (!table_entry->is_param) {
                if (call_param==NULL) {
                    //printf("too few arguments for %s\n", definition->identifier);
                }
                if (table_entry->type!=call_param->node->type) {
                    match = false;
                    //printf("wrong type of argument for %s (%s)\n", definition->identifier, type_names2[call_param->node->type]);
                    
                }
                call_param = call_param->next;
            }
        }
        if (call_param!=NULL) {
            // too many argumants!
            //printf("too many arguments for %s!\n", definition->identifier);
            match = false;
        }
        if (match) {
            getchild(call, 0)->type = definition->type;
            return;
        }
    }
    printf("Line %d, column %d: Cannot find symbol %s", getchild(call, 0)->token_line, getchild(call, 0)->token_column, getchild(call, 0)->token);
    call_param = call->children->next;
    printf("(");
    if ((call_param=call_param->next)!=NULL) {
        printf("%s", type_names2[call_param->node->type]);
        //call_param->node->type = category_to_type2[call_param->node->category];
        //check_expression(call_param->node, symbol_list);
        while ((call_param=call_param->next)!=NULL) {
            printf(",%s", type_names2[call_param->node->type]);
            //call_param->node->type = category_to_type2[call_param->node->category];
            //check_expression(call_param->node, symbol_list);
        }
    }
    printf(")\n");
    call->type = undef_type;
    getchild(call, 0)->type = undef_type;
}

void check_Statement(struct node *statement, struct symbol_list *symbol_list) {
    if (statement->category == Call) {
        check_Call(statement, symbol_list);

    } else if (statement->category == Assign) {
        check_Assign(statement, symbol_list);

    } else if (statement->category == Return) {
        check_Return(statement, symbol_list);

    } else if (statement->category == Print) {
        check_Print(statement, symbol_list);

    } else if (statement->category == If) {
        check_If(statement, symbol_list);

    }
}

void check_VarDecl(struct node *vardecl, struct symbol_list *symbol_func) {
    if (search_symbol(symbol_func, getchild(vardecl, 1)->token, 1, false)!=NULL) {
        printf("Line %d, column %d: Symbol %s already defined\n", getchild(vardecl, 1)->token_line, getchild(vardecl, 1)->token_column, getchild(vardecl, 1)->token);
    } else {
        insert_symbol(symbol_func, getchild(vardecl, 1)->token, category_to_type2[getchild(vardecl, 0)->category], vardecl);
    }
}

void check_FuncParams(struct node *params, struct symbol_list *symbol_func) {
    struct node_list *child = params->children;
    struct symbol_list *new_entry;

    bool already_exists = false;

    while ((child = child->next) != NULL) {
        // find if symbol already exists
        already_exists = search_symbol(symbol_func, getchild(child->node, 1)->token, 1, false);
        // dont care if it already exists, add it anyway, but mark it as "is_invalid" to not display it
        new_entry = insert_symbol(symbol_func, getchild(child->node, 1)->token, category_to_type2[getchild(child->node, 0)->category], child->node);
        if (new_entry!=NULL) {
            new_entry->is_param = true;
            if (already_exists) {
                printf("Line %d, column %d: Symbol %s already defined\n", getchild(child->node, 1)->token_line, getchild(child->node, 1)->token_column, getchild(child->node, 1)->token);
                new_entry->is_invalid = true;
            }
        }
    }
}

void check_FuncBody(struct node *body, struct symbol_list *symbol_func) {
    /* Body only has VarDecl or Statements */
    struct node_list *child = body->children;
    while ((child=child->next) != NULL) {
        if (child->node->category == VarDecl) {
            // var decl
            check_VarDecl(child->node, symbol_func);

        } else {
            // statement
            check_Statement(child->node, symbol_func);

        }
    }
}

void check_FuncDecl(struct node *declaration, struct symbol_list *symbol_global_scope) {
    if (declaration==NULL) {
        return;
    }
    // IMPORTANTE!
    // FuncDecl can have 2 or 3 children:
    //      if it has 3, they are: id, return type, funcparam)
    //      if it has 2, they are: id, funcparams

    // get the info needed
    struct node *funcheader_node, *funcbody_node;
    struct node *identifier_node, *return_node, *paramdecl_node;
    funcheader_node = getchild(declaration, 0);
    funcbody_node = getchild(declaration, 1);
    identifier_node = getchild(funcheader_node, 0);
    return_node = getchild(funcheader_node, 1);
    // if there is no return node, set it to paramdecl, and return_node=None
    if (return_node->category==FuncParams) {
        paramdecl_node = return_node;
        struct node dummy_return_node;
        return_node = &dummy_return_node;
        return_node->category = None;
    } else {
        paramdecl_node = getchild(funcheader_node, 2);
    }

    // check header
    // check if function symbol already exists
    struct symbol_list *global_entry;
    if (search_symbol(symbol_global_scope, identifier_node->token, 1, true)) {
        printf("Line %d, column %d: Symbol %s already defined\n", identifier_node->token_line, identifier_node->token_column, identifier_node->token);
        return;
    } else {
        // add function symbol to global table
        global_entry = insert_symbol(symbol_global_scope, identifier_node->token, category_to_type2[return_node->category], declaration);
        global_entry->is_function = true;
    }

    // create scope
    struct symbol_list *symbol_scope = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    symbol_scope->identifier = strdup(global_entry->identifier);
    symbol_scope->parent_scope = symbol_global_scope;
    symbol_scope->next = NULL;

    global_entry->child_scope = symbol_scope;

    // add return symbol
    insert_symbol(symbol_scope, "return", global_entry->type, NULL);    // node is added when we find Return node, no need to look for it now

    // check the params
    check_FuncParams(paramdecl_node, symbol_scope);


    // check body
    check_FuncBody(funcbody_node, symbol_scope);

    
}

//void check_VarDecl(struct node *declaration) {
//    if (declaration==NULL) {
//        return;
//    }
//
//    // get the info needed
//    struct node *type_node, *identifier_node;
//    type_node = getchild(declaration, 0);
//    identifier_node = getchild(declaration, 1);
//
//    // check if var symbol already exists
//    if (search_symbol(symbol_table, identifier_node->token)) {
//        printf("Line %d, column %d: Symbol %s already defined\n", identifier_node->token_line, identifier_node->token_column, identifier_node->token);
//        return;
//    } else {
//        // add global variable symbol to global table
//        insert_symbol(symbol_table, identifier_node->token, category_to_type2[type_node->category], declaration);
//    }
//
//}






// semantic analysis begins here, with the AST root node
int check_program(struct node *program) {
    symbol_table = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    symbol_table->next = NULL;
    //insert_symbol(symbol_table, "write", integer_type, newnode(Function, NULL));/* predeclared functions (no children) */
    //insert_symbol(symbol_table, "read", integer_type, newnode(Function, NULL));
    struct node_list *child = program->children;

    //printf("%s\n", category_names2[child->next->node->category]);
    //printf("%p, %p\n", child->next->node->children, NULL);
    //printf("%s\n", child->next->node->children->next->node->category);


    while((child = child->next) != NULL) {
        if (child->node->category == FuncDecl) {
            check_FuncDecl(child->node, symbol_table);
        } else if (child->node->category == VarDecl) {
            check_VarDecl(child->node, symbol_table);
        }
        //printf("category: %s\n", category_names2[child->node->category]);
    }
    return semantic_errors;
}


// insert a new symbol in the list, unless it is already there
struct symbol_list *insert_symbol(struct symbol_list *table, char *identifier, enum type type, struct node *node) {
    struct symbol_list *new = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    new->identifier = strdup(identifier);
    new->type = type;
    new->node = node;
    new->is_function = false;
    new->is_invalid = false;
    new->is_param = false;
    new->child_scope = NULL;
    new->parent_scope = NULL;
    new->next = NULL;
    struct symbol_list *symbol = table;
    while(symbol != NULL) {
        if(symbol->next == NULL) {
            symbol->next = new;    /* insert new symbol at the tail of the list */
            break;
        } else if(strcmp(symbol->next->identifier, identifier) == 0) {
            // if i want to double insert, i do
            #ifdef VERBOSE
                printf("[VERBOSE] CUIDADO, DOUBLE INSERT NA TABELA \"%s\" DE NODE \"%s\"\n", table->identifier, symbol->identifier);
            #endif
            //free(new);
            //return NULL;           /* return NULL if symbol is already inserted */
        }
        symbol = symbol->next;
    }
    return new;
}

// look up a symbol by its identifier (not only in current table, but also all parent tables)
struct symbol_list *search_symbol(struct symbol_list *table, char *identifier, int depth, bool is_function) {
    if (table==NULL) {
        return NULL;
    }
    if (depth==0) {
        return NULL;
    }
    struct symbol_list *symbol;
    for(symbol = table->next; symbol != NULL; symbol = symbol->next) {
        if(strcmp(symbol->identifier, identifier) == 0) {
            if (is_function==symbol->is_function) {
                return symbol;
            }
        }
    }
    return search_symbol(table->parent_scope, identifier, depth-1, is_function);
}


void show_symbol_table() {
    // global scope
    struct symbol_list *symbol;
    printf("===== Global Symbol Table =====\n");
    for(symbol = symbol_table->next; symbol != NULL; symbol = symbol->next) {
        // “Name\t[ParamTypes]\tType[\tparam]”
        char *paramtypes = show_functionparameters(symbol);
        printf("%s\t%s\t%s\n", symbol->identifier, paramtypes, type_names2[symbol->type]);
        free(paramtypes);
    }
    printf("\n");

    // now for every scope
    struct symbol_list *scope;
    for(symbol = symbol_table->next; symbol != NULL; symbol = symbol->next) {
        if (symbol->node->category != FuncDecl) {
            continue;
        }

        scope = symbol->child_scope;
        char *paramtypes = show_functionparameters(symbol);
        printf("===== Function %s%s Symbol Table =====\n", scope->identifier, paramtypes);
        free(paramtypes);

        struct symbol_list *symbol2;
        for(symbol2 = scope->next; symbol2 != NULL; symbol2 = symbol2->next) {
            if (symbol2->is_invalid) {
                continue;
            }
            printf("%s\t\t%s", symbol2->identifier, type_names2[symbol2->type]);
            if (symbol2->is_param) {
                printf("\tparam\n");
            } else {
                printf("\n");
            }
        }

        printf("\n");
    }
}

char *show_functionparameters(struct symbol_list *symbol) {
    char *result = (char *)malloc(1024);
    memset(result, '\0', 1024);
    if (symbol->node->category == VarDecl) {
        // When is Var Decl
        // must be empty
    }
    if (symbol->node->category == FuncDecl) {
        // When is Function
        sprintf(result+strlen(result), "(");

        struct symbol_list *scope = symbol->child_scope;
        for(struct symbol_list *symbol2 = scope->next; symbol2 != NULL; symbol2 = symbol2->next) {
            if (symbol2->is_param) {
                sprintf(result+strlen(result), "%s,", type_names2[symbol2->type]);
            }
        }

        if (result[strlen(result)-1]==',') {
            // remove extra comma
            result[strlen(result)-1] = '\0';
        }

        sprintf(result+strlen(result), ")");
        
    }
    return result;
}
