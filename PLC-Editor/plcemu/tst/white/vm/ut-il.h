#ifndef _UT_IL_H_ 
#define _UT_IL_H_

void ut_char()
{
    int result = read_char(NULL, -1);
    CU_ASSERT(result == (BYTE)PLC_ERR);
}

void ut_number()
{
    int result = extract_number(NULL, -1);
    CU_ASSERT(result == PLC_ERR);
    
    char * line = "lol lol";
    result = extract_number(line, 0);
    CU_ASSERT(result == PLC_ERR);
    
    line = "lol52a";
    result = extract_number(line, 100);
    CU_ASSERT(result == PLC_ERR);
    
    result = extract_number(line, 5);
    CU_ASSERT(result == PLC_ERR);
    
    result = extract_number(line, 4);
    CU_ASSERT(result == 2);
    
    result = extract_number(line, 3);
    CU_ASSERT(result == 52);
    
    result = extract_number(line, 0);
    CU_ASSERT(result == PLC_ERR);
}

void ut_comments()
{
    read_line_trunk_comments(NULL);
    char line[MAXSTR];
    memset(line, 0, MAXSTR);
    sprintf(line, "%s", "lol");
    read_line_trunk_comments(line);
    CU_ASSERT(strcmp("lol", line)==0);
    
    sprintf(line, "%s", "lol ; lala");
    read_line_trunk_comments(line);
    
    CU_ASSERT(strcmp("lol ", line)==0);
}

void ut_whitespace()
{
    char line[MAXSTR];
    memset(line, 0, MAXSTR);
    sprintf(line, "whitespace!!!   \t   \n\r ");
    trunk_whitespace(line);
    CU_ASSERT(strcmp(line, "whitespace!!!") == 0);
    
    sprintf(line, "   \n\t\r  whitespace!!!   \t   \n\r ");
    trunk_whitespace(line);
    CU_ASSERT(strcmp(line, "whitespace!!!") == 0);
}

void ut_label()
{
    trunk_label(NULL, NULL, NULL);
    
    char line[MAXSTR];
    char buf[MAXSTR];
    char result[MAXSTR];
    memset(line, 0, MAXSTR);
    memset(buf, 0, MAXSTR);
    memset(result, 0, MAXSTR);

    sprintf(line, "%s", "lol");
    trunk_label(line, buf, result);
    CU_ASSERT(strcmp("lol", buf)==0);
    CU_ASSERT(strcmp("", result)==0);
    
    sprintf(line, "%s", "lol:mama");
    trunk_label(line, buf, result);
    CU_ASSERT(strcmp("lol", result)==0);
    CU_ASSERT(strcmp("mama", buf)==0);
    
}

void ut_modifier()
{   
    char line[MAXSTR];
    memset(line, 0, MAXSTR);
    
    BYTE result = read_modifier(NULL, NULL);
    CU_ASSERT(result == (BYTE)PLC_ERR);
    
    char * target = "";
    sprintf(line, "%s", "lol");
    result = read_modifier(line, &target);
    CU_ASSERT(result == NOP);
    CU_ASSERT(target == NULL);
     
    sprintf(line, "%s", "lol(");
    result = read_modifier(line, &target);
    CU_ASSERT(result == IL_PUSH);
    CU_ASSERT(*target == '(');
    CU_ASSERT(target == line +3);
    
    sprintf(line, "%s", "lo?l");
    result = read_modifier(line, &target);
    CU_ASSERT(result == IL_COND);
    CU_ASSERT(*target == '?');
    
    sprintf(line, "%s", "l!ol");
    result = read_modifier(line, &target);
    CU_ASSERT(result == IL_NEG);
    CU_ASSERT(*target == '!');
  
    sprintf(line, "%s", "l ol");
    result = read_modifier(line, &target);
    CU_ASSERT(result == IL_NORM);
    CU_ASSERT(*target == ' ');
}

void ut_operator()
{   
    char line[MAXSTR];
    memset(line, 0, MAXSTR);
    
    BYTE result = read_operator(NULL, NULL);
    CU_ASSERT(result == (BYTE)PLC_ERR);
    
    sprintf(line, "%s", "LT");
    result = read_operator(line, NULL);
    CU_ASSERT(result == IL_LT);
    
    sprintf(line, "%s", "XOR lwieufql");
    result = read_operator(line, line + 3);
    CU_ASSERT(result == IL_XOR);
    
    sprintf(line, "%s", "lwieufql");
    result = read_operator(line, line + 3);
    CU_ASSERT(result == N_IL_INSN);
    
}


void ut_arguments()
{   
    int result = find_arguments(NULL, NULL, NULL, NULL); 
    CU_ASSERT(result == PLC_ERR);
    
    char line[MAXSTR];
    memset(line, 0, MAXSTR);
    
    BYTE operand = 0;
    BYTE byte = 0;
    BYTE bit = 0;
    
    sprintf(line, "%s", "aqhsgdf");
    result = find_arguments(line, &operand, &byte, &bit); 
    CU_ASSERT(result == ERR_BADCHAR);
    
    sprintf(line, "%s", "aqh\%9df");
    result = find_arguments(line, &operand, &byte, &bit); 
    CU_ASSERT(result == ERR_BADOPERAND);
    
    sprintf(line, "%s", "aqh\%zdf");
    result = find_arguments(line, &operand, &byte, &bit); 
    CU_ASSERT(result == ERR_BADCHAR);
    
    sprintf(line, "%s", "aqh\%mdf");
    result = find_arguments(line, &operand, &byte, &bit); 
    CU_ASSERT(result ==  ERR_BADINDEX);
    CU_ASSERT(operand == OP_MEMORY);
    
    sprintf(line, "%s", "aqh\%i25f");
    result = find_arguments(line, &operand, &byte, &bit); 
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(operand == OP_INPUT);
    CU_ASSERT(byte == 25);
    CU_ASSERT(bit == BYTESIZE);
    
    sprintf(line, "%s", "\%t25/f");
    result = find_arguments(line, &operand, &byte, &bit); 
    CU_ASSERT(result == ERR_BADINDEX);
    CU_ASSERT(operand == OP_TIMEOUT);
    CU_ASSERT(byte == 25);
    CU_ASSERT(bit == BYTESIZE);
    
    sprintf(line, "%s", "\%q25/9");
    result = find_arguments(line, &operand, &byte, &bit); 
    CU_ASSERT(result == ERR_BADINDEX);
    CU_ASSERT(operand == OP_OUTPUT);
    CU_ASSERT(byte == 25);
    CU_ASSERT(bit == BYTESIZE);
    
    sprintf(line, "%s", "\%b25/7");
    result = find_arguments(line, &operand, &byte, &bit); 
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(operand == OP_BLINKOUT);
    CU_ASSERT(byte == 25);
    CU_ASSERT(bit == 7);
    
    sprintf(line, "%s", "\%if0");
    result = find_arguments(line, &operand, &byte, &bit); 
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(operand == OP_REAL_INPUT);
    CU_ASSERT(byte == 0);
    
    sprintf(line, "%s", "\%mf1");
    result = find_arguments(line, &operand, &byte, &bit); 
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(operand == OP_REAL_MEMORY);
    CU_ASSERT(byte == 1);
    
    sprintf(line, "%s", "\%qf21");
    result = find_arguments(line, &operand, &byte, &bit); 
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(operand == OP_REAL_OUTPUT);
    CU_ASSERT(byte == 21);        
}

void ut_parse_real()
{   
    int result = PLC_OK;
   
    char line[MAXSTR];
    memset(line, 0, MAXSTR);
    
    instruction_t it;
    
    struct rung ru;
    memset(&ru, 0, sizeof(struct rung));
    
    char lines[MAXBUF][MAXSTR];
    memset(lines, 0, MAXBUF * MAXSTR); 
/*    
    
Knuth's online variance:
    n = 1
    mean = 5.0
    m2 = 0.0
    delta = 0.0
      
    read x :
        n += 1
        delta = x - mean
        mean += delta/n
        m2 += delta*(x - mean)

    if n < 2:
        return float('nan')
    else:
        return m2 / (n - 1)

    IL code:
    
    %M0 = 1; n
    %M1 = 5.0; mean
    %M2 = 0; m2
    %M3 = 0; delta
    %M4 = 1; const
    %I0 = 7.5; x: real input %I0

    %Q0 = 0; y: real outpput %Q0
    
    
    LD %mf0  ; n = n+1
    ADD %mf1  ;
    ST %mf0  ;
    
    LD %if0 ; read x
    SUB %mf1 ; 
    ST %mf3  ; delta = x - mean
    
    LD %mf1  ; Acc = mean
    ADD(%mf3 ; Acc = delta,              Stack = ADD mean
    DIV %mf0 ; Acc = delta/n,            Stack = ADD mean
    )       ; Acc = mean + ( delta/n )
    ST %mf1  ; mean = mean + ( delta/n )
    
    LD %mf2  ; Acc = m2
    ADD(%mf3 ; Acc = delta,              Stack = ADD m2
    MUL(%if0 ; Acc = x,                  Stack = MUL delta, ADD m2
    SUB %mf1 ; Acc = x - mean,           Stack = MUL delta, ADD m2
    )       ; Acc = delta * (x - mean), Stack = ADD m2
    )       ; Acc = m2 + (delta * (x - mean))
    ST %mf2  ; m2 = m2 + (delta * (x - mean))
    
    LD %mf2  ; Acc = m2
    DIV(%mf0 ; Acc = n,                  Stack = DIV m2
    SUB %mf4   ; Acc = n-1,                Stack = DIV m2
    )       ; Acc = m2 / n-1,           
    ST %Qf0  ; y = m2 / (n - 1)
*/

   sprintf(lines[0], "%s\n", "LD %mf0  ; n = n+1");
   result = parse_il_line(lines[0], &ru);
   get(&ru, 0, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_LD);
   CU_ASSERT(it->operand == OP_REAL_MEMORY);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 0);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[1], "%s\n", "ADD %mf1  ;");
   result = parse_il_line(lines[1], &ru);
   get(&ru, 1, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_ADD);
   CU_ASSERT(it->operand == OP_REAL_MEMORY);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 1);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[2], "%s\n", "ST %mf0  ;");
   result = parse_il_line(lines[2], &ru);
   get(&ru, 2, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_ST);
   CU_ASSERT(it->operand == OP_REAL_MEMIN);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 0);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[3], "%s\n", "LD %if0 ; read x");

   result = parse_il_line(lines[3], &ru);
   get(&ru, 3, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_LD);
   CU_ASSERT(it->operand == OP_REAL_INPUT);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 0);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[4], "%s\n", "SUB %mf1 ; ");

   result = parse_il_line(lines[4], &ru);
   get(&ru, 4, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_SUB);
   CU_ASSERT(it->operand == OP_REAL_MEMORY);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 1);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[5], "%s\n", "ST %mf3  ; delta = x - mean");

   result = parse_il_line(lines[5], &ru);
   get(&ru, 5, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_ST);
   CU_ASSERT(it->operand == OP_REAL_MEMIN);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 3);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[6], "%s\n", "LD %mf1  ; Acc = mean");
   result = parse_il_line(lines[6], &ru);
   get(&ru, 6, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_LD);
   CU_ASSERT(it->operand == OP_REAL_MEMORY);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 1);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[7], "%s\n", "ADD(%mf3 ; Acc = delta, Stack = ADD mean");
   result = parse_il_line(lines[7], &ru);
   get(&ru, 7, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_ADD);
   CU_ASSERT(it->operand == OP_REAL_MEMORY);
   CU_ASSERT(it->modifier == IL_PUSH);
   CU_ASSERT(it->byte == 3);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[8], "%s\n", "DIV %mf0 ; Acc = delta/n, Stack = ADD mean");
   result = parse_il_line(lines[8], &ru);
   get(&ru, 8, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_DIV);
   CU_ASSERT(it->operand == OP_REAL_MEMORY);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 0);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[9], "%s\n", ")       ; Acc = mean + ( delta/n )");

   result = parse_il_line(lines[9], &ru);
   get(&ru, 9, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_POP);
   
   sprintf(lines[10], "%s\n", "ST %mf1  ; mean = mean + ( delta/n )");

   result = parse_il_line(lines[10], &ru);
   get(&ru, 10, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_ST);
   CU_ASSERT(it->operand == OP_REAL_MEMIN);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 1);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[11], "%s\n", "LD %mf2  ; Acc = m2");

   result = parse_il_line(lines[11], &ru);
   get(&ru, 11, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_LD);
   CU_ASSERT(it->operand == OP_REAL_MEMORY);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 2);
   CU_ASSERT(it->bit == BYTESIZE);

   sprintf(lines[12], "%s\n", "ADD(%mf3 ; Acc = delta, Stack = ADD m2");
   result = parse_il_line(lines[12], &ru);
   get(&ru, 12, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_ADD);
   CU_ASSERT(it->operand == OP_REAL_MEMORY);
   CU_ASSERT(it->modifier == IL_PUSH);
   CU_ASSERT(it->byte == 3);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[13], "%s\n", "MUL(%if0 ;Acc = x,Stack=MUL delta,ADD m2");
   result = parse_il_line(lines[13], &ru);
   get(&ru, 13, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_MUL);
   CU_ASSERT(it->operand == OP_REAL_INPUT);
   CU_ASSERT(it->modifier == IL_PUSH);
   CU_ASSERT(it->byte == 0);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[14], "%s\n", "SUB %mf1 ;Acc=x-mean,Stack=MUL delta,ADD m2");
   result = parse_il_line(lines[14], &ru);
   get(&ru, 14, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_SUB);
   CU_ASSERT(it->operand == OP_REAL_MEMORY);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 1);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[15], "%s\n", "); Acc = delta * (x - mean), Stack = ADD m2");

   result = parse_il_line(lines[15], &ru);
   get(&ru, 15, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_POP);
   
   sprintf(lines[16], "%s\n", ") ; Acc = m2 + (delta * (x - mean))");

   result = parse_il_line(lines[16], &ru);
   get(&ru, 16, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_POP);
   
   sprintf(lines[17], "%s\n", "ST %mf2  ; m2 = m2 + (delta * (x - mean))");

   result = parse_il_line(lines[17], &ru);
   get(&ru, 17, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_ST);
   CU_ASSERT(it->operand == OP_REAL_MEMIN);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 2);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[18], "%s\n", "LD %mf2  ; Acc = m2");
   result = parse_il_line(lines[18], &ru);
   get(&ru, 18, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_LD);
   CU_ASSERT(it->operand == OP_REAL_MEMORY);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 2);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[19], "%s\n", "DIV(%mf0 ; Acc = n, Stack = DIV m2");
   result = parse_il_line(lines[19], &ru);
   get(&ru, 19, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_DIV);
   CU_ASSERT(it->operand == OP_REAL_MEMORY);
   CU_ASSERT(it->modifier == IL_PUSH);
   CU_ASSERT(it->byte == 0);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[20], "%s\n", "SUB %mf4   ; Acc = n-1, Stack = DIV m2");
   result = parse_il_line(lines[20], &ru);
   get(&ru, 20, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_SUB);
   CU_ASSERT(it->operand == OP_REAL_MEMORY);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 4);
   CU_ASSERT(it->bit == BYTESIZE);
   
   sprintf(lines[21], "%s\n", ")       ; Acc = m2 / n-1");

   result = parse_il_line(lines[21], &ru);
   get(&ru, 21, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_POP);
   
   sprintf(lines[22], "%s\n", "ST %Qf0  ; y = m2 / (n - 1)");

   result = parse_il_line(lines[22], &ru);
   get(&ru, 22, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_ST);
   CU_ASSERT(it->operand == OP_REAL_CONTACT);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 0);
   CU_ASSERT(it->bit == BYTESIZE);
    
   memset(line, 0, MAXSTR);

   clear_rung(&ru);
   struct PLC_regs p;
   init_mock_plc(&p);
   
   result = parse_il_program(lines, &p);
   
   CU_ASSERT(result == PLC_OK);
   
   char dump[MAXSTR * MAXBUF];
   memset(dump, 0, MAXBUF * MAXSTR);
   dump_rung(p.rungs[0], dump);
   //printf("%s", dump); 
   const char * expected = 
"0.LD mf0/8\n\
1.ADD mf1/8\n\
2.ST MF0/8\n\
3.LD if0/8\n\
4.SUB mf1/8\n\
5.ST MF3/8\n\
6.LD mf1/8\n\
7.ADD(mf3/8\n\
8.DIV mf0/8\n\
9.)\n\
10.ST MF1/8\n\
11.LD mf2/8\n\
12.ADD(mf3/8\n\
13.MUL(if0/8\n\
14.SUB mf1/8\n\
15.)\n\
16.)\n\
17.ST MF2/8\n\
18.LD mf2/8\n\
19.DIV(mf0/8\n\
20.SUB mf4/8\n\
21.)\n\
22.ST QF0/8\n";
    CU_ASSERT_STRING_EQUAL(dump, expected);
}

void ut_parse()
{   
    int result = parse_il_line(NULL, NULL);
    CU_ASSERT(result == PLC_ERR);
    
    char line[MAXSTR];
    memset(line, 0, MAXSTR);
    
    instruction_t it;
    
    struct rung ru;
    memset(&ru, 0, sizeof(struct rung));
    
    char lines[MAXBUF][MAXSTR];
    memset(lines, 0, MAXBUF * MAXSTR); 
    
/* euclidean Greatest Common divisor:
function gcd(a, b)
    while a ≠ b 
        if a > b
           a := a − b; 
        else
           b := b − a; 
    return a;
    
    IL code:
    
    while:LD %m0    ; A
    EQ %m1          ; A == B
    JMP?endwhile    ; while(A != B)
        LT %m1      ; A < B
        JMP?reverse
        SUB %m1     ; A - B
        ST %m0      ; A = A - B
    JMP while
    reverse:LD %m1  ; B
        SUB %m0     ; B - A
        ST %m1      ; B = B - A
    JMP while
    endwhile:LD %m0 ; 
    ST %q0 ; output gcd 
*/
   sprintf(lines[0], "%s\n", "while:LD %m0      ; A");
   result = parse_il_line(lines[0], &ru);
   get(&ru, 0, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_LD);
   CU_ASSERT(it->operand == OP_MEMORY);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 0);
   CU_ASSERT(it->bit == BYTESIZE);
   CU_ASSERT(strcmp(it->label, "while") == 0);
  
   sprintf(lines[1], "%s\n", "EQ %m1            ; A == B");
   result = parse_il_line(lines[1], &ru);
   get(&ru, 1, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_EQ);
   CU_ASSERT(it->operand == OP_MEMORY);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 1);
   CU_ASSERT(it->bit == BYTESIZE);
   CU_ASSERT(strcmp(it->label, "") == 0);
  
   //memset(line, 0, MAXSTR);
   
   sprintf(lines[2], "%s\n", "JMP?endwhile      ; while(A != B)");
   result = parse_il_line(lines[2], &ru);
   get(&ru, 2, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_JMP);
   CU_ASSERT(it->operand == 0);
   CU_ASSERT(it->modifier == IL_COND);
   CU_ASSERT(it->byte == 0);
   CU_ASSERT(it->bit == 0);
   CU_ASSERT(strcmp(it->label, "") == 0);
   CU_ASSERT(strcmp(it->lookup, "endwhile") == 0); 
   
   sprintf(lines[3], "%s\n", "LT %m1            ; A < B");
   result = parse_il_line(lines[3], &ru);
   get(&ru, 3, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_LT);
   CU_ASSERT(it->operand == OP_MEMORY);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 1);
   CU_ASSERT(it->bit == BYTESIZE);
   CU_ASSERT(strcmp(it->label, "") == 0);
   CU_ASSERT(strcmp(it->lookup, "") == 0); 
   
   sprintf(lines[4], "%s\n", "JMP?reverse");
   result = parse_il_line(lines[4], &ru);
   get(&ru, 4, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_JMP);
   CU_ASSERT(it->operand == 0);
   CU_ASSERT(it->modifier == IL_COND);
   CU_ASSERT(it->byte == 0);
   CU_ASSERT(it->bit == 0);
   CU_ASSERT(strcmp(it->label, "") == 0);
   CU_ASSERT(strcmp(it->lookup, "reverse") == 0); 
   
   sprintf(lines[5], "%s\n", "SUB %m1           ; A - B");
   result = parse_il_line(lines[5], &ru);
   get(&ru, 5, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_SUB);
   CU_ASSERT(it->operand == OP_MEMORY);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 1);
   CU_ASSERT(it->bit == BYTESIZE);
   CU_ASSERT(strcmp(it->label, "") == 0);
   CU_ASSERT(strcmp(it->lookup, "") == 0); 
   
   sprintf(lines[6], "%s\n", "ST %m0            ; A = A - B");
   result = parse_il_line(lines[6], &ru);
   get(&ru, 6, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_ST);
   CU_ASSERT(it->operand == OP_PULSEIN);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 0);
   CU_ASSERT(it->bit == BYTESIZE);
   CU_ASSERT(strcmp(it->label, "") == 0);
   CU_ASSERT(strcmp(it->lookup, "") == 0); 
   
   sprintf(lines[7], "%s\n", "JMP while");
   result = parse_il_line(lines[7], &ru);
   get(&ru, 7, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_JMP);
   CU_ASSERT(it->operand == 0);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 0);
   CU_ASSERT(it->bit == 0);
   CU_ASSERT(strcmp(it->label, "") == 0);
   CU_ASSERT(strcmp(it->lookup, "while") == 0); 
     
   sprintf(lines[8], "%s\n", "reverse:LD %m1  ; B");
   result = parse_il_line(lines[8], &ru);
   get(&ru, 8, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_LD);
   CU_ASSERT(it->operand == OP_MEMORY);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 1);
   CU_ASSERT(it->bit == BYTESIZE);
   CU_ASSERT(strcmp(it->label, "reverse") == 0);
   CU_ASSERT(strcmp(it->lookup, "") == 0); 
   
   sprintf(lines[9], "%s\n", "SUB %m0     ; B - A");
   result = parse_il_line(lines[9], &ru);
   get(&ru, 9, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_SUB);
   CU_ASSERT(it->operand == OP_MEMORY);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 0);
   CU_ASSERT(it->bit == BYTESIZE);
   CU_ASSERT(strcmp(it->label, "") == 0);
   CU_ASSERT(strcmp(it->lookup, "") == 0); 
   
   sprintf(lines[10], "%s\n", "ST %m1      ; B = B - A");
   result = parse_il_line(lines[10], &ru);
   get(&ru, 10, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_ST);
   CU_ASSERT(it->operand == OP_PULSEIN);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 1);
   CU_ASSERT(it->bit == BYTESIZE);
   CU_ASSERT(strcmp(it->label, "") == 0);
   CU_ASSERT(strcmp(it->lookup, "") == 0); 
   
   sprintf(lines[11], "%s\n", "JMP while");
   result = parse_il_line(lines[11], &ru);
   get(&ru, 11, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_JMP);
   CU_ASSERT(it->operand == 0);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 0);
   CU_ASSERT(it->bit == 0);
   CU_ASSERT(strcmp(it->label, "") == 0);
   CU_ASSERT(strcmp(it->lookup, "while") == 0); 
   
   sprintf(lines[12], "%s\n", "endwhile:LD %m0 ;");
   result = parse_il_line(lines[12], &ru);
   get(&ru, 12, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_LD);
   CU_ASSERT(it->operand == OP_MEMORY);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 0);
   CU_ASSERT(it->bit == BYTESIZE);
   CU_ASSERT(strcmp(it->label, "endwhile") == 0);
   CU_ASSERT(strcmp(it->lookup, "") == 0); 
   
   sprintf(lines[13], "%s\n", "ST %q0 ; output gcd");
   result = parse_il_line(lines[13], &ru);
   get(&ru, 13, &it);
   
   CU_ASSERT(result == PLC_OK);
   
   CU_ASSERT(it->operation == IL_ST);
   CU_ASSERT(it->operand == OP_CONTACT);
   CU_ASSERT(it->modifier == IL_NORM);
   CU_ASSERT(it->byte == 0);
   CU_ASSERT(it->bit == BYTESIZE);
   CU_ASSERT(strcmp(it->label, "") == 0);
   CU_ASSERT(strcmp(it->lookup, "") == 0); 
   
   memset(line, 0, MAXSTR);

   clear_rung(&ru);
   struct PLC_regs p;
   init_mock_plc(&p);
   
   result = parse_il_program(lines, &p);
   
   CU_ASSERT(result == PLC_OK);
   
   char dump[MAXSTR * MAXBUF];
   memset(dump, 0, MAXBUF * MAXSTR);
   dump_rung(p.rungs[0], dump);
   //printf("%s", dump); 
   
   const char * expected = 
"0.while:LD m0/8\n\
1.EQ m1/8\n\
2.JMP?12\n\
3.LT m1/8\n\
4.JMP?8\n\
5.SUB m1/8\n\
6.ST M0/8\n\
7.JMP 0\n\
8.reverse:LD m1/8\n\
9.SUB m0/8\n\
10.ST M1/8\n\
11.JMP 0\n\
12.endwhile:LD m0/8\n\
13.ST Q0/8\n"; 
   CU_ASSERT_STRING_EQUAL(dump, expected);
}
#endif //_UT_IL_H_
