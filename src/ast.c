#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"

static char *category_names[] = { "Program", "Function", "Parameters", "Parameter", "Arguments", "Integer", "Double", "Identifier", "Natural", "Decimal", "Call", "If", "Add", "Sub", "Mul", "Div" };

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
    if(node==NULL) {
        return;
    }

    for(int i = 0; i < depth; i++){
        printf("__");
    }    

    // Don't print auxiliary nodes
    if(strcmp(category_names[node->category], "AUX")!=0){
        // Print the category and the token (if it exists)
        if(node->token==NULL){
            printf("%s\n", category_names[node->category]);
        }
        else{
            printf("%s(%s)\n", category_names[node->category], node->token);
        }
    }
    
    // Visit all children
    struct node_list *child = node->children;
    while((child = child->next) != NULL){

        if(strcmp(category_names[child->node->category], "AUX") == 0){
            // Don't increase the depth for auxiliary nodes
            show(child->node, depth);
        }
        else{
            show(child->node, depth+1);
        }
    }
}