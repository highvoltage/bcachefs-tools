/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _BCACHEFS_FS_IO_H
#define _BCACHEFS_FS_IO_H

#ifndef NO_BCACHEFS_FS

#include "buckets.h"
#include "io_types.h"

#include <linux/uio.h>

int bch2_writepage(struct page *, struct writeback_control *);
int bch2_readpage(struct file *, struct page *);

int bch2_writepages(struct address_space *, struct writeback_control *);
int bch2_readpages(struct file *, struct address_space *,
		   struct list_head *, unsigned);

int bch2_write_begin(struct file *, struct address_space *, loff_t,
		     unsigned, unsigned, struct page **, void **);
int bch2_write_end(struct file *, struct address_space *, loff_t,
		   unsigned, unsigned, struct page *, void *);

ssize_t bch2_direct_IO(struct kiocb *, struct iov_iter *);

ssize_t bch2_write_iter(struct kiocb *, struct iov_iter *);

int bch2_fsync(struct file *, loff_t, loff_t, int);

int bch2_truncate(struct bch_inode_info *, struct iattr *);
long bch2_fallocate_dispatch(struct file *, int, loff_t, loff_t);

loff_t bch2_llseek(struct file *, loff_t, int);

vm_fault_t bch2_page_mkwrite(struct vm_fault *);
void bch2_invalidatepage(struct page *, unsigned int, unsigned int);
int bch2_releasepage(struct page *, gfp_t);
int bch2_migrate_page(struct address_space *, struct page *,
		      struct page *, enum migrate_mode);

void bch2_fs_fsio_exit(struct bch_fs *);
int bch2_fs_fsio_init(struct bch_fs *);
#else
static inline void bch2_fs_fsio_exit(struct bch_fs *c) {}
static inline int bch2_fs_fsio_init(struct bch_fs *c) { return 0; }
#endif

#endif /* _BCACHEFS_FS_IO_H */
