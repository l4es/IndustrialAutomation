#ifndef _UT_INIT_H_ 
#define _UT_INIT_H_

struct PLC_regs Plc;

void ut_extract() 
{
    char * line = "I 1 lol";
    char name[SMALLSTR];
    int index = 0;
    int r = extract_name(NULL, NULL, -1 );
    CU_ASSERT(r == PLC_ERR);
    
    r = extract_name(line, NULL, -1 );
    CU_ASSERT(r == PLC_ERR);
    
    r = extract_name(line, name, 0);
    CU_ASSERT(r == 1);
    CU_ASSERT_STRING_EQUAL(name, "I");
    
    //
    
    line = "  \t\n   I_MAX 1 lol";
    r = extract_name(line, name, 0);  
    //ignore trailing spaces
    CU_ASSERT(r == 12);
    CU_ASSERT_STRING_EQUAL(name, "I_MAX");
    
    //printf("found %s up to %d\n", name, r);
    
    r = extract_index(NULL, NULL, -1 );
    CU_ASSERT(r == PLC_ERR);
    
    r = extract_index(line, NULL, -1 );
    CU_ASSERT(r == PLC_ERR);
    
    line = "I 5 lol";
    r = extract_index(line, &index, 1);
    CU_ASSERT(r == 3);
    CU_ASSERT(index == 5);
    //
    line = "I\t\t \n\r57 lol";
    r = extract_index(line, &index, 1);
    CU_ASSERT(r == 8);
    CU_ASSERT(index == 57);
    //printf("found %d up to %d\n", index, r);
    
    r = extract_value(NULL, NULL, -1 );
    CU_ASSERT(r == PLC_ERR);
    
    r = extract_value(line, NULL, -1 );
    CU_ASSERT(r == PLC_ERR);
    
    line = "I 5 lol";
    r = extract_value(line, name, 3 );
    CU_ASSERT(r == 7);
    CU_ASSERT_STRING_EQUAL(name, "lol");
    
    line = "I 5 lol\n\r\n\n\n";
    r = extract_value(line, name, 3 );
    CU_ASSERT(r == 7);
    CU_ASSERT_STRING_EQUAL(name, "lol");
    //printf("found %s up to %d\n", name, r);
   
}



void ut_load() 
{
}

#endif //_UT_INIT_H_

