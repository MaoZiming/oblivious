// #include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/mm_types.h>

#include <linux/fs.h> // Needed by filp
#include <asm/uaccess.h> // Needed by segment descriptors

#include "common.h"

// /data/traces/[trace_id].bin.[thread_ind]
const char *RECORD_FILE_FMT = "/data/traces/%s/main.bin.%d";

const unsigned long PAGE_ADDR_MASK = ~0xfff;
const unsigned long PRESENT_BIT_MASK = 1UL;
const unsigned long SPECIAL_BIT_MASK = 1UL << 58;

#ifdef DEBUG_FS
struct dentry *debugfs_root;
#endif

pte_t *addr2pte(unsigned long addr, struct mm_struct *mm)
{
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte = NULL;

	// walk the page table ` https://lwn.net/Articles/106177/
	//todo:: pteditor does it wrong i think,
	//it does not dereference pte when passing around
	pgd = pgd_offset(mm, addr);
	if (unlikely(pgd_none(*pgd) || pgd_bad(*pgd)))
		return pte;
	pud = pud_offset(pgd, addr);
	if (unlikely(pud_none(*pud) || pud_bad(*pud)))
		return pte;
	pmd = pmd_offset(pud, addr);

	// todo::investigate
	// currently, in trace recording pmd is none when
	// the page table is in process of being grown
	// this is because we intercept kernel page fault *before*
	// it grows page tables and is ready to assign ptes.
	// this, I think, means that we miss memory accesses which are
	// alligned to page table boundaries and since we do not
	// add special bit to these addresses in the very beginning,
	// we never add them to the trace record

	// todo:: to support thp, do some error checking here and see if a huge page is being allocated
	if (unlikely(pmd_none(*pmd) || pud_large(*pud)))
		return pte;

	//todo:: pte_offset_map_lock<-- what is this? when whould I need to take a lock?
	pte = pte_offset_map(pmd, addr);

	return pte;
}

struct file *open_trace(const char *filepath)
{
	struct file *f;

	mm_segment_t old_fs = get_fs();
	set_fs(get_ds()); // KERNEL_DS

	// todo::mkdir before open
	f = filp_open(filepath, O_CREAT | O_WRONLY | O_LARGEFILE, 0644);
	if (IS_ERR(f)) {
		printk(KERN_ERR
		       "unable to create/open file %s. ERR code: %pe\n",
		       filepath, f);
		f = NULL;
	}

	set_fs(old_fs);
	return f;
}

void close_trace(struct file *f)
{
	mm_segment_t old_fs = get_fs();
	set_fs(get_ds()); // KERNEL_DS

	filp_close(f, NULL);

	set_fs(old_fs);
}

void write_buffered_trace_to_file(struct file *f, const char *buf, const char *buf_time, long buf_len, long buf_time_len)
{
	long buf_left_to_write = buf_len;
	long buf_time_left_to_write = buf_time_len;

	mm_segment_t old_fs = get_fs();
	set_fs(get_ds()); // KERNEL_DS

	printk(KERN_DEBUG "Writing recorded trace (num accesses=%ld)",
	       buf_len / sizeof(void *));

	while (buf_left_to_write > 0 && buf_time_left_to_write) {
		// cannot write more than 2g at a time from kernel
		// fixed in newer kernels, I guess just upgrade?
		ssize_t count = kernel_write(f, buf, buf_left_to_write, f->f_pos);

		//size_t can not be smaller than zero
		if (count < 0) {
			printk(KERN_ERR "Failed writing. "
					"errno=%ld, left to "
					"write %ld\n",
			       count, buf_left_to_write);
			break;
		}

		f->f_pos += count;
		buf_left_to_write -= count;
		buf += count;

		// fixed in newer kernels, I guess just upgrade?
		count = kernel_write(f, buf_time, buf_time_left_to_write, f->f_pos);

		//size_t can not be smaller than zero
		if (count < 0) {
			printk(KERN_ERR "Failed writing. "
					"errno=%ld, left to "
					"write %ld\n",
			       count, buf_time_left_to_write);
			break;
		}

		f->f_pos += count;
		buf_time_left_to_write -= count;
		buf_time += count;
	}

	if (buf_left_to_write > 0 || buf_time_left_to_write > 0) {
		printk(KERN_ERR "buf: unable to write %ld "
				"out of %ld total\n",
		       buf_left_to_write, buf_len);
		printk(KERN_ERR "buf_time: unable to write %ld "
				"out of %ld total\n",
		       buf_time_left_to_write, buf_time_len);
	}
	set_fs(old_fs);
}
