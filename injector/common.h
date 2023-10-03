#ifndef COMMON_H
#define COMMON_H

// enable debugfs monitoring hooks of the module
// see debugfs docs here `
// https://www.kernel.org/doc/html/latest//filesystems/debugfs.html
#define DEBUG_FS 1

#if DEBUG_FS
#include <linux/debugfs.h>
extern struct dentry *debugfs_root;
#endif

/* Copied from arch/x86/mm/fault.c. */
/*
 * Page fault error code bits:
 *
 *   bit 0 ==	 0: no page found	1: protection fault
 *   bit 1 ==	 0: read access		1: write access
 *   bit 2 ==	 0: kernel-mode access	1: user-mode access
 *   bit 3 ==				1: use of reserved bit detected
 *   bit 4 ==				1: fault was an instruction fetch
 *   bit 5 ==				1: protection keys block access
 */
enum x86_pf_error_code {
	// clang-format off
	PF_PROT		=		1 << 0,
	PF_WRITE	=		1 << 1,
	PF_USER		=		1 << 2,
	PF_RSVD		=		1 << 3,
	PF_INSTR	=		1 << 4,
	PF_PK		=		1 << 5,
	// clang-format on
};

#define FILEPATH_LEN 256
extern const char *RECORD_FILE_FMT;

extern const unsigned long PAGE_ADDR_MASK;
extern const unsigned long PRESENT_BIT_MASK;
extern const unsigned long SPECIAL_BIT_MASK;

// N.B. does not take mmap_sem and the caller must take the semaphore
// if need be
pte_t *addr2pte(unsigned long addr, struct mm_struct *mm);

// does not return anything since we cannot take any action on fail
struct file *open_trace(const char *filepath);
void close_trace(struct file *f);
void write_buffered_trace_to_file(struct file *f, const char *buf, const char *buf_time, long buf_len, long buf_time_len);

#endif /*COMMON_H*/
