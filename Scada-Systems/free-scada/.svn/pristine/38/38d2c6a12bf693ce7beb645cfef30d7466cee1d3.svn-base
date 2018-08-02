#include "tut.h"
#include "tut_reporter.h"
#include <stdio.h>
#include <iostream>

namespace tut 
{ 
	test_runner_singleton runner; 
}

int main(int argc, const char* argv[])
{ 
	tut::reporter rep;
	tut::runner.get().set_callback(&rep);
	tut::runner.get().run_tests();

	printf("Press a key to finish.");
	getchar();

	return 0; 
} 
