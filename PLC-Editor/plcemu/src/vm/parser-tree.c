
#include "plclib.h"
#include "parser-tree.h"

/*TODO: memory optimization: create a factory to allocate different
 size per node type*/
 
item_t mk_identifier(const BYTE operand,
                     const BYTE byte,
                     const BYTE bit)
{
    item_t r = (item_t)malloc(sizeof(struct item));
    memset(r, 0 , sizeof(struct item));
    r->tag = TAG_IDENTIFIER;
    r->v.id.operand = operand;
    r->v.id.byte = byte;
    r->v.id.bit = bit;
    return r;
}

item_t mk_expression(const item_t a,
                     const item_t b,
                     const BYTE op,
                     const BYTE mod)
{
    item_t r = (item_t)malloc(sizeof(struct item));
    memset(r, 0 , sizeof(struct item));
    r->tag = TAG_EXPRESSION;
    r->v.exp.op = op;
    r->v.exp.mod = mod;
    r->v.exp.a = a;
    r->v.exp.b = b;
    return r; 
}

item_t mk_assignment(const item_t identifier,
                     const item_t expression,
                     const BYTE type)
{
    item_t r = (item_t)malloc(sizeof(struct item));
    memset(r, 0 , sizeof(struct item));
    r->tag = TAG_ASSIGNMENT;
    r->v.ass.left = identifier;
    r->v.ass.right = expression;
    r->v.ass.type = type;
    return r;
}

item_t clear_tree(item_t root)
{
    item_t r = root;
    if(root){
        switch(root->tag){
            case TAG_EXPRESSION:
                r->v.exp.a = clear_tree(root->v.exp.a);
                r->v.exp.b = clear_tree(root->v.exp.b); 
                break;
                  
            case TAG_ASSIGNMENT:
                r->v.ass.left = clear_tree(root->v.ass.left);
                r->v.ass.right = clear_tree(root->v.ass.right); 
                break;
                
            default: 
               break;
        }
        //tree leaves
        if(r!=NULL){
            free(r);
            memset(r, 0, sizeof(struct item));
            r = (item_t)NULL;
        }
    }
    return r;
}


