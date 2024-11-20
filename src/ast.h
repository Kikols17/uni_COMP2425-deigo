#ifndef _AST_H
#define _AST_H


#include <stddef.h>   // preciso para o NULL (n sei pq nunca foi preciso)

enum category {  Program ,  VarDecl ,  FuncDecl ,  FuncHeader ,  FuncParams ,  FuncBody ,  ParamDecl ,  Assign ,  Int ,  Float32 ,  Bool ,  String ,  Natural ,  Decimal ,  Identifier ,  StrLit ,  For ,  If ,  Block ,  Call ,  Return ,  Print ,  ParseArgs ,  Or ,  And ,  Eq ,  Ne ,  Lt ,  Le ,  Gt ,  Ge ,  Add ,  Sub ,  Mul ,  Div ,  Mod ,  Not ,  Minus ,  Plus ,  AUX  };
#define names { "Program", "VarDecl", "FuncDecl", "FuncHeader", "FuncParams", "FuncBody", "ParamDecl", "Assign", "Int", "Float32", "Bool", "String", "Natural", "Decimal", "Identifier", "StrLit", "For", "If", "Block", "Call", "Return", "Print", "ParseArgs", "Or", "And", "Eq", "Ne", "Lt", "Le", "Gt", "Ge", "Add", "Sub", "Mul", "Div", "Mod", "Not", "Minus", "Plus", "AUX" }


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
