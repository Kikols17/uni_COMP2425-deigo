#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantics.h"

int semantic_errors = 0;

struct symbol_list *symbol_table;

const char *category_names2[] = CATEGORY_NAMES;
const char *type_names2[] = TYPE_NAMES;
const char category_to_type2[] = CATEGORY_TO_TYPE;


void check_expression(struct node *expression, struct symbol_list *scope) {
    //switch(expression->category) {
    //    case Identifier:
    //        if(search_symbol(scope, expression->token) == NULL) {
    //            printf("Variable %s (%d:%d) undeclared\n", expression->token, expression->token_line, expression->token_column);
    //            semantic_errors++;
    //        } else {
    //            expression->type = search_symbol(scope, expression->token)->type;
    //        }
    //        break;
    //    case Natural:
    //        expression->type = int_type;
    //        break;
    //    case Decimal:
    //        expression->type = float32_type;
    //        break;
    //    case Call:
    //        if(search_symbol(symbol_table, getchild(expression, 0)->token) == NULL || search_symbol(symbol_table, getchild(expression, 0)->token)->node->category != Function) {
    //            printf("Function %s (%d:%d) undeclared\n", getchild(expression, 0)->token, getchild(expression, 0)->token_line, getchild(expression, 0)->token_column);
    //            semantic_errors++;
    //        } else {
    //            struct node *arguments = getchild(expression, 1);
    //            struct node *parameters = getchild(search_symbol(symbol_table, getchild(expression, 0)->token)->node, 1);
    //            if(parameters != NULL && countchildren(arguments) != countchildren(parameters)) {
    //                printf("Calling %s (%d:%d) with incorrect arguments\n", getchild(expression, 0)->token, getchild(expression, 0)->token_line, getchild(expression, 0)->token_column);
    //                semantic_errors++;
    //            } else {
    //                struct node_list *argument = arguments->children;
    //                while((argument = argument->next) != NULL)
    //                    check_expression(argument->node, scope);
    //            }
    //        }
    //        break;
    //    case If:
    //        check_expression(getchild(expression, 0), scope);
    //        check_expression(getchild(expression, 1), scope);
    //        check_expression(getchild(expression, 2), scope);
    //        break;
    //    case Add:
    //    case Sub:
    //    case Mul:
    //    case Div:
    //        check_expression(getchild(expression, 0), scope);
    //        check_expression(getchild(expression, 1), scope);
    //        break;
    //    default:
    //        break;
    //}
}

void check_parameters(struct node *parameters, struct symbol_list *scope) {
    //struct node_list *parameter = parameters->children;
    //while((parameter = parameter->next) != NULL) {
    //    struct node *id = getchild(parameter->node, 1);
    //    enum type type = category_type(getchild(parameter->node, 0)->category);
    //    if(search_symbol(symbol_table, id->token) == NULL) {
    //        insert_symbol(symbol_table, id->token, type, parameter->node);
    //        insert_symbol(scope, id->token, type, parameter->node);
    //    } else {
    //        printf("Identifier %s (%d:%d) already declared\n", id->token, id->token_line, id->token_column);
    //        semantic_errors++;
    //    }
    //}
}

void check_function(struct node *function) {
    //struct node *id = getchild(function, 0);
    //if(search_symbol(symbol_table, id->token) == NULL) {
    //    insert_symbol(symbol_table, id->token, none_type, function);
    //} else {
    //    printf("Identifier %s (%d:%d) already declared\n", id->token, id->token_line, id->token_column);
    //    semantic_errors++;
    //}
    //struct symbol_list *scope = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    //scope->next = NULL;
    //check_parameters(getchild(function, 1), scope);
    //check_expression(getchild(function, 2), scope);
    /* ToDo: scope should be free'd */
}

void check_FuncDecl(struct node *declaration) {
    if (declaration==NULL) {
        return;
    }
    // IMPORTANTE!
    // FuncDecl can have 2 or 3 children:
    //      if it has 3, they are: id, return type, funcparam)
    //      if it has 2, they are: id, funcparams

    // get the info needed
    struct node *funcheader_node;
    struct node *identifier_node, *return_node, *paramdecl_node;
    funcheader_node = getchild(declaration, 0);
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
    if (search_symbol(symbol_table, identifier_node->token)) {
        printf("Line %d, column %d: Symbol %s already defined\n", identifier_node->token_line, identifier_node->token_column, identifier_node->token);
        return;
    } else {
        insert_symbol(symbol_table, identifier_node->token, category_to_type2[return_node->category], declaration);
    }

    
}

void check_VarDecl(struct node *declaration) {
    if (declaration==NULL) {
        return;
    }

    // get the info needed
    struct node *type_node, *identifier_node;
    type_node = getchild(declaration, 0);
    identifier_node = getchild(declaration, 1);

    // check if var symbol already exists
    if (search_symbol(symbol_table, identifier_node->token)) {
        printf("Line %d, column %d: Symbol %s already defined\n", identifier_node->token_line, identifier_node->token_column, identifier_node->token);
        return;
    } else {
        insert_symbol(symbol_table, identifier_node->token, category_to_type2[type_node->category], declaration);
    }

}






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
            check_FuncDecl(child->node);
        } else if (child->node->category == VarDecl) {
            check_VarDecl(child->node);
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
    new->next = NULL;
    struct symbol_list *symbol = table;
    while(symbol != NULL) {
        if(symbol->next == NULL) {
            symbol->next = new;    /* insert new symbol at the tail of the list */
            break;
        } else if(strcmp(symbol->next->identifier, identifier) == 0) {
            free(new);
            return NULL;           /* return NULL if symbol is already inserted */
        }
        symbol = symbol->next;
    }
    return new;
}

// look up a symbol by its identifier
struct symbol_list *search_symbol(struct symbol_list *table, char *identifier) {
    struct symbol_list *symbol;
    for(symbol = table->next; symbol != NULL; symbol = symbol->next) {
        if(strcmp(symbol->identifier, identifier) == 0) {
            return symbol;
        }
    }
    return NULL;
}

void show_symbol_table() {
    struct symbol_list *symbol;
    printf("===== Global Symbol Table =====\n");
    for(symbol = symbol_table->next; symbol != NULL; symbol = symbol->next) {
        // “Name\t[ParamTypes]\tType[\tparam]”
        char *paramtypes = show_functionparameters(symbol);
        printf("%s\t%s\t%s\n", symbol->identifier, paramtypes, type_names2[symbol->type]);
        free(paramtypes);
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
        struct node *header = getchild(symbol->node, 0);
        //printf("%s\n", category_names2[header->category]);
        // cant be sure if the FuncParams is the third or second child, so do this:
        struct node *params = getchild(header, 1);
        //printf("%s\n", category_names2[params->category]);
        if (params->category != FuncParams) {
            params = getchild(header, 2);
            //printf("%s\n", category_names2[params->category]);
        }
        //printf("%s\n", category_names2[params->category]);

        struct node_list *child = params->children;
        while ((child = child->next) != NULL) {
            struct node *cur_param_type = getchild(child->node, 0);
            sprintf(result+strlen(result), "%s,", type_names2[category_to_type2[cur_param_type->category]]);
        }

        if (result[strlen(result)-1]==',') {
            // remove extra comma
            result[strlen(result)-1] = '\0';
        }

        sprintf(result+strlen(result), ")");
        
    }
    return result;
}
