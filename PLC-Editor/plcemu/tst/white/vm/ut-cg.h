#ifndef _UT_CG_H_
#define _UT_CG_H_

void ut_gen_expr()
{
    struct rung ru;
    
    memset(&ru, 0, sizeof(struct rung));
    
    instruction_t ins;
    //ERROR: null
    int result = gen_expr(NULL, NULL, 0); 
    CU_ASSERT(result == PLC_ERR);    
   
    item_t id1 = mk_identifier(OP_INPUT,  0, 0);
    item_t id2 = mk_identifier(OP_MEMORY, 1, 5);
    
    //ERROR: wrong tag
    result = gen_expr(id1, &ru, 0);
    CU_ASSERT(result == PLC_ERR);
    
    //ERROR: bad operator
    item_t it = mk_expression(NULL, NULL,-1, -1);
    result = gen_expr(it, &ru, 0);
    CU_ASSERT(result == ERR_BADOPERATOR);
    clear_tree(it);
    
    //ERROR: null left operand
    it = mk_expression(NULL, NULL, IL_AND, IL_NORM);
    result = gen_expr(it, &ru, 0);
    CU_ASSERT(result == ERR_BADOPERAND);
    clear_tree(it);
    
    //I0.0 AND M1.5 =>
    //
    //LD I0.0
    //AND M1.5
    it = mk_expression(id1, id2, IL_AND, IL_NORM);
    
    result = gen_expr(it, &ru, 0);
    get(&ru, 0, &ins);
    
    CU_ASSERT(ins->operation == IL_LD);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 0);
    
    get(&ru, 1, &ins);
    
    CU_ASSERT(ins->operation == IL_AND);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 5);
    
    clear_tree(it);
    clear_rung(&ru);
    //null right operand (just evaluate left branch)
    // A AND(null)

    id1 = mk_identifier(OP_INPUT,  0, 0);
    id2 = mk_identifier(OP_MEMORY, 1, 5);
    
    it = mk_expression(id1, NULL, IL_AND, IL_PUSH);
    
    result = gen_expr(it, &ru, 0);
    get(&ru, 0, &ins);
    // LD A
    CU_ASSERT(ru.insno == 1);
    CU_ASSERT(ins->operation == IL_LD);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 0);
    
    clear_tree(it);
    clear_rung(&ru);
    
    //I0.0 OR (M1.1 AND M1.2) =>
    id1 = mk_identifier(OP_INPUT,  0, 0);
    id2 = mk_identifier(OP_MEMORY, 1, 1);
    item_t id3 = mk_identifier(OP_MEMORY, 1, 2);
    
    item_t inner = mk_expression(id2, id3, IL_AND, IL_NORM);
    item_t outer = mk_expression(id1, inner, IL_OR, IL_PUSH);
    
    result = gen_expr(outer, &ru, 0);
    //
    //LD I0.0
    //OR(M1.1
    //AND M1.2
    //)
    CU_ASSERT(ru.insno == 4);
    get(&ru, 0, &ins);
    
    CU_ASSERT(ins->operation == IL_LD);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 0);
    
    get(&ru, 1, &ins);
    
    CU_ASSERT(ins->operation == IL_OR);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_PUSH);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 1);
    
    get(&ru, 2, &ins);
    
    CU_ASSERT(ins->operation == IL_AND);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 2);
    
    get(&ru, 3, &ins);
    
    CU_ASSERT(ins->operation == IL_POP);
    
    clear_tree(outer);
    clear_rung(&ru);
    
    //(I0.0 AND I0.1) OR (M1.1 AND M1.2) =>
    id1 = mk_identifier(OP_INPUT,  0, 0);
    id2 = mk_identifier(OP_MEMORY, 1, 1);
    id3 = mk_identifier(OP_MEMORY, 1, 2);
    item_t id4 = mk_identifier(OP_INPUT, 0, 1);
    
    item_t left = mk_expression(id1, id4, IL_AND, IL_NORM);
    item_t right = mk_expression(id2, id3, IL_AND, IL_NORM);
    outer = mk_expression(left, right, IL_OR, IL_PUSH);
    //
    //LD I0.0
    //AND I0.1
    //OR(M1.1
    //AND M1.2
    //)
    result = gen_expr(outer, &ru, 0);
    
    CU_ASSERT(ru.insno == 5);
    
    get(&ru, 0, &ins);
    
    CU_ASSERT(ins->operation == IL_LD);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 0);
    
    get(&ru, 1, &ins);
    
    CU_ASSERT(ins->operation == IL_AND);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 1);
    
    get(&ru, 2, &ins);
    
    CU_ASSERT(ins->operation == IL_OR);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_PUSH);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 1);
    
    get(&ru, 3, &ins);
    
    CU_ASSERT(ins->operation == IL_AND);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 2);
    
    get(&ru, 4, &ins);
    
    CU_ASSERT(ins->operation == IL_POP);
    
    
    clear_tree(outer);
    clear_rung(&ru);

//(I0.0 AND (I0.1 OR I0.2)) OR (M1.1 AND M1.2) =>
    id1 = mk_identifier(OP_INPUT,  0, 0);
    id2 = mk_identifier(OP_MEMORY, 1, 1);
    id3 = mk_identifier(OP_MEMORY, 1, 2);
    id4 = mk_identifier(OP_INPUT, 0, 1);
    item_t id5 = mk_identifier(OP_INPUT, 0, 2);
    
    inner = mk_expression(id4, id5, IL_OR, IL_NORM);
    
    left = mk_expression(id1, inner, IL_AND, IL_PUSH);
    right = mk_expression(id2, id3, IL_AND, IL_NORM);
    
    outer = mk_expression(left, right, IL_OR, IL_PUSH);
    
    //
    //LD I0.0
    //AND(I0.1
    //OR I0.2
    //)
    //OR(M1.1
    //AND M1.2
    //)
    
    result = gen_expr(outer, &ru, 0);
    
    CU_ASSERT(ru.insno == 7);
    
    get(&ru, 0, &ins);
    
    CU_ASSERT(ins->operation == IL_LD);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 0);
    
    get(&ru, 1, &ins);
    
    CU_ASSERT(ins->operation == IL_AND);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_PUSH);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 1);
    
    get(&ru, 2, &ins);
    
    CU_ASSERT(ins->operation == IL_OR);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 2);
    
    get(&ru, 3, &ins);
    CU_ASSERT(ins->operation == IL_POP);
    
    get(&ru, 4, &ins);
    CU_ASSERT(ins->operation == IL_OR);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_PUSH);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 1);
    
    get(&ru, 5, &ins);
    CU_ASSERT(ins->operation == IL_AND);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 2);
    
    get(&ru, 6, &ins);
    
    CU_ASSERT(ins->operation == IL_POP);
    
    clear_tree(outer);
    clear_rung(&ru);
}


void ut_gen_ass()
{
    struct rung ru;
    memset(&ru, 0, sizeof(struct rung));
    
    char dump[MAXSTR * MAXBUF];
    memset(dump, 0, MAXBUF * MAXSTR);
    //ERROR: null
    int result = gen_ass(NULL, NULL); 
    CU_ASSERT(result == PLC_ERR); 
    
    item_t id1 = mk_identifier(OP_INPUT,  0, 0);
    item_t exp = mk_expression(id1, NULL, IL_LD, IL_NORM);
    
    //ERROR: not an assignment
    
    result = gen_ass(exp, &ru);
    CU_ASSERT(result == PLC_ERR);
    
    //ERROR: left part not an identifier
    
    item_t ass = mk_assignment(exp, id1, LD_COIL);
    result = gen_ass(ass, &ru);
    CU_ASSERT(result == ERR_BADOPERAND);
    //clear_tree(ass);
    
    //ERROR: wrong type
    ass = mk_assignment(id1, NULL, 0);
    result = gen_ass(ass, &ru);
    CU_ASSERT(result == ERR_BADCOIL);
    clear_tree(ass);
   
    //ERROR: right part not an identifier or expression
    ass = mk_assignment(id1, NULL, LD_COIL);
    result = gen_ass(ass, &ru);
    CU_ASSERT(result == ERR_BADOPERATOR);
    //clear_tree(ass);
    
    //assign identifier1 := identifier2
    item_t id2 = mk_identifier(OP_MEMORY,  1, 8);
    id1 = mk_identifier(OP_INPUT,  0, 0);
    exp = mk_expression(id1, NULL, IL_LD, IL_NORM);
    ass = mk_assignment(id2, id1, LD_COIL);
    
    result = gen_ass(ass, &ru);
    CU_ASSERT(result == PLC_OK);
    //LD A
    //ST B
    CU_ASSERT(ru.insno == 2);
    
    instruction_t ins;

    get(&ru, 0, &ins);
    
    CU_ASSERT(ins->operation == IL_LD);
    CU_ASSERT(ins->operand == OP_INPUT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 0);
    
    get(&ru, 1, &ins);
    
    CU_ASSERT(ins->operation == IL_ST);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 8);
   
    clear_tree(ass);
    clear_rung(&ru);
    //assign identifier1 := expression
    id2 = mk_identifier(OP_MEMORY,  1, 8);
    id1 = mk_identifier(OP_INPUT,  0, 0);
    exp = mk_expression(id1, NULL, IL_AND, IL_NORM);
    ass = mk_assignment(id2, exp, LD_DOWN);
    
    result = gen_ass(ass, &ru);
    CU_ASSERT(result == PLC_OK);
    //LD A
    //ST B
    CU_ASSERT(ru.insno == 2);
    
    get(&ru, 1, &ins);
    
    CU_ASSERT(ins->operation == IL_ST);
    CU_ASSERT(ins->operand == OP_MEMORY);
    CU_ASSERT(ins->modifier == IL_NEG);
    CU_ASSERT(ins->byte == 1);
    CU_ASSERT(ins->bit == 8);
    
    clear_tree(ass);
    clear_rung(&ru); 
    
    //a := (b or null)
    id2 = mk_identifier(OP_CONTACT,  0, 1);
    id1 = mk_identifier(OP_INPUT,  0, 0);
    exp = mk_expression(id1, NULL, IL_OR, IL_PUSH);
    ass = mk_assignment(id2, exp, LD_COIL);
    
    result = gen_ass(ass, &ru);
    CU_ASSERT(result == PLC_OK);
    //LD A
    //ST B
    CU_ASSERT(ru.insno == 2);
    
    get(&ru, 1, &ins);
    
    CU_ASSERT(ins->operation == IL_ST);
    CU_ASSERT(ins->operand == OP_CONTACT);
    CU_ASSERT(ins->modifier == IL_NORM);
    CU_ASSERT(ins->byte == 0);
    CU_ASSERT(ins->bit == 1);
   
    //dump_rung(&ru, dump);
    //printf("%s\n", dump);
    
    clear_tree(ass);
    clear_rung(&ru);  
    
    //assign Q0/1 := I0/1 OR(I0/2)
    
    id2 = mk_identifier(OP_INPUT,  0, 2);
    id1 = mk_identifier(OP_INPUT,  0, 1);
    item_t q1 = mk_identifier(OP_INPUT,  0, 1);
    item_t exp1 = mk_expression(id1, NULL, IL_AND, IL_NORM);
    item_t exp2 = mk_expression(id2, NULL, IL_AND, IL_NORM);
    exp = mk_expression(exp1, exp2, IL_OR, IL_PUSH);
    ass = mk_assignment(q1, exp, LD_DOWN);
    result = gen_ass(ass, &ru);
    
    memset(dump, 0, MAXBUF * MAXSTR);
    dump_rung(&ru, dump);
    //printf("%s\n", dump);
    
    const char * expected = "\
0.LD i0/1\n\
1.OR(i0/2\n\
2.)\n\
3.ST!i0/1\n";

    CU_ASSERT_STRING_EQUAL(dump, expected);    
}

#endif //_UT_CG_H_
