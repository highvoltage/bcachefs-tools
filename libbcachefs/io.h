/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _BCACHEFS_IO_H
#define _BCACHEFS_IO_H

#include "checksum.h"
#include "io_types.h"

#define to_wbio(_bio)			\
	container_of((_bio), struct bch_write_bio, bio)

#define to_rbio(_bio)			\
	container_of((_bio), struct bch_read_bio, bio)

void bch2_bio_free_pages_pool(struct bch_fs *, struct bio *);
void bch2_bio_alloc_pages_pool(struct bch_fs *, struct bio *, size_t);

void bch2_latency_acct(struct bch_dev *, u64, int);

void bch2_submit_wbio_replicas(struct bch_write_bio *, struct bch_fs *,
			       enum bch_data_type, const struct bkey_i *);

#define BLK_STS_REMOVED		((__force blk_status_t)128)

enum bch_write_flags {
	BCH_WRITE_ALLOC_NOWAIT		= (1 << 0),
	BCH_WRITE_CACHED		= (1 << 1),
	BCH_WRITE_FLUSH			= (1 << 2),
	BCH_WRITE_DATA_ENCODED		= (1 << 3),
	BCH_WRITE_PAGES_STABLE		= (1 << 4),
	BCH_WRITE_PAGES_OWNED		= (1 << 5),
	BCH_WRITE_ONLY_SPECIFIED_DEVS	= (1 << 6),
	BCH_WRITE_NOPUT_RESERVATION	= (1 << 7),

	/* Internal: */
	BCH_WRITE_JOURNAL_SEQ_PTR	= (1 << 8),
};

static inline u64 *op_journal_seq(struct bch_write_op *op)
{
	return (op->flags & BCH_WRITE_JOURNAL_SEQ_PTR)
		? op->journal_seq_p : &op->journal_seq;
}

static inline void op_journal_seq_set(struct bch_write_op *op, u64 *journal_seq)
{
	op->journal_seq_p = journal_seq;
	op->flags |= BCH_WRITE_JOURNAL_SEQ_PTR;
}

static inline struct workqueue_struct *index_update_wq(struct bch_write_op *op)
{
	return op->alloc_reserve == RESERVE_MOVINGGC
		? op->c->copygc_wq
		: op->c->wq;
}

int bch2_write_index_default(struct bch_write_op *);

static inline void bch2_write_op_init(struct bch_write_op *op, struct bch_fs *c,
				      struct bch_io_opts opts)
{
	op->c			= c;
	op->io_wq		= index_update_wq(op);
	op->flags		= 0;
	op->written		= 0;
	op->error		= 0;
	op->csum_type		= bch2_data_checksum_type(c, opts.data_checksum);
	op->compression_type	= bch2_compression_opt_to_type[opts.compression];
	op->nr_replicas		= 0;
	op->nr_replicas_required = c->opts.data_replicas_required;
	op->alloc_reserve	= RESERVE_NONE;
	op->open_buckets.nr	= 0;
	op->devs_have.nr	= 0;
	op->target		= 0;
	op->opts		= opts;
	op->pos			= POS_MAX;
	op->version		= ZERO_VERSION;
	op->write_point		= (struct write_point_specifier) { 0 };
	op->res			= (struct disk_reservation) { 0 };
	op->journal_seq		= 0;
	op->index_update_fn	= bch2_write_index_default;
}

void bch2_write(struct closure *);

static inline struct bch_write_bio *wbio_init(struct bio *bio)
{
	struct bch_write_bio *wbio = to_wbio(bio);

	memset(wbio, 0, offsetof(struct bch_write_bio, bio));
	return wbio;
}

struct bch_devs_mask;
struct cache_promote_op;
struct extent_ptr_decoded;

int __bch2_read_extent(struct bch_fs *, struct bch_read_bio *, struct bvec_iter,
		       struct bkey_s_c, struct bch_io_failures *, unsigned);
void bch2_read(struct bch_fs *, struct bch_read_bio *, u64);

enum bch_read_flags {
	BCH_READ_RETRY_IF_STALE		= 1 << 0,
	BCH_READ_MAY_PROMOTE		= 1 << 1,
	BCH_READ_USER_MAPPED		= 1 << 2,
	BCH_READ_NODECODE		= 1 << 3,
	BCH_READ_LAST_FRAGMENT		= 1 << 4,

	/* internal: */
	BCH_READ_MUST_BOUNCE		= 1 << 5,
	BCH_READ_MUST_CLONE		= 1 << 6,
	BCH_READ_IN_RETRY		= 1 << 7,
};

static inline void bch2_read_extent(struct bch_fs *c,
				    struct bch_read_bio *rbio,
				    struct bkey_s_c k,
				    unsigned flags)
{
	__bch2_read_extent(c, rbio, rbio->bio.bi_iter, k, NULL, flags);
}

static inline struct bch_read_bio *rbio_init(struct bio *bio,
					     struct bch_io_opts opts)
{
	struct bch_read_bio *rbio = to_rbio(bio);

	rbio->_state	= 0;
	rbio->promote	= NULL;
	rbio->opts	= opts;
	return rbio;
}

void bch2_fs_io_exit(struct bch_fs *);
int bch2_fs_io_init(struct bch_fs *);

#endif /* _BCACHEFS_IO_H */
