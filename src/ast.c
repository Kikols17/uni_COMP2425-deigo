#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"

const char *category_names[] = { "Program", "VarDecl", "FuncDecl", "FuncHeader", "FuncParams", "FuncBody", "ParamDecl", "Assign", "Int", "Float32", "Bool", "String", "Natural", "Decimal", "Identifier", "StrLit", "For", "If", "Block", "Call", "Return", "Print", "ParseArgs", "Or", "And", "Eq", "Ne", "Lt", "Le", "Gt", "Ge", "Add", "Sub", "Mul", "Div", "Mod", "Not", "Minus", "Plus", "AUX" };

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

    // Don't print auxiliary nodes
    if(node->category != AUX){
        // Add indentation according to the current depth
        for(int i = 0; i < depth; i++){
            printf("..");
        }    
        // Print the category and the token (if it exists)
        if(node->token == NULL){
            printf("%s\n", category_names[node->category]);
        }
        else{
            printf("%s(%s)\n", category_names[node->category], node->token);
        }
    }
    
    // Visit all children
    if(node->children == NULL){
        return;
    }
    struct node_list *child = node->children;
    while((child = child->next) != NULL){
        if(child->node->category == AUX){
            // Don't increase the depth for auxiliary nodes
            show(child->node, depth);
        }
        else{
            show(child->node, depth+1);
        }
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
    //printf("pau\n");
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
        if (child->node->category != AUX) {
            count++;
        } else {
            count += countchildren(child->node);
        }
    }

    return count;
}