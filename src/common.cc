#include "common.h"
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void assert_hook(void)
{
	model_print("Add breakpoint to line %u in file %s.\n", __LINE__, __FILE__);
}

void print_trace(void){
	void *array[10];
	size_t size;
	size = backtrace(array, 10);
	backtrace_symbols_fd(array, size, STDERR_FILENO);	
}
