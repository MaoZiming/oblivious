#ifndef INJECTIONS_H
#define INJECTIONS_H

typedef void (*injected_func_type)(void);
extern injected_func_type pointers[100];
extern void kernel_noop(void);
// extern void set_pointer(int i, void (*f)());

enum memtrace_state_lags {
	TAPE_OPS = 1 << 0,
	SWAP_SSD_OPTIMIZATION = 1 << 1,
	FASTSWAP_ASYNCWRITES = 1 << 2,

	TAPE_FETCH = 1 << 3,
	LRU_LOGS = 1 << 4,

	OFFLOAD_FETCH = 1 << 5,
	MARK_UNEVICTABLE = 1 << 6,

	ONE_TAPE = 1 << 7,
};

// space for mem_pattern_trace state which needs to live accross injector
// module reloads.
typedef struct {
	int flags;
	// in case I need to add more fields before doing a kernel recompile
	char padding[60];
} mem_pattern_trace_state;

extern mem_pattern_trace_state memtrace_state;

static inline void memtrace_setflag(int flag)
{
	memtrace_state.flags |= flag;
}

static inline int memtrace_getflag(int flag)
{
	return memtrace_state.flags & flag;
}

static inline void memtrace_clearflag(int flag)
{
	memtrace_state.flags &= ~flag;
}

#define OBL_MAX_NUM_THREADS 20

struct trace_recording_state {
	unsigned long *accesses;
	unsigned long long *timestamps;
	unsigned long pos;
	struct file *f;

	unsigned long microset_size;
	unsigned long microset_pos;
	unsigned long *microset;
	unsigned long long *microset_times;
};

struct task_struct_oblivious {
	// thread index
	int tind;
	int flags;
	struct trace_recording_state record;
};

extern struct task_struct_oblivious pid_to_obl[65535];

#endif /* INJECTIONS_H */
