#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "CUnit/Basic.h"
#include "CUnit/Console.h"
#include "CUnit/Automated.h"

#include "plclib.h"
#include "plcemu.h"
#include "parser-tree.h"
#include "parser-il.h"
#include "parser-ld.h"
#include "codegen.h"
#include "init.h"

#include "ut-lib.h"
#include "ut-il.h"
#include "ut-ld.h"
#include "ut-tree.h"
#include "ut-cg.h"
#include "ut-init.h"

#define TRUE 1
#define FALSE 0

int init_suite_success (void)
{
  return 0;
}
int init_suite_failure (void)
{
  return -1;
}
int clean_suite_success (void)
{
  return 0;
}
int clean_suite_failure (void)
{
  return -1;
}

#define ADD_TEST(suite,name)\
  (NULL == CU_add_test((suite),#name,name))


int main ()
{
  CU_pSuite           suite_lib = NULL;
  CU_pSuite           suite_il = NULL;
  CU_pSuite           suite_ld = NULL;
  CU_pSuite           suite_tree = NULL;
  CU_pSuite           suite_codegen = NULL;
  CU_pSuite           suite_init = NULL;
  
  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry ())
    return CU_get_error ();

  /* add a suite to the registry */
  suite_lib = CU_add_suite ("plclib core vm library", 
                            init_suite_success, 
                            clean_suite_success);
                            
  suite_il = CU_add_suite ("instruction list lexer - generator", 
                            init_suite_success, 
                            clean_suite_success);
                            
  suite_ld = CU_add_suite ("ladder logic expression parser", 
                           init_suite_success, 
                           clean_suite_success);            
  
  suite_tree = CU_add_suite ("syntax tree", 
                           init_suite_success, 
                           clean_suite_success);            
  
  suite_codegen = CU_add_suite ("microcode generator", 
                           init_suite_success, 
                           clean_suite_success);      
  
  suite_init = CU_add_suite ("program initialization", 
                           init_suite_success,
                           clean_suite_success);
  
  if(NULL == suite_lib
  || NULL == suite_il
  || NULL == suite_ld
  || NULL == suite_tree
  || NULL == suite_codegen
  || NULL == suite_init
  ) 
  {
    CU_cleanup_registry ();
    return CU_get_error ();
  }

 // start_thread();
//plclib
  if(ADD_TEST(suite_lib, ut_codec)
  || ADD_TEST(suite_lib, ut_stack)
  || ADD_TEST(suite_lib, ut_type)   
  || ADD_TEST(suite_lib, ut_operate)
  || ADD_TEST(suite_lib, ut_operate_b)  
  || ADD_TEST(suite_lib, ut_operate_r)
  || ADD_TEST(suite_lib, ut_jmp) 
  || ADD_TEST(suite_lib, ut_rung) 
  || ADD_TEST(suite_lib, ut_set_reset) 
  || ADD_TEST(suite_lib, ut_st) 
  || ADD_TEST(suite_lib, ut_st_discrete) 
  || ADD_TEST(suite_lib, ut_st_real) 
  || ADD_TEST(suite_lib, ut_ld) 
  || ADD_TEST(suite_lib, ut_ld_discrete) 
  || ADD_TEST(suite_lib, ut_ld_real) 
  || ADD_TEST(suite_lib, ut_stackable) 
  || ADD_TEST(suite_lib, ut_instruct_bitwise)
  || ADD_TEST(suite_lib, ut_instruct_scalar) 
  || ADD_TEST(suite_lib, ut_instruct_real)
  || ADD_TEST(suite_lib, ut_task_bitwise)
  || ADD_TEST(suite_lib, ut_task_scalar)
  || ADD_TEST(suite_lib, ut_task_real)
  || ADD_TEST(suite_lib, ut_task_timeout)
    )
  {
	CU_cleanup_registry ();
        return CU_get_error ();
  }
//IL lexer-codegen
  if(ADD_TEST(suite_il, ut_number) 
  || ADD_TEST(suite_il, ut_char)
  || ADD_TEST(suite_il, ut_comments)
  || ADD_TEST(suite_il, ut_whitespace) 
  || ADD_TEST(suite_il, ut_label) 
  || ADD_TEST(suite_il, ut_modifier)  
  || ADD_TEST(suite_il, ut_operator)  
  || ADD_TEST(suite_il, ut_arguments)
  || ADD_TEST(suite_il, ut_parse)
  || ADD_TEST(suite_il, ut_parse_real)
     )
  {
	CU_cleanup_registry ();
        return CU_get_error ();
  }

//LD expression parser
  if(ADD_TEST(suite_ld, ut_minmin) 
  || ADD_TEST(suite_ld, ut_parse_ld_line)
  || ADD_TEST(suite_ld, ut_find_next_node)
  || ADD_TEST(suite_ld, ut_parse_ld_program)  
     )
  {
	CU_cleanup_registry ();
        return CU_get_error ();
  }

//syntax tree
  if(ADD_TEST(suite_tree, ut_mk_identifier) 
  || ADD_TEST(suite_tree, ut_mk_expression)
  || ADD_TEST(suite_tree, ut_mk_assignment)
     )
  {
	CU_cleanup_registry ();
        return CU_get_error ();
  }

//code generator
  if(ADD_TEST(suite_codegen, ut_gen_expr) 
  || ADD_TEST(suite_codegen, ut_gen_ass)
     )
  {
	CU_cleanup_registry ();
        return CU_get_error ();
  }
  
  //initialization
 if(ADD_TEST(suite_init, ut_extract) 
 || ADD_TEST(suite_init, ut_load)
     )
  {
	CU_cleanup_registry ();
        return CU_get_error ();
  }

  /* Run all tests using the basic interface */

  CU_basic_set_mode (CU_BRM_VERBOSE);
  CU_basic_run_tests ();
  printf ("\n");
  CU_basic_show_failures (CU_get_failure_list ());
  printf ("\n\n");

  /* Run all tests using the automated interface */
  /*CU_automated_enable_junit_xml(0);*/
  CU_automated_run_tests ();
  CU_list_tests_to_file ();

  /* Clean up registry and return */
  CU_cleanup_registry ();

  //stop_thread();

  return CU_get_error ();
}

