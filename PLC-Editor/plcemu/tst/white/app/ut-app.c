#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "CUnit/Basic.h"
#include "CUnit/Console.h"
#include "CUnit/Automated.h"

#include "plclib.h"
#include "plcemu.h"
#include "config.h"
#include "ut-conf.h"

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
  CU_pSuite           suite_conf = NULL;
  
  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry ())
    return CU_get_error ();

  /* add a suite to the registry */
  
  suite_conf = CU_add_suite ("vm configurator", 
                           init_suite_success,
                           clean_suite_success);
  
  if(NULL == suite_conf) 
  {
    CU_cleanup_registry ();
    return CU_get_error ();
  }

 // start_thread();
  
  //configurator
  if(ADD_TEST(suite_conf, ut_conf) 
  || ADD_TEST(suite_conf, ut_load)
  || ADD_TEST(suite_conf, ut_apply)
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

