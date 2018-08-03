#ifndef _UT_TREE_H_
#define _UT_TREE_H_

void ut_mk_identifier()
{
    BYTE op = OP_INPUT;

    BYTE byte = 1;
    BYTE bit = 1;
    
    item_t id = mk_identifier(op, byte, bit);
    
    CU_ASSERT(id->tag == TAG_IDENTIFIER);
    CU_ASSERT(id->v.id.operand == op);
    CU_ASSERT(id->v.id.byte == byte);
    CU_ASSERT(id->v.id.bit == bit);
   
    id = clear_tree(id);
    CU_ASSERT(id==NULL);
} 

void ut_mk_expression()
{
    item_t id1 = mk_identifier(OP_INPUT,  0, 0);
    item_t id2 = mk_identifier(OP_MEMORY,  1, 8);
    //degenarate input does not understand christ
    item_t dummy = mk_expression(NULL, NULL, 0, 0);
    dummy = clear_tree(dummy);
    
    item_t it = mk_expression(id1, id2, IL_AND, IL_PUSH);
    CU_ASSERT(it->tag == TAG_EXPRESSION); 
    CU_ASSERT(it->v.exp.op == IL_AND);
    CU_ASSERT(it->v.exp.mod == IL_PUSH);
    CU_ASSERT(it->v.exp.a->v.id.operand == OP_INPUT);
    CU_ASSERT(it->v.exp.b->v.id.operand == OP_MEMORY);
    it = clear_tree(it);
    CU_ASSERT(it==NULL);
    CU_ASSERT(id1->tag==0);
    CU_ASSERT(id2->tag==0);
}

void ut_mk_assignment()
{
    item_t dummy = mk_assignment(NULL, NULL, 0);
    CU_ASSERT(dummy->tag == TAG_ASSIGNMENT); 
    dummy = clear_tree(dummy);
    
    item_t id1 = mk_identifier(OP_INPUT,  0, 0);
    item_t id2 = mk_identifier(OP_MEMORY,  1, 8);
    item_t exp = mk_expression(id1, NULL, IL_LD, IL_NORM);
    item_t ass = mk_assignment(id2, exp, LD_COIL);
    CU_ASSERT(ass->v.ass.left->v.id.operand == OP_MEMORY);
    CU_ASSERT(ass->v.ass.right->v.exp.a->v.id.operand == OP_INPUT);
    CU_ASSERT(ass->v.ass.type == LD_COIL);
    ass = clear_tree(ass);
} 

#endif //_UT_TREE_H_

