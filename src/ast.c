#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"

const char *category_names[] = { "Program", "VarDecl", "FuncDecl", "FuncHeader", "FuncParams", "FuncBody", "ParamDecl", "Assign", "Int", "Float32", "Bool", "String", "Natural", "Decimal", "Identifier", "StrLit", "For", "If", "Block", "Call", "Return", "Print", "ParseArgs", "Or", "And", "Eq", "Ne", "Lt", "Le", "Gt", "Ge", "Add", "Sub", "Mul", "Div", "Mod", "Not", "Minus", "Plus", "TEMP" };

// create a node of a given category with a given lexical symbol
struct node *newnode(enum category category, char *token) {
    struct node *new = malloc(sizeof(struct node));
    new->category = category;
    new->token = token;
    new->children = malloc(sizeof(struct node_list));
    new->children->node = NULL;
    new->children->next = NULL;
    return new;
}

// append a node to the list of children of the parent node
void addchild(struct node *parent, struct node *child) {
    //show(parent, 0);
    //printf("Adding child\n");

    struct node_list *new = malloc(sizeof(struct node_list));
    new->node = child;
    new->next = NULL;
    struct node_list *children = parent->children;
    while(children->next != NULL)
        children = children->next;
    children->next = new;
}

// shows the AST
void show(struct node *node, int depth) {
    // no more nodes
    if(node == NULL){
        return;
    }

    // Indentation
    for(int i = 0; i < depth; i++) {
        printf("..");
    }    
    // Category (option)
    if(node->token == NULL) {
        printf("%s\n", category_names[node->category]);
    } else {
        printf("%s(%s)\n", category_names[node->category], node->token);
    }
    
    // Visit all children
    if(node->children == NULL) {
        return;
    }
    struct node_list *child = node->children;
    while((child = child->next) != NULL) {
        show(child->node, depth+1);
    }
}



void reversenode(struct node* node) {
    // if node does not exist or has no children or has only one child, return
    if (node == NULL || node->children == NULL || node->children->next == NULL) {
        return;
    }

    struct node_list *prev = NULL;
    struct node_list *current = node->children->next;
    struct node_list *next = NULL;

    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    node->children->next = prev;

    struct node_list *child = node->children->next;
    while (child != NULL) {
        reversenode(child->node);
        child = child->next;
    }

}

void addchild_allchildren(struct node *parent, struct node *child) {
    // adds the child node to all children of the parent node
    if (parent == NULL || child == NULL) {
        return;
    }

    //printf("pai: %p\n", parent);
    struct node_list *childlist = parent->children;
    while ((childlist = childlist->next)->next->next != NULL) {
        //printf("filho: %p\n", childlist->node);
        addchild(childlist->node, child);
    }
}

int countchildren(struct node *node) {
    // returns the number of children of a node

    if (node == NULL || node->children == NULL) {
        return 0;
    }

    int count = 0;
    struct node_list *child = node->children;
    while ((child = child->next) != NULL) {
        if (child->node->category != TEMP) {
            count++;
        } else {
            count += countchildren(child->node);
        }
    }

    return count;
}


void clean_tree(struct node *program) {
    /* Run this to clear the tree of TEMP nodes
     *
     */
    if (program == NULL || program->children == NULL) {
        return;
    }

    struct node_list *stack[1000]; // Stack to hold nodes for DFS
    int stack_size = 0;

    stack[stack_size++] = program->children;

    while (stack_size > 0) {
        struct node_list *current_list = stack[--stack_size];
        struct node_list *prev = current_list;
        struct node_list *current = prev->next;

        while (current != NULL) {
            if (current->node->category == TEMP) {
                struct node_list *aux_children = current->node->children->next;

                if (aux_children != NULL) {
                    struct node_list *last_aux_child = aux_children;
                    while (last_aux_child->next != NULL) {
                        last_aux_child = last_aux_child->next;
                    }

                    prev->next = aux_children;
                    last_aux_child->next = current->next;
                } else {
                    prev->next = current->next;
                }

                free(current->node->children);
                free(current->node);

                struct node_list *toremove = current;
                current = prev->next;
                free(toremove);
            } else {
                stack[stack_size++] = current->node->children;
                prev = current;
                current = current->next;
            }
        }
    }
}
