#ifndef _AST_H
#define _AST_H


#include <stddef.h>   // preciso para o NULL (n sei pq nunca foi preciso)

enum category               {  Program,      VarDecl,      FuncDecl,     FuncHeader,   FuncParams,   FuncBody,     ParamDecl,    Assign,       Int,        Natural,   Decimal,        Float32,        Bool,        String,        Identifier,   StrLit,       For,          If,           Block,        Call,         Return,       Print,        ParseArgs,    Or,           And,          Eq,           Ne,           Lt,           Le,           Gt,           Ge,           Add,          Sub,          Mul,          Div,          Mod,          Not,          Minus,        Plus,         TEMP,          None};
#define CATEGORY_NAMES      { "Program",    "VarDecl",    "FuncDecl",   "FuncHeader", "FuncParams", "FuncBody",   "ParamDecl",  "Assign",     "Int",      "Natural", "Decimal",      "Float32",      "Bool",      "String",      "Identifier", "StrLit",     "For",        "If",         "Block",      "Call",       "Return",     "Print",      "ParseArgs",  "Or",         "And",        "Eq",         "Ne",         "Lt",         "Le",         "Gt",         "Ge",         "Add",        "Sub",        "Mul",        "Div",        "Mod",        "Not",        "Minus",      "Plus",       "TEMP"         "None"};
#define CATEGORY_TO_TYPE    {  undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   int_type,   int_type,  float32_type,   float32_type,   bool_type,   string_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   undef_type,   none_type }

enum type           {  bool_type,  int_type,  float32_type,  string_type,  undef_type,  none_type,  EMPTY  };   // precisa de "_type", para não colidir com os types de C
#define TYPE_NAMES  { "bool",     "int",     "float32",     "string",     "undef",     "none",     "EMPTY" };


struct node {
    enum category category;
    char *token;
    enum type type;
    int token_line;
    int token_column;
    struct node_list *children;
};

struct node_list {
    struct node *node;
    struct node_list *next;
};

struct node *newnode(enum category category, char *token);
void addchild(struct node *parent, struct node *child);
struct node *getchild(struct node *parent, int position);
void show(struct node *node, int depth);

void reversenode(struct node* node);
void addchild_allchildren(struct node *parent, struct node *child);
int countchildren(struct node *node);
void clean_tree(struct node *parent);


#endif
