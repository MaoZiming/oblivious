#include <linux/kernel.h>
#include <linux/export.h>
#include "injections.h"

void kernel_noop(void)
{
}
injected_func_type pointers[100] = {[0 ... 99] = kernel_noop };
struct task_struct_oblivious pid_to_obl[65535];

// void set_pointer(int i, injected_func_type f)
// {
// 	pointers[i] = f;
// }

mem_pattern_trace_state memtrace_state = {
	.flags = (TAPE_OPS | SWAP_SSD_OPTIMIZATION | FASTSWAP_ASYNCWRITES |
		  TAPE_FETCH)
};

