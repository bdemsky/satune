#include "common.h"

void assert_hook(void)
{
	model_print("Add breakpoint to line %u in file %s.\n", __LINE__, __FILE__);
}
