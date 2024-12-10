#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantics.h"
#include "codegen.h"

extern struct symbol_list *symbol_table;

const char *type_to_llvm3[] = TYPE_TO_LLVM;
const char *category_to_llvm3[] = CATEGORY_TO_LLVM;
const enum type category_to_type3[] = CATEGORY_TO_TYPE;

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
    printf("%s %s\n", type_to_llvm3[category_to_type3[type_node->category]], id_node->llvm_name);
}





void codegen_expression(struct node *expression, int ind) {
    struct node *left_expression;
    struct node *right_expression;
    struct symbol_list *definition;

    switch(expression->category) {
        case Identifier:
            // find out the identifier's declaration node (we assume it exists)
            definition = search_symbol(cur_scope, expression->token, -1, false);
            sprintf(expression->llvm_name, "%s", getchild(definition->node, 1)->llvm_name);
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
            printf("%s = fadd float %s, 0\n", expression->llvm_name, expression->token);
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
        


        case Minus:
        case Plus:
        case Not:
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
        case Lt:
        case Le:
        case Gt:
        case Ge:
            ;
            left_expression = getchild(expression, 0);
            right_expression = getchild(expression, 1);

            codegen_indent(ind);
            printf("; COMPARISON \"%s\"\n", category_to_llvm3[expression->category]);

            codegen_expression(left_expression, ind+1);
            codegen_expression(right_expression, ind+1);

            sprintf(expression->llvm_name, "%%%d", temporary++);

            codegen_indent(ind);
            printf("%s = icmp %s %s %s, %s\n", expression->llvm_name, category_to_llvm3[expression->category], type_to_llvm3[left_expression->type], left_expression->llvm_name, right_expression->llvm_name);

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
        printf("CALLHOU");
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

    sprintf(return_node->llvm_name, "%%%d", temporary++);

    codegen_indent(ind);
    printf("%s = call %s @_%s(", return_node->llvm_name, type_to_llvm3[definition->type], definition->identifier);
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
    printf("store %s %s, %s* %s\n", type_to_llvm3[expr->type], expr->llvm_name, type_to_llvm3[definition->type], definition->identifier);
}



void codegen_return(struct node *return_node, int ind) {

    struct node *value_node = getchild(return_node, 0);

    codegen_indent(ind);
    printf("; RETURN\n");

    codegen_expression(value_node, ind+1);

    codegen_indent(ind);
    printf("ret %s %s\n", type_to_llvm3[value_node->type], value_node->llvm_name);
}


void codegen_block(struct node *block, int ind) {
    struct node *cur_statement;
    int curr = 0;
    while((cur_statement = getchild(block, curr++)) != NULL) {
        codegen_statement(cur_statement, ind);
    }
}



void codegen_if(struct node *if_node, int ind) {
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
    printf("br label %%If%dend\n", if_id); temporary++;
    

    codegen_indent(ind);
    printf("If%delse:\n", if_id);
    codegen_block(else_node, ind+1);
    codegen_indent(ind+1);
    printf("br label %%If%dend\n", if_id); temporary++;


    codegen_indent(ind);
    printf("If%dend:\n", if_id);
}


void codegen_statement(struct node *statement, int ind) {
    if (statement->category == Call) {
        //check_Call(statement, symbol_scope);
        codegen_call(statement, ind);

    } else if (statement->category == Assign) {
        //check_Assign(statement, symbol_scope);

    } else if (statement->category == Return) {
        //check_Return(statement, symbol_scope);
        codegen_return(statement, ind);

    } else if (statement->category == Print) {
        //check_Print(statement, symbol_scope);

    } else if (statement->category == If) {
        codegen_if(statement, ind);

    } else if (statement->category == For) {
        //check_For(statement, symbol_scope);

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
        sprintf(id_node->llvm_name, "%%%s", id_node->token);
        printf("%s %s", type_to_llvm3[category_to_type3[type_node->category]], id_node->llvm_name);
    }
}



void codegen_funcheader(struct node *funcheader, int ind) {

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
    printf(")");
}



void codegen_funcbody(struct node *funcbody, int ind) {
    temporary = 1;
    struct node_list *child = funcbody->children;
    while ((child=child->next) != NULL) {
        if (child->node->category == VarDecl) {
            // vardecl
            //codegen_statement(child->node, ind);

        }
    }

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

    codegen_funcheader(funcheader, ind);
    codegen_indent(ind); printf(" {\n");
    codegen_funcbody(funcbody, ind+1);

    codegen_indent(ind+1);
    printf("ret i32 0\n", type_to_llvm3[node->type]);       // TODO fix this

    codegen_indent(ind); printf("}\n");
}



void codegen_program(struct node *program) {
    printf("; bem fixe este programa\n");

    // setup all the global variables
    printf("; ----- Global variables -----\n");
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
           "  %%1 = call i32 @_main(i32 0)\n"
           "  ret i32 %%1\n"
           "}\n");

}