#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantics.h"

int semantic_errors = 0;

struct symbol_list *symbol_table;

// Stack with FuncBody's to be checked
int                 FuncBodyStack_count = 0;
struct symbol_list *FuncBodyStack_symbol_scope[2048];
struct node        *FuncBodyStack_node[2048];

const char *category_names2[] = CATEGORY_NAMES;
const char *type_names2[] = TYPE_NAMES;
const enum type category_to_type2[] = CATEGORY_TO_TYPE;


void check_expression(struct node *expression, struct symbol_list *symbol_scope) {
    char string_op[] = "\0\0";  // minimum characters
    struct node *left_expr;
    struct node *right_expr;

    switch(expression->category) {
        case Identifier:
            // check if has been declared
            ;   // ?? WHY
            struct symbol_list *declaration = search_symbol(symbol_scope, expression->token, -1, false);
            if (declaration==NULL) {
                printf("Line %d, column %d: Cannot find symbol %s\n", expression->token_line, expression->token_column, expression->token);
                semantic_errors++;
                expression->type = undef_type;
            } else {
                expression->type = declaration->type;
                declaration->is_used = true;
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
            if (string_op[0]=='\0') { string_op[0]='+'; }
        case Sub:
            if (string_op[0]=='\0') { string_op[0]='-'; }
        case Mul:
            if (string_op[0]=='\0') { string_op[0]='*'; }
        case Div:
            if (string_op[0]=='\0') { string_op[0]='/'; }
        case Mod:
            if (string_op[0]=='\0') { string_op[0]='%'; }

            left_expr = getchild(expression, 0);
            right_expr = getchild(expression, 1);
            bool legal = false;

            check_expression(left_expr, symbol_scope);
            check_expression(right_expr, symbol_scope);

            if (left_expr->type==right_expr->type) {
                if (left_expr->type==int_type || left_expr->type==float32_type) {
                    legal = true;
                } else {
                    legal = false;
                }
            } else {
                legal = false;
            }

            if (!legal) {
                printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n", expression->token_line, expression->token_column, string_op, type_names2[left_expr->type], type_names2[right_expr->type]);
                semantic_errors++;
                expression->type = undef_type;
            } else {
                expression->type = left_expr->type;
            }
            break;
        


        case Minus:
            if (string_op[0]=='\0') { string_op[0]='-'; }
        case Plus:
            if (string_op[0]=='\0') { string_op[0]='+'; }
            check_expression(getchild(expression, 0), symbol_scope);
            if (getchild(expression, 0)->type == int_type || getchild(expression, 0)->type == float32_type) {
                legal = true;
            } else {
                legal = false;
                expression->type = undef_type;
            }
        case Not:
            if (string_op[0]=='\0') { string_op[0]='!';
                check_expression(getchild(expression, 0), symbol_scope);
                if (getchild(expression, 0)->type == bool_type) {
                    legal = true;
                } else {
                    legal = false;
                    expression->type = bool_type;
                }
            }
            if (!legal) {
                printf("Line %d, column %d: Operator %s cannot be applied to type %s\n", expression->token_line, expression->token_column, string_op, type_names2[getchild(expression, 0)->type]);
                semantic_errors++;
            } else {
                expression->type = getchild(expression, 0)->type;
            }
            break;



        case Or:
            if (string_op[0]=='\0') { string_op[0]='|'; string_op[1]='|'; }
        case And:
            if (string_op[0]=='\0') { string_op[0]='&'; string_op[1]='&'; }

            left_expr = getchild(expression, 0);
            right_expr = getchild(expression, 1);

            check_expression(left_expr, symbol_scope);
            check_expression(right_expr, symbol_scope);
            if (left_expr->type!=bool_type || right_expr->type!=bool_type) {
                printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n", expression->token_line, expression->token_column, string_op, type_names2[left_expr->type], type_names2[right_expr->type]);
                semantic_errors++;
            }
            expression->type = bool_type;
            break;



        case Eq:
            if (string_op[0]=='\0') { string_op[0]='='; string_op[1]='='; }
        case Ne:
            if (string_op[0]=='\0') { string_op[0]='!'; string_op[1]='='; }

            left_expr = getchild(expression, 0);
            right_expr = getchild(expression, 1);

            check_expression(left_expr, symbol_scope);
            check_expression(right_expr, symbol_scope);

            if (left_expr->type!=right_expr->type || (left_expr->type==undef_type) || (left_expr->type==none_type)) {
                printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n", expression->token_line, expression->token_column, string_op, type_names2[left_expr->type], type_names2[right_expr->type]);
                semantic_errors++;
            }
            expression->type = bool_type;
            break;



        case Lt:
            if (string_op[0]=='\0') { string_op[0]='<'; }
        case Le:
            if (string_op[0]=='\0') { string_op[0]='<'; string_op[1]='='; }
        case Gt:
            if (string_op[0]=='\0') { string_op[0]='>'; }
        case Ge:
            if (string_op[0]=='\0') { string_op[0]='>'; string_op[1]='='; }


            left_expr = getchild(expression, 0);
            right_expr = getchild(expression, 1);

            check_expression(left_expr, symbol_scope);
            check_expression(right_expr, symbol_scope);

            if (left_expr->type!=right_expr->type || !(left_expr->type==int_type || left_expr->type==float32_type)) {
                printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n", expression->token_line, expression->token_column, string_op, type_names2[left_expr->type], type_names2[right_expr->type]);
                semantic_errors++;
            }
            expression->type = bool_type;
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

void check_Block(struct node *block_node, struct symbol_list *symbol_scope) {
    struct node_list *child = block_node->children;
    while ((child=child->next)!=NULL) {
        check_expression(child->node, symbol_scope);
    }
}

void check_For(struct node *for_node, struct symbol_list *symbol_scope) {
    /* getchild(if_node, 0)  ----->  condition [case1] (check_expression) must be boolean OR for body (check_block) [case2]
     * getchild(if_node, 1)  ----->  for body [case1] (check_block) if getchild(if_node, 0) is not already body [case2]
     */

    if (getchild(for_node, 1)!=NULL) {
        /* [case1]
         * getchild(if_node, 0)  ----->  condition (check_expression) must be boolean
         * getchild(if_node, 1)  ----->  for body (check_block)
         */
        check_expression(getchild(for_node, 0), symbol_scope);
        check_Block(getchild(for_node, 1), symbol_scope);
        if ((getchild(for_node, 0)->type!=bool_type || getchild(for_node, 0)->type==none_type) || getchild(for_node, 0)->type==undef_type) {
            printf("Line %d, column %d: Incompatible type %s in for statement\n", getchild(for_node, 0)->token_line, getchild(for_node, 0)->token_column, type_names2[getchild(for_node, 0)->type]);
            semantic_errors++;
        }

    } else {
        /* [case2]
         * getchild(if_node, 0)  ----->  for body (check_block)
         * getchild(if_node, 1)  ----->  NULL
         */
        check_Block(getchild(for_node, 0), symbol_scope);

    }

}

void check_If(struct node *if_node, struct symbol_list *symbol_scope) {
    /* getchild(if_node, 0)   -----> condition (check_expression) must be boolean
     * getchild(if_node, 1)   -----> if body (check_block)
     * getchild(if_node, 2)   -----> else body (check_block)
     *
     * Como dgo n tem scope dentro de scopes afinal n é preciso criar aqui novo scope,
     * só perdi tempo a planear para isto :(
     **/

    check_expression(getchild(if_node, 0), symbol_scope);
    if (getchild(if_node, 0)->type != bool_type) {
        printf("Line %d, column %d: Incompatible type %s in if statement\n", getchild(if_node, 0)->token_line, getchild(if_node, 0)->token_column, type_names2[getchild(if_node, 0)->type]);
        semantic_errors++;
    }


    if (getchild(if_node, 1)!=NULL) {
        check_Block(getchild(if_node, 1), symbol_scope);
    }

    // Not always there is else block, (meta2 "no unnecessary blocks")
    if (getchild(if_node, 2)!=NULL) {
        check_Block(getchild(if_node, 2), symbol_scope);
    }
}

void check_ParseArgs(struct node *parse_args, struct symbol_list *symbol_scope) {
    check_expression(getchild(parse_args, 0), symbol_scope);
    check_expression(getchild(parse_args, 1), symbol_scope);
    parse_args->type = getchild(parse_args, 0)->type;

    if ((getchild(parse_args, 0)->type!=int_type && getchild(parse_args, 0)->type!=string_type) || getchild(parse_args, 1)->type!=int_type) {
        printf("Line %d, column %d: Operator strconv.Atoi cannot be applied to types %s, %s\n", parse_args->token_line, parse_args->token_column, type_names2[getchild(parse_args, 0)->type], type_names2[getchild(parse_args, 1)->type]);
        semantic_errors++;
    }
}

void check_Print(struct node *print_node, struct symbol_list *symbol_scope) {
    //struct symbol_list *definition;
    //if (getchild(print_node, 0)->category == Identifier) {
    //    // if is identifier, make sure it has been defined
    //    definition = search_symbol(symbol_scope, getchild(print_node, 0)->token, -1, false);
    //    if (definition==NULL) {
    //        // was not defined
    //        getchild(print_node, 0)->type = undef_type;
    //        printf("Line %d, column %d: Cannot find symbol %s\n", getchild(print_node, 0)->token_line, getchild(print_node, 0)->token_column, getchild(print_node, 0)->token);
    //        semantic_errors++;
    //        return;
    //    }
    //    getchild(print_node, 0)->type = definition->type;
    //} else {
    //    //getchild(print_node, 0)->type = category_to_type2[getchild(print_node, 0)->category];
    //    check_expression(getchild(print_node, 0), symbol_scope);
    //}
    check_expression(getchild(print_node, 0), symbol_scope);
    if (getchild(print_node, 0)->type == undef_type) {
        printf("Line %d, column %d: Incompatible type %s in fmt.Println statement\n", print_node->token_line, print_node->token_column, type_names2[getchild(print_node, 0)->type]);
        semantic_errors++;
    }
}

void check_Return(struct node *return_node, struct symbol_list *symbol_scope) {
    struct symbol_list *symbol_return = search_symbol(symbol_scope, "return", 1, false);
    symbol_return->node = return_node;
    // assume there is always a return symbol (already goofed up if not)

    if (getchild(return_node, 0)==NULL) {
        // no params
        if (symbol_return->type != none_type) {
            printf("Line %d, column %d: Incompatible type void in return statement\n", return_node->token_line, return_node->token_column);
            semantic_errors++;
        }
    } else {
        check_expression(getchild(return_node, 0), symbol_scope);
        if (symbol_return->type != getchild(return_node, 0)->type) {
            printf("Line %d, column %d: Incompatible type %s in return statement\n", getchild(return_node, 0)->token_line, getchild(return_node, 0)->token_column, type_names2[getchild(return_node, 0)->type]);
            semantic_errors++;
        }
    }
}

void check_Assign(struct node *assign, struct symbol_list *symbol_scope) {
    // check variable
    //struct symbol_list *definition = search_symbol(symbol_scope, getchild(assign, 0)->token, -1, false);
    //if (definition!=NULL) {
    //    getchild(assign, 0)->type = definition->type;
    //    definition->is_used = true;
    //} else {
    //    getchild(assign, 0)->type = undef_type;
    //    printf("Line %d, column %d: Cannot find symbol %s\n", getchild(assign, 0)->token_line, getchild(assign, 0)->token_column, getchild(assign, 0)->token);
    //    semantic_errors++;
    //}

    // check variable
    check_expression(getchild(assign, 0), symbol_scope);

    // check statement
    check_expression(getchild(assign, 1), symbol_scope);
    
    // check if assign is legal
    if (getchild(assign, 0)->type == undef_type || getchild(assign, 0)->type!=getchild(assign, 1)->type) {
        char typestring0[32];
        char typestring1[32];
        if (getchild(assign, 0)->type==none_type) {
            sprintf(typestring0, "void");
        } else {
            sprintf(typestring0, type_names2[getchild(assign, 0)->type]);
        }
        if (getchild(assign, 1)->type==none_type) {
            sprintf(typestring1, "void");
        } else {
            sprintf(typestring1, type_names2[getchild(assign, 1)->type]);
        }
        printf("Line %d, column %d: Operator = cannot be applied to types %s, %s\n", assign->token_line, assign->token_column, typestring0, typestring1);
        semantic_errors++;
    }
    assign->type = getchild(assign, 0)->type;

}

void check_Call(struct node *call, struct symbol_list *symbol_list) {
    struct symbol_list *definition = search_symbol(symbol_table, getchild(call, 0)->token, -1, true);
    struct node_list *call_param = call->children->next;
    while ((call_param = call_param->next) != NULL) {
        check_expression(call_param->node, symbol_list);
    }

    bool match = true;
    if (definition!=NULL) {
        #ifdef VERBOSE
        printf("[VERBOSE] Função %s pelo menos existe\n", getchild(call, 0)->token);
        #endif
        // check function call

        // check if parameters are right in number and type
        // we will be comparing the call_param node's type with the entries on the defenition's symbol table
        call_param = call->children->next->next;
        struct symbol_list *table_entry = definition->child_scope->next;
        enum type definition_return_type;
        for(; table_entry!=NULL; table_entry=table_entry->next) {
            if (strcmp(table_entry->identifier, "return")==0) {
                definition_return_type = table_entry->type;
                //printf("type: %s\n", type_names2[definition_return_type]);
            }
            if (table_entry->is_param) {
                if (call_param==NULL) {
                    #ifdef VERBOSE
                    printf("[VERBOSE] too few arguments for %s\n", definition->identifier);
                    #endif
                    match = false;
                    break;
                }
                if (table_entry->type!=call_param->node->type) {
                    #ifdef VERBOSE
                    printf("[VERBOSE] wrong type of argument for %s (%s)\n", definition->identifier, type_names2[call_param->node->type]);
                    #endif
                    match = false;
                    
                }
                call_param = call_param->next;
            }
        }
        if (call_param!=NULL) {
            // too many argumants!
            #ifdef VERBOSE
            printf("[VERBOSE] too many arguments for %s!\n", definition->identifier);
            #endif
            match = false;
        }
        if (match) {
            if (definition_return_type!=none_type) {
                call->type = definition_return_type;
            }
            getchild(call, 0)->type = definition_return_type;
            getchild(call, 0)->is_callid = true;
            return;
        }
        #ifdef VERBOSE
        else {
            printf("[VERBOSE] unmatched params for %s!\n", getchild(call, 0)->token);
        }
        #endif
    }
    #ifdef VERBOSE
    else {
        printf("[VERBOSE] Função %s straight up nao existe\n", getchild(call, 0)->token);
    }
    #endif
    printf("Line %d, column %d: Cannot find symbol %s", getchild(call, 0)->token_line, getchild(call, 0)->token_column, getchild(call, 0)->token);
    semantic_errors++;
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

void check_Statement(struct node *statement, struct symbol_list *symbol_scope) {
    if (statement->category == Call) {
        check_Call(statement, symbol_scope);

    } else if (statement->category == Assign) {
        check_Assign(statement, symbol_scope);

    } else if (statement->category == Return) {
        check_Return(statement, symbol_scope);

    } else if (statement->category == Print) {
        check_Print(statement, symbol_scope);

    } else if (statement->category == If) {
        check_If(statement, symbol_scope);

    } else if (statement->category == For) {
        check_For(statement, symbol_scope);

    } else if (statement->category == ParseArgs) {
        check_ParseArgs(statement, symbol_scope);
    }
}

void check_VarDecl(struct node *vardecl, struct symbol_list *symbol_func) {
    if (search_symbol(symbol_func, getchild(vardecl, 1)->token, 1, false)!=NULL) {
        printf("Line %d, column %d: Symbol %s already defined\n", getchild(vardecl, 1)->token_line, getchild(vardecl, 1)->token_column, getchild(vardecl, 1)->token);
        semantic_errors++;
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
                semantic_errors++;
                new_entry->is_invalid = true;
            }
        }
    }
}

void check_FuncBody(struct node *body, struct symbol_list *symbol_scope) {
    /* Body only has VarDecl or Statements */
    struct node_list *child = body->children;
    while ((child=child->next) != NULL) {
        if (child->node->category == VarDecl) {
            // var decl
            check_VarDecl(child->node, symbol_scope);

        } else {
            // statement
            check_Statement(child->node, symbol_scope);

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
    if (search_symbol(symbol_global_scope, identifier_node->token, 1, true)!=NULL  ||  search_symbol(symbol_global_scope, identifier_node->token, 1, false)!=NULL) {
        printf("Line %d, column %d: Symbol %s already defined\n", identifier_node->token_line, identifier_node->token_column, identifier_node->token);
        semantic_errors++;
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

    // CHECK THE BODY LATER
    // if not, other symbols like functions may not be defined but should be accessed
    FuncBodyStack_node[FuncBodyStack_count] = funcbody_node;
    FuncBodyStack_symbol_scope[FuncBodyStack_count] = symbol_scope;
    FuncBodyStack_count++;
    
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
    // now check all FuncBody's
    for (int i=0; i<FuncBodyStack_count; i++) {
        check_FuncBody(FuncBodyStack_node[i], FuncBodyStack_symbol_scope[i]);
        check_UnusedSymbols(FuncBodyStack_symbol_scope[i]);
    }
    //check_UnusedSymbols(symbol_table);
    
    return semantic_errors;
}

void check_UnusedSymbols(struct symbol_list *symbol_scope) {
    if (symbol_scope==NULL) {
        // empty scope
        return;
    }

    struct symbol_list *symbol;
    for(symbol = symbol_scope->next; symbol != NULL; symbol = symbol->next) {
        if (symbol->is_used==false && symbol->is_param==false) {
            if (symbol->node==NULL || getchild(symbol->node, 1)==NULL || symbol->node->category!=VarDecl || symbol->is_invalid) {
                continue;
            }
            //printf("Category: %s ", category_names2[symbol->node->category]);
            printf("Line %d, column %d: Symbol %s declared but never used\n", getchild(symbol->node, 1)->token_line, getchild(symbol->node, 1)->token_column, getchild(symbol->node, 1)->token);
            semantic_errors++;
        }
        //check_UnusedSymbols(symbol->child_scope);
    }
    

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
    new->is_used = false;
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
        #ifdef VERBOSE
        printf("[VERBOSE] Calhou identifier=%s, depth=%d, is_function=%d\n", identifier, depth, is_function);
        #endif
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
            #ifdef VERBOSE
            else {
                //printf("[VERBOSE] Symbol %s found but not match is_function (%d!=%d)\n", symbol->identifier, symbol->is_function, is_function);
            }
            #endif
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
