#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantics.h"
#include "codegen.h"

/*
 * Trabalho realizado por:
 *      - Francisco Amado Lapa Marques Silva - uc2022213583
 *      - Miguel Moital Rodrigues Cabral Martins - uc2022213951
 */

extern struct symbol_list *symbol_table;

const char *type_to_llvm3[] = TYPE_TO_LLVM;
const char *empty_type_llvm[] = EMPTY_TYPE_LLVM;
const char *category_to_llvm3[] = CATEGORY_TO_LLVM;
const enum type category_to_type3[] = CATEGORY_TO_TYPE;
char pointer_char[2];
char strlit_buff[4096];

int temporary;   // sequence of temporary registers in a function
int if_count = 0;
int for_count = 0;

struct symbol_list *cur_scope = NULL;


void codegen_indent(int ind) {
    for (int i=0; i<ind; i++) {
        printf("    ");
    }
}





void codegen_globalvar(struct node *vardecl, int ind) {
    struct node *type_node = getchild(vardecl, 0);
    struct node *id_node = getchild(vardecl, 1);
    sprintf(id_node->llvm_name, "@%s", id_node->token);
    codegen_indent(ind);
    printf("%s = global %s %s\n", id_node->llvm_name, type_to_llvm3[category_to_type3[type_node->category]], empty_type_llvm[category_to_type3[type_node->category]]);
}





void codegen_expression(struct node *expression, int ind) {
    struct node *left_expression;
    struct node *right_expression;
    struct symbol_list *definition;

    char op_prefix = '\n';

    switch(expression->category) {
        case Identifier:
            // find out the identifier's declaration node (we assume it exists)
            definition = search_symbol(cur_scope, expression->token, -1, false);

            sprintf(expression->llvm_name, "%%%d", temporary++);
            codegen_indent(ind);
            printf("%s = load %s, %s* %s\n", expression->llvm_name, type_to_llvm3[category_to_type3[getchild(definition->node, 0)->category]], type_to_llvm3[category_to_type3[getchild(definition->node, 0)->category]], getchild(definition->node, 1)->llvm_name);

            //if (getchild(definition->node, 1)->llvm_name[0] == '@') {
            //    sprintf(expression->llvm_name, "%%%d", temporary++);
            //    codegen_indent(ind);
            //    printf("%s = load %s, %s* %s\n", expression->llvm_name, type_to_llvm3[category_to_type3[getchild(definition->node, 0)->category]], type_to_llvm3[category_to_type3[getchild(definition->node, 0)->category]], getchild(definition->node, 1)->llvm_name);
            //} else {
            //    //sprintf(expression->llvm_name, "%s", getchild(definition->node, 1)->llvm_name);
            //    //codegen_indent(ind);
            //    //printf("; Var Identifier \"%s\"\n", expression->llvm_name);
            //}


            break;



        case Int:
        case Natural:
            sprintf(expression->llvm_name, "%%%d", temporary++);
            codegen_indent(ind);
            printf("%s = add i32 %s, 0\n", expression->llvm_name, expression->token);
            break;

        case Float32:
        case Decimal:
            sprintf(expression->llvm_name, "%%%d", temporary++);
            codegen_indent(ind);
            printf("%s = fadd double %s, 0.0\n", expression->llvm_name, expression->token);
            break;
        
        case Bool:
            // Bool never exists as a terminal Node, only as a type_node in a VarDecl
            break;

        case String:
        case StrLit:

            break;
        


        case Add:
        case Sub:
        case Mul:
        case Div:
        case Mod:
            ;           // n percebo
            left_expression = getchild(expression, 0);
            right_expression = getchild(expression, 1);

            codegen_indent(ind);
            printf("; MATH OPERATION \"%s\"\n", category_to_llvm3[expression->category]);

            codegen_expression(left_expression, ind+1);
            codegen_expression(right_expression, ind+1);

            sprintf(expression->llvm_name, "%%%d", temporary++);

            codegen_indent(ind);
            printf("%s = ", expression->llvm_name);
            if (left_expression->type==float32_type) {
                printf("f");
            } else {
                if (expression->category==Div || expression->category==Mod) {
                    printf("s");
                }
            }

            printf("%s %s %s, %s;\n", category_to_llvm3[expression->category], type_to_llvm3[expression->type], left_expression->llvm_name, right_expression->llvm_name);

            break;
        


        case Plus:
            // do nothing?
            break;
        case Minus:
            right_expression = getchild(expression, 0);

            codegen_indent(ind);
            printf("; MINUS\n");
            
            codegen_expression(right_expression, ind+1);

            sprintf(expression->llvm_name, "%%%d", temporary++);

            codegen_indent(ind);
            printf("%s = ", expression->llvm_name);
            if (right_expression->type==float32_type) {
                printf("f");
            }
            printf("sub %s -%s, %s\n", type_to_llvm3[right_expression->type], empty_type_llvm[right_expression->type], right_expression->llvm_name);
            break;
        case Not:
            right_expression = getchild(expression, 0);

            codegen_indent(ind);
            printf("; NOT\n");

            codegen_expression(right_expression, ind+1);

            sprintf(expression->llvm_name, "%%%d", temporary++);

            codegen_indent(ind);
            printf("%s = xor i1 %s, true\n", expression->llvm_name, right_expression->llvm_name);
            break;



        case Or:
        case And:
            ;
            left_expression = getchild(expression, 0);
            right_expression = getchild(expression, 1);

            codegen_indent(ind);
            printf("; LOGIC OPERATION \"%s\"\n", category_to_llvm3[expression->category]);

            codegen_expression(left_expression, ind+1);
            codegen_expression(right_expression, ind+1);

            sprintf(expression->llvm_name, "%%%d", temporary++);

            codegen_indent(ind);
            printf("%s = %s i1 %s, %s\n", expression->llvm_name, category_to_llvm3[expression->category], left_expression->llvm_name, right_expression->llvm_name);
            break;



        case Eq:
        case Ne:
            if (op_prefix=='\n') { op_prefix=' '; }
        case Lt:
        case Le:
        case Gt:
        case Ge:
            if (op_prefix=='\n') { if (expression->type == float32_type) {op_prefix='o';} else {op_prefix='s';} }
            ;
            left_expression = getchild(expression, 0);
            right_expression = getchild(expression, 1);

            codegen_indent(ind);
            printf("; COMPARISON \"%s\"\n", category_to_llvm3[expression->category]);

            codegen_expression(left_expression, ind+1);
            codegen_expression(right_expression, ind+1);

            sprintf(expression->llvm_name, "%%%d", temporary++);

            codegen_indent(ind);
            if (expression->type == float32_type) {
                printf("%s = fcmp %c%s %s %s, %s\n", expression->llvm_name, op_prefix, category_to_llvm3[expression->category], type_to_llvm3[left_expression->type], left_expression->llvm_name, right_expression->llvm_name);
            } else {
                printf("%s = icmp %c%s %s %s, %s\n", expression->llvm_name, op_prefix, category_to_llvm3[expression->category], type_to_llvm3[left_expression->type], left_expression->llvm_name, right_expression->llvm_name);
            }

            break;




        default:
            codegen_statement(expression, ind);
            break;
    }
}



void codegen_call(struct node *return_node, int ind) {
    struct node *id_node = getchild(return_node, 0);

    struct symbol_list *definition = search_symbol(symbol_table, id_node->token, -1, true);
    if (definition==NULL) {
        printf("CALLHOU\n");
        return;
    }

    // resolve param expressions
    struct node *cur_param;
    int curr = 1;
    codegen_indent(ind);
    printf("; CALL \"%s\"\n", definition->identifier);
    codegen_indent(ind+1);
    printf("; PARAMS\n");
    while((cur_param = getchild(return_node, curr++)) != NULL) {
        codegen_expression(cur_param, ind+1);
    }


    codegen_indent(ind);
    if (definition->type!=none_type) {
        sprintf(return_node->llvm_name, "%%%d", temporary++);
        printf("%s = ",return_node->llvm_name);
    }
    printf("call %s @_%s(", type_to_llvm3[definition->type], definition->identifier);
    curr = 1;
    while((cur_param = getchild(return_node, curr++)) != NULL) {
        if (curr>2)
            printf(", ");
        printf("%s %s", type_to_llvm3[cur_param->type], cur_param->llvm_name);
    }
    printf(")\n");
}



void codegen_assign(struct node *assign_node, int ind) {
    struct node *var = getchild(assign_node, 0);
    struct node *expr = getchild(assign_node, 1);

    // find definition of var;
    struct symbol_list *definition = search_symbol(cur_scope, var->token, -1, false);

    // resolve expr
    codegen_indent(ind);
    printf("; ASSIGN \"%s\"\n", var->token);
    codegen_expression(expr, ind+1);

    codegen_indent(ind);
    //printf("store %s %s, %s* %s\n", type_to_llvm3[expr->type], expr->llvm_name, type_to_llvm3[definition->type], getchild(definition->node,1)->llvm_name);

    if (getchild(definition->node,1)->llvm_name[0]=='@') {
        codegen_indent(ind);
        printf("store %s %s, %s* %s\n", type_to_llvm3[expr->type], expr->llvm_name, type_to_llvm3[definition->type], getchild(definition->node,1)->llvm_name);
    } else {
        printf("store %s %s, %s* %s\n", type_to_llvm3[expr->type], expr->llvm_name, type_to_llvm3[definition->type], getchild(definition->node,1)->llvm_name);
    }
}



void codegen_return(struct node *return_node, int ind) {
    printf("\n");
    struct node *value_node = getchild(return_node, 0);

    codegen_indent(ind);
    printf("; RETURN\n");

    if (value_node==NULL) {
        codegen_indent(ind);
        printf("ret void\n");
        return;
    }

    codegen_expression(value_node, ind+1);

    sprintf(return_node->llvm_name, "%%%d", temporary++);

    if (value_node!=NULL) {
        codegen_indent(ind+1);
        //printf("store %s* %s, %s* %s\n", type_to_llvm3[value_node->type], value_node->llvm_name, type_to_llvm3[value_node->type], return_node->llvm_name);
        if (value_node->llvm_name[0] == '@') {
            printf("%s = load %s, %s* %s\n", return_node->llvm_name, type_to_llvm3[value_node->type], type_to_llvm3[value_node->type], value_node->llvm_name );
        } else {
            printf("%s = add %s %s, 0\n", return_node->llvm_name, type_to_llvm3[value_node->type], value_node->llvm_name);
        }
    }

    codegen_indent(ind);
    printf("ret %s %s\n", type_to_llvm3[value_node->type], return_node->llvm_name);
    
}


void codegen_block(struct node *block, int ind) {
    struct node *cur_statement;
    int curr = 0;
    while((cur_statement = getchild(block, curr++)) != NULL) {
        codegen_statement(cur_statement, ind);
    }
}



void codegen_print(struct node *print_node, int ind) {
    struct node *expr = getchild(print_node, 0);

    codegen_indent(ind);
    printf("; PRINTING\n");
    codegen_expression(expr, ind+1);

    //sprintf(print_node->llvm_name, "%%%d", temporary++);
    //temporary++;

    codegen_indent(ind);
    switch (expr->type) {
        case int_type:
            printf("call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.print_int, i64 0, i64 0), i32 %s)\n", /*print_node->llvm_name,*/ expr->llvm_name);
            temporary++;
            break;
        
        case float32_type:
            printf("call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str.print_float, i64 0, i64 0), double %s)\n", /*print_node->llvm_name,*/ expr->llvm_name);
            temporary++;
            break;
        
        case string_type:
            printf("call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([%zu x i8], [%zu x i8]* %s, i64 0, i64 0))\n", /*print_node->llvm_name,*/ expr->strlit_size, expr->strlit_size, expr->llvm_name);
            temporary++;
            break;

        case bool_type:
            // TODO
            break;

        default:
            break;
    }
}



void codegen_if(struct node *if_node, int ind) {
    printf("\n");
    int if_id = ++if_count;
    struct node *condition = getchild(if_node, 0);
    struct node *then_node = getchild(if_node, 1);
    struct node *else_node = getchild(if_node, 2);

    codegen_indent(ind);
    printf("; IF\n");

    codegen_indent(ind+1);
    printf("; IF - CONDITION\n");
    codegen_expression(condition, ind+1);

    codegen_indent(ind);
    printf("br i1 %s, label %%If%dthen, label %%If%delse\n", condition->llvm_name, if_count, if_count);


    codegen_indent(ind);
    printf("If%dthen:\n", if_id);
    codegen_block(then_node, ind+1);
    codegen_indent(ind+1);
    printf("br label %%If%dend\n", if_id); //temporary++;
    

    codegen_indent(ind);
    printf("If%delse:\n", if_id);
    codegen_block(else_node, ind+1);
    codegen_indent(ind+1);
    printf("br label %%If%dend\n", if_id); //temporary++;


    codegen_indent(ind);
    printf("If%dend:\n", if_id);
}


void codegen_for(struct node *for_node, int ind) {
    printf("\n");
    int for_id = ++for_count;
    struct node *condition = NULL;
    struct node *block_node = getchild(for_node, 1);

    if (block_node==NULL) {
        block_node = getchild(for_node, 0);
    } else {
        condition = getchild(for_node, 0);
    }

    codegen_indent(ind);
    printf("; FOR\n");
    codegen_indent(ind);
    printf("br label %%For%dcondition\n", for_id);
    codegen_indent(ind);
    printf("For%dcondition:\n", for_id);

    codegen_indent(ind+1);
    printf("; FOR - CONDITION\n");
    if (condition!=NULL) {
        codegen_expression(condition, ind+1);
        codegen_indent(ind+1);
        printf("br i1 %s, label %%For%dblock, label %%For%dend\n", condition->llvm_name, for_id, for_id);
    } else {
        codegen_indent(ind+1);
        printf("br label %%For%dblock", for_id);
    }

    codegen_indent(ind);
    printf("For%dblock:\n", for_id);
    codegen_indent(ind);
    printf("; FOR - BLOCK\n");

    codegen_block(block_node, ind+1);
    codegen_indent(ind);
    printf("br label %%For%dcondition\n", for_id);

    codegen_indent(ind);
    printf("For%dend:\n", for_id);

}


void codegen_parseargs(struct node *parseargs, int ind) {
    struct node *args = getchild(parseargs, 0);
    codegen_indent(ind);
    printf("; PARSE ARGS\n");
    codegen_expression(args, ind+1);

    sprintf(parseargs->llvm_name, "%%%d", temporary++);

    codegen_indent(ind);
    printf("%s = call i32 @atoi(i8* %s)\n", parseargs->llvm_name, args->llvm_name);
}


void codegen_statement(struct node *statement, int ind) {
    if (statement->category == Call) {
        //check_Call(statement, symbol_scope);
        codegen_call(statement, ind);

    } else if (statement->category == Assign) {
        //check_Assign(statement, symbol_scope);
        codegen_assign(statement, ind);

    } else if (statement->category == Return) {
        //check_Return(statement, symbol_scope);
        codegen_return(statement, ind);

    } else if (statement->category == Print) {
        //check_Print(statement, symbol_scope);
        codegen_print(statement, ind);

    } else if (statement->category == If) {
        codegen_if(statement, ind);

    } else if (statement->category == For) {
        //check_For(statement, symbol_scope);
        codegen_for(statement, ind);

    } else if (statement->category == ParseArgs) {
        //check_ParseArgs(statement, symbol_scope);
    }
}




void codegen_funcheaderparams(struct node *params) {
    struct node *cur_param;
    int curr = 0;
    while((cur_param = getchild(params, curr++)) != NULL) {
        struct node *type_node = getchild(cur_param, 0);
        struct node *id_node = getchild(cur_param, 1);
        if (curr > 1) {
            printf(", ");
        }
        sprintf(id_node->llvm_name, "%%%s.ptr", id_node->token);
        printf("%s %%%s", type_to_llvm3[category_to_type3[type_node->category]], id_node->token);   // cant be ->llvm_name, because it can't have the .ptr part
    }
}



struct node *codegen_funcheader(struct node *funcheader, int ind) {
    // returns type_node, for convenience

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
    codegen_funcheaderparams(params_node);
    printf(") {\n");


    // add all local variables (params and local vars)
    if (params_node==NULL) {
        codegen_indent(ind+1);
        printf("; NO LOCAL VARs\n");
        return type_node;
    }
    codegen_indent(ind+1);
    printf("; LOCAL VAR DECLARATIONS\n");
    struct symbol_list *symbol;
    for(symbol = cur_scope->next->next; symbol != NULL; symbol = symbol->next) {
        if (strcmp(symbol->identifier, "return")==0) {
            continue;
        }
        struct node *cur_var = symbol->node;
        struct node *var_type = getchild(cur_var, 0);
        struct node *var_id = getchild(cur_var, 1);

        if (var_id->llvm_name[0]=='\0') {
            sprintf(var_id->llvm_name, "%%%s.ptr", var_id->token);
        }


        codegen_indent(ind+2);
        printf("%s = alloca %s\t\t; \\", var_id->llvm_name, type_to_llvm3[symbol->type]);

        if (symbol->is_param) {
            printf(" Input param \"%s\"\n", var_id->token);
        } else {
            printf(" Local variable \"%s\"\n", var_id->token);
        }

        if(var_type->type == string_type){
            // Initialize to empty string
            codegen_indent(ind+2);
            printf("store i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.empty_str, i32 0, i32 0), i8** %s\t; /\n", var_id->llvm_name);
        } else if (symbol->is_param) {
            // Store parameter value
            codegen_indent(ind+2);
            printf("store %s %%%s, %s* %s\t; /\n", type_to_llvm3[symbol->type], var_id->token, type_to_llvm3[symbol->type], var_id->llvm_name);
        }

    }
    codegen_indent(ind+1);
    printf("; LOCAL VAR DECLARATIONS - end\n\n");

    return type_node;
}



void codegen_funcbody(struct node *funcbody, int ind) {
    temporary = 1;
    struct node_list *child = funcbody->children;
    //while ((child=child->next) != NULL) {
    //    if (child->node->category == VarDecl) {
    //        // vardecl
    //        //codegen_statement(child->node, ind);
    //        // THIS IS DONE BY THE codegen_funcheader
    //    }
    //}

    child = funcbody->children;
    while ((child=child->next) != NULL) {
        if (child->node->category != VarDecl) {
            // statement
            codegen_statement(child->node, ind);

        }
    }
}



void codegen_function(struct node *node, int ind) {
    // declare the function


    struct node *funcheader, *funcbody;
    funcheader = getchild(node, 0);
    funcbody = getchild(node, 1);

    cur_scope = search_symbol(symbol_table, getchild(funcheader,0)->token, -1, true)->child_scope;

    struct node *type_node = codegen_funcheader(funcheader, ind);

    codegen_funcbody(funcbody, ind+1);

    codegen_indent(ind+1);
    if (type_node!=NULL) {
        printf("ret %s %s\n", type_to_llvm3[category_to_type3[type_node->category]], empty_type_llvm[category_to_type3[type_node->category]]);
    } else {
        printf("ret void\n");
    }
    //printf("ret i32 0\n"/*, type_to_llvm3[node->type]*/);       // TODO fix this

    codegen_indent(ind); printf("}\n");
}



int codegen_recur_stringdecl(struct node *node, int ind, int str_count) {
    struct node *next_node;
    int curr = 0;
    if (node->category == StrLit) {
        str_count++;
        sprintf(node->llvm_name, "@.str.%d", str_count);
        memset(strlit_buff, '\0', 4096);
        for (int i=1; node->token[i]!='\0'; i++) {
            node->strlit_size++;
            if (node->token[i]=='\\') {
                i++;
                if (node->token[i]=='n') {
                    sprintf(strlit_buff+strlen(strlit_buff), "\\0A");
                } else if (node->token[i]=='t') {
                    sprintf(strlit_buff+strlen(strlit_buff), "\\09");
                } else if (node->token[i]=='\\') {
                    sprintf(strlit_buff+strlen(strlit_buff), "\\5C");
                } else if (node->token[i]=='\"') {
                    sprintf(strlit_buff+strlen(strlit_buff), "\\22");
                } else if (node->token[i]=='f') {
                    sprintf(strlit_buff+strlen(strlit_buff), "\\0C");
                } else if (node->token[i]=='r') {
                    sprintf(strlit_buff+strlen(strlit_buff), "\\0D");
                }
                continue;
            }
            if (node->token[i]=='%') {
                sprintf(strlit_buff+strlen(strlit_buff), "%%%%");
                node->strlit_size++;
                continue;
            }
            sprintf(strlit_buff+strlen(strlit_buff), "%c", node->token[i]);
        }
        // remove last '"'
        sprintf(strlit_buff+strlen(strlit_buff)-1, "");

        // add "\n" to the end
        sprintf(strlit_buff+strlen(strlit_buff), "\\0A");

        // finally print the declaration
        codegen_indent(ind);
        printf("@.str.%d = private unnamed_addr constant [%zu x i8] c\"%s\\00\" align 1\n", str_count, ++node->strlit_size, strlit_buff);  // +1 on the strlit_size because we added the \n to the string
    }
    while ((next_node = getchild(node, curr++)) != NULL) {
        str_count = codegen_recur_stringdecl(next_node, 0, str_count);
    }

    return str_count;
}



void codegen_program(struct node *program) {
    printf("; bem fixe este programa\n");

    printf("; ----- Print Strings -----\n");
    printf("@.str.print_int = private unnamed_addr constant  [4 x i8] c\"%%d\\0A\\00\" align 1\n");
    printf("@.str.print_float = private unnamed_addr constant  [7 x i8] c\"%%.08f\\0A\\00\" align 1\n");
    printf("@.str.print_true = private unnamed_addr constant  [6 x i8] c\"true\\0A\\00\" align 1\n");
    printf("@.str.print_false = private unnamed_addr constant  [7 x i8] c\"false\\0A\\00\" align 1\n");
    printf("@.str.empty_str = private unnamed_addr constant [1 x i8] c\"\\00\" align 1\n\n");

    printf("\n\n; ----- String Declarations -----\n");
    codegen_recur_stringdecl(program, 0, 0);

    printf("\n\n; ----- Imported Function -----\n"),
    printf("declare i32 @atoi(i8*)\n");
    printf("declare i32 @printf(i8*,  ...)\n");

    // setup all the global variables
    printf("\n\n; ----- Global Variables -----\n");
    struct node *cur_node;
    int curr = 0;
    while((cur_node = getchild(program, curr++)) != NULL) {
        if (cur_node->category == VarDecl) {
            codegen_globalvar(cur_node, 0);
        }
    }

    // setup all the functions
    printf("\n\n; ----- Functions -----\n");
    curr = 0;
    while((cur_node = getchild(program, curr++)) != NULL) {
        if (cur_node->category == FuncDecl) {
            codegen_function(cur_node, 0);
        }
    }

    // add entry point
    printf("\n; ----- Entry point -----\n"
           "define i32 @main() {\n"
           "  call void @_main()\n"
           "  ret i32 0\n"
           "}\n");

}