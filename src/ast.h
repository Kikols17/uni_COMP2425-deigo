#ifndef _AST_H
#define _AST_H


#include <stddef.h>   // preciso para o NULL (n sei pq nunca foi preciso)

enum category {   AUX ,  Program ,  VarDecl ,  FuncDecl ,  FuncHeader ,  FuncParams ,  FuncBody ,  ParamDecl ,  Block ,  If ,  For ,  Return ,  Call ,  Print ,  ParseArgs ,  Or ,  And ,  Eq ,  Ne ,  Lt ,  Gt ,  Le ,  Ge ,  Add ,  Sub ,  Mul ,  Div ,  Mod ,  Not ,  Minus ,  Plus ,  Assign ,  Int ,  Float32 ,  Bool ,  String ,  Natural ,  Decimal ,  Identifier ,  StrLit  };
#define names {  "AUX", "Program", "VarDecl", "FuncDecl", "FuncHeader", "FuncParams", "FuncBody", "ParamDecl", "Block", "If", "For", "Return", "Call", "Print", "ParseArgs", "Or", "And", "Eq", "Ne", "Lt", "Gt", "Le", "Ge", "Add", "Sub", "Mul", "Div", "Mod", "Not", "Minus", "Plus", "Assign", "Int", "Float32", "Bool", "String", "Natural", "Decimal", "Identifier", "StrLit" }

struct node {
    enum category category;
    char *token;
    struct node_list *children;
};

struct node_list {
    struct node *node;
    struct node_list *next;
};

struct node *newnode(enum category category, char *token);
void addchild(struct node *parent, struct node *child);
void show(struct node *node, int depth);

void reversenode(struct node* node);
void addchild_allchildren(struct node *parent, struct node *child);
int countchildren(struct node *node);


#endif
