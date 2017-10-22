/*
** Copyright (C) 2004, 2017 by Kevin L. Mitchell <klmitch@mit.edu>
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the Free
** Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
** MA 02111-1307, USA
*/
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "test-harness.h"

#include "dbprim.h"
#include "dbprim_int.h"

TEST_PROG(t_hashtab, "Test hash table functionality")

#define FLUSH_THRESHOLD	10

/* calculate the hamming weight... */
static unsigned long
hamming(unsigned long bits)
{
#define _hw(n, m, s)	(((n) & (m)) + (((n) >> (s)) & (m)))
  bits = _hw(bits, 0x55555555,  1);
  bits = _hw(bits, 0x33333333,  2);
  bits = _hw(bits, 0x0f0f0f0f,  4);
  bits = _hw(bits, 0x00ff00ff,  8);
  bits = _hw(bits, 0x0000ffff, 16);

  return bits;
}

/* simple hash function for testing... */
static hash_t
t_hash(hash_table_t *tab, db_key_t *key)
{
  hash_t hash = 0;
  unsigned char *kv = dk_key(key);
  int i;

  for (i = dk_len(key); i; i--) /* hash the string */
    hash += hash * 257 + kv[i];

  return hash; /* return the hash value */
}

/* simple hash comparison function for testing... */
static unsigned long
t_comp(hash_table_t *tab, db_key_t *key1, db_key_t *key2)
{
  if (dk_len(key1) < 0) /* first, fix up the lengths... */
    dk_len(key1) = strlen((char *)dk_key(key1));
  if (dk_len(key2) < 0)
    dk_len(key2) = strlen((char *)dk_key(key2));

  if (dk_len(key1) != dk_len(key2)) /* if lengths don't match... */
    return 1; /* then the entries can't possibly match */

  /* use memcmp to do the actual comparison */
  return memcmp(dk_key(key1), dk_key(key2), dk_len(key1));
}

static unsigned int rsize_flags = 0;

#define RSF_INHIBIT	0x0001
#define RSF_RESIZE	0x0002

static hash_t rsize_new = 0;
static hash_t rsize_err = 44;

/* simple resize callback function for testing */
static db_err_t
t_resize(hash_table_t *tab, hash_t new)
{
  rsize_new = new; /* save new table size... */
  rsize_flags |= RSF_RESIZE; /* mark that it's been resized */

  /* should we inhibit resize? */
  return rsize_flags & RSF_INHIBIT ? rsize_err : 0;
}

static db_key_t keys[] = {
  DB_KEY_INIT("one", 3),	   DB_KEY_INIT("two", 3),
  DB_KEY_INIT("three", 5),	   DB_KEY_INIT("four", 4),
  DB_KEY_INIT("five", 4),	   DB_KEY_INIT("six", 3),
  DB_KEY_INIT("seven", 5),	   DB_KEY_INIT("eight", 5),
  DB_KEY_INIT("nine", 4),	   DB_KEY_INIT("ten", 3),
  DB_KEY_INIT("eleven", 6),	   DB_KEY_INIT("twelve", 6),
  DB_KEY_INIT("thirteen", 8),	   DB_KEY_INIT("fourteen", 8),
  DB_KEY_INIT("fifteen", 7),	   DB_KEY_INIT("sixteen", 7),
  DB_KEY_INIT("seventeen", 9),	   DB_KEY_INIT("eighteen", 8),
  DB_KEY_INIT("nineteen", 8),	   DB_KEY_INIT("twenty", 6),
  DB_KEY_INIT("twenty-one", 10),   DB_KEY_INIT("twenty-two", 10),
  DB_KEY_INIT("twenty-three", 12), DB_KEY_INIT("twenty-four", 11),
  DB_KEY_INIT("twenty-five", 11),  DB_KEY_INIT("twenty-six", 10),
  DB_KEY_INIT("twenty-seven", 12), DB_KEY_INIT("twenty-eight", 12),
  DB_KEY_INIT("twenty-nine", 11),  DB_KEY_INIT("thirty", 6)
};

#define HASH_ENT_CNT	(sizeof(keys) / sizeof(db_key_t))

static struct moves_s {
  int elem;
  db_key_t key;
} moves[] = {
  {  3, DB_KEY_INIT("thirty-one", 10) },
  { 14, DB_KEY_INIT("thirty-two", 10) },
  {  6, DB_KEY_INIT("thirty-three", 12) },
  { 11, DB_KEY_INIT("thirty-four", 11) },
  {  1, DB_KEY_INIT("thirty-five", 11) },
  {  4, DB_KEY_INIT("thirty-six", 10) },
  { 27, DB_KEY_INIT("thirty-seven", 12) },
  { 24, DB_KEY_INIT("thirty-eight", 12) },
  { 18, DB_KEY_INIT("thirty-nine", 11) },
  { 15, DB_KEY_INIT("forty", 5) }
};

#define HASH_MOVE_CNT	(sizeof(moves) / sizeof(struct moves_s))

static int removes[] = { 5, 7, 24, 21, 13, 8, 22 };

#define HASH_REMOVE_CNT	(sizeof(removes) / sizeof(int))

static struct fnv1_s {
  hash_t fnv1;
  hash_t fnv1a;
  db_key_t key;
} hashes[] = {
  { 1444023680UL,  572056750UL, DB_KEY_INIT("forty-one", 9) },
  { 1828084722UL, 3950110388UL, DB_KEY_INIT("forty-two", 9) },
  { 2785089628UL, 1892710214UL, DB_KEY_INIT("forty-three", 11) },
  {  211736576UL,  494166078UL, DB_KEY_INIT("forty-four", 10) },
  {  195944716UL, 1993281510UL, DB_KEY_INIT("forty-five", 10) },
  { 1996993540UL,  545288830UL, DB_KEY_INIT("forty-six", 9) },
  {  866566787UL, 1643030155UL, DB_KEY_INIT("forty-seven", 11) },
  {  253309861UL, 1717483017UL, DB_KEY_INIT("forty-eight", 11) },
  { 2638188604UL, 2609787894UL, DB_KEY_INIT("forty-nine", 10) },
  {  614582933UL, 3180588513UL, DB_KEY_INIT("fifty", 5) },
};

#define HASH_CNT (sizeof(hashes) / sizeof(struct fnv1_s))

struct iter_s {
  int elem;
  unsigned long err;
  unsigned long visited;
};

#define VISIT_INIT	((1 << HASH_ENT_CNT) - 1)

static unsigned long
t_iter(hash_table_t *tab, hash_entry_t *ent, struct iter_s *iter)
{
  /* If this is the one we error on... */
  if (iter->elem == (intptr_t)he_value(ent))
    return iter->err; /* then error out, dude! */

  /* must be after the error return because the element we're looking at
   * might actually be the last element anyway, so if we turned off the
   * bit, iter->visited would be zero, and the test would unexpectedly
   * fail.
   */
  iter->visited &= ~(1 << (intptr_t)he_value(ent));

  return 0;
}

int
main(int argc, char **argv)
{
  unsigned long err;
  hash_table_t tab; /* a hash table to play with... */
  hash_entry_t ents[HASH_ENT_CNT]; /* some entries to operate on... */
  hash_entry_t *he = 0;
  int i, tmp;
  unsigned long visited = VISIT_INIT, visit_init = VISIT_INIT;
  hash_t oldsize = 0, newsize = 0;
  hash_t hash;
  struct iter_s iter_tmp = { 0, 88, 0 };
  int resize_successful = 0;

  srand(time(0)); /* seed random number generator */

  /* OK, let's test ht_init first... */
  TEST(t_hashtab, ht_init, "Test that ht_init() may be called",
       (!(err = ht_init(&tab, 0, t_hash, t_comp, t_resize, 0, 6)) &&
	ht_modulus(&tab) == 7), FATAL(0),
       ("ht_init() call successful"),
       ("ht_init() call failed with error %lu modulus %" PRIu32, err,
	ht_modulus(&tab)));

  /* Now let's try some he_init()s... */
  TEST_DECL(t_hashtab, he_init, "Test that he_init() may be called")
  for (i = 0; i < HASH_ENT_CNT; i++)
    if ((err = he_init(&ents[i], (void *)((intptr_t)i))))
      FAIL(TEST_NAME(he_init), FATAL(0), "he_init() failed with error %lu",
	   err);
  PASS(TEST_NAME(he_init), "he_init() calls successful");

  /* All right, let's add the entries to the hash table. */
  TEST_DECL(t_hashtab, ht_add, "Test that ht_add() adds entries to hash table")
  for (i = 0; i < HASH_ENT_CNT; i++)
    if ((err = ht_add(&tab, &ents[i], &keys[i])))
      FAIL(TEST_NAME(ht_add), FATAL(0), "ht_add() failed with error %lu", err);
  PASS(TEST_NAME(ht_add), "ht_add() calls successful");

  /* Now let's try a few ht_find() trials... */
  TEST_DECL(t_hashtab, ht_find, "Test that ht_find() finds entries in hash "
	    "table")
  for (visited = VISIT_INIT; visited; visited &= ~(1 << tmp)) {
    tmp = rand() % HASH_ENT_CNT; /* select an entry at random */
    fprintf(stderr, "Looking up entry %d . . .\n", tmp);
    if ((err = ht_find(&tab, &he, &keys[tmp])))
      FAIL(TEST_NAME(ht_find), FATAL(0), "ht_find() failed with error %lu",
	   err);
    else if (he != &ents[tmp])
      FAIL(TEST_NAME(ht_find), FATAL(0), "ht_find() found wrong entry; "
	   "expected %d returned %ld", tmp, he ? -1 : he - ents);
  }
  PASS(TEST_NAME(ht_find), "ht_find() calls successful");

  /* OK, let's do some ht_move() tests now... */
  TEST_DECL(t_hashtab, ht_move, "Test that ht_move() moves entries around")
  for (i = 0; i < HASH_MOVE_CNT; i++)
    if ((err = ht_move(&tab, &ents[moves[i].elem], &moves[i].key)))
      FAIL(TEST_NAME(ht_move), FATAL(0), "ht_move() failed with error %lu",
	   err);
  PASS(TEST_NAME(ht_move), "ht_move() calls successful");

  /* Now move on to ht_remove()... */
  TEST_DECL(t_hashtab, ht_remove, "Test that ht_remove() removes entries")
  for (i = 0; i < HASH_REMOVE_CNT; i++) {
    if ((err = ht_remove(&tab, &ents[removes[i]])))
      FAIL(TEST_NAME(ht_remove), FATAL(0), "ht_remove() failed with error %lu",
	   err);
    visit_init &= ~(1 << removes[i]); /* keep track of what's removed */
  }
  PASS(TEST_NAME(ht_remove), "ht_remove() calls successful");

  /* OK, let's try some ht_iter() tests */
  TEST_DECL(t_hashtab, ht_iter, "Test that ht_iter() iterates properly")
  for (visited = VISIT_INIT; visited; visited &= ~(1 << iter_tmp.elem)) {
    iter_tmp.elem = rand() % HASH_ENT_CNT;
    iter_tmp.visited = visit_init;

    fprintf(stderr, "Iterating, ending on element %d (visited 0x%08lx; "
	    "element mask 0x%08lx)%s\n", iter_tmp.elem, iter_tmp.visited,
	    (0x01lu << iter_tmp.elem),
	    !(visit_init & (1 << iter_tmp.elem)) ? " [missing]" : "");

    err = ht_iter(&tab, (hash_iter_t)t_iter, (void *)&iter_tmp);

    fprintf(stderr, "Visited elements 0x%08lx; returned error %lu\n",
	    iter_tmp.visited, err);

    /* check error return... */
    if (err != ((visit_init & (1 << iter_tmp.elem)) ? iter_tmp.err : 0)) {
      FAIL(TEST_NAME(ht_iter), 0, "ht_iter() returned unexpected error "
	   "code %lu", err);
      goto iter_fail; /* yeah, yeah, I know; this is just the easiest way */
		      /* to get out of this loop and avoid calling PASS	  */
		      /* that I can think of...				  */
    }

    /* Now check visited elements... */
    if ((err ? (iter_tmp.visited == 0) : (iter_tmp.visited != 0))) {
      FAIL(TEST_NAME(ht_iter), 0, "ht_iter() failed to visit expected items; "
	   "visited 0x%08lx %c= 0", iter_tmp.visited, err ? '!' : '=');
      goto iter_fail; /* yeah, yeah, I know; this is just the easiest way */
		      /* to get out of this loop and avoid calling PASS	  */
		      /* that I can think of...				  */
    }
  }
  PASS(TEST_NAME(ht_iter), "ht_iter() calls successful");
 iter_fail:

  /* Now let's check ht_resize()... */
  TEST(t_hashtab, ht_resize, "Test that ht_resize() may be called",
       (!(err = ht_resize(&tab, 20)) && ht_modulus(&tab) == 23), 0,
       ("ht_resize() call successful"),
       ("ht_resize() call failed with error %lu modulus %" PRIu32, err,
	ht_modulus(&tab)));

  /* remember if ht_resize worked... */
  if (!err && ht_modulus(&tab) == 23)
    resize_successful = 1;

  /* OK, let's try some flushing... */
  TEST_DECL(t_hashtab, ht_flush, "Test that ht_flush() flushes the table")
  for (visited = visit_init; visited; ) {
    iter_tmp.elem = rand() % HASH_ENT_CNT; /* select an element */

    /* are we ready to actually clean the whole thing? */
    if (!(visited & (1 << iter_tmp.elem)) &&
	hamming(visited) >= FLUSH_THRESHOLD)
      continue; /* select another element */

    iter_tmp.visited = visited; /* figure out what we visited... */

    fprintf(stderr, "Flushing, ending on element %d (visited 0x%08lx; "
	    "element mask 0x%08lx; hamming %lu)%s\n", iter_tmp.elem,
	    iter_tmp.visited, (0x01lu << iter_tmp.elem),
	    hamming(iter_tmp.visited),
	    !(visited & (1 << iter_tmp.elem)) ? " [missing]" : "");

    err = ht_flush(&tab, (hash_iter_t)t_iter, (void *)&iter_tmp);

    fprintf(stderr, "Visited elements 0x%08lx; returned error %lu\n",
	    iter_tmp.visited, err);

    /* check error return... */
    if (err != ((visited & (1 << iter_tmp.elem)) ? iter_tmp.err : 0))
      FAIL(TEST_NAME(ht_flush), FATAL(0), "ht_flush() returned unexpected "
	   "error code %lu", err);
    if (err ? (iter_tmp.visited == 0) : (iter_tmp.visited != 0))
      FAIL(TEST_NAME(ht_flush), FATAL(0), "ht_flush() failed to visit "
	   "expected items; visited 0x%08lx %c= 0", iter_tmp.visited,
	   err ? '!' : '=');

    /* update what's been flushed out... */
    visited &= iter_tmp.visited & ~(1 << iter_tmp.elem);
  }
  if (ht_count(&tab) == 0)
    PASS(TEST_NAME(ht_flush), "ht_flush() properly flushed the table");
  else
    FAIL(TEST_NAME(ht_flush), FATAL(0), "ht_flush() seems to have left "
	 "items?");

  TEST(t_hashtab, ht_free, "Test that ht_free() may be called",
       (!(err = ht_free(&tab)) && !tab.ht_table), FATAL(0),
       ("ht_free() call successful"),
       ("ht_free() call failed with error %lu", err));

  if (!resize_successful) /* if resize didn't succeed... */
    return 0; /* well, there's no point testing AUTOGROW/AUTOSHRINK */

  /* allow automatic growing/shrinking */
  ht_flags(&tab) |= HASH_FLAG_AUTOGROW | HASH_FLAG_AUTOSHRINK;

  /* OK, do some tests with auto-growing... */
  TEST_DECL(t_hashtab, ht_add_autogrow, "Test that ht_add() automatically "
	    "grows the table")
  for (i = 0; i < HASH_ENT_CNT; i++) {
    oldsize = ht_modulus(&tab); /* save old size... */
    rsize_flags = RSF_INHIBIT; /* inhibit the growth... */

    fprintf(stderr, "Adding element %d\n", i);

    err = ht_add(&tab, &ents[i], &keys[i]); /* add the entry */

    /* Now, check for errors... */
    if (ht_modulus(&tab) != oldsize)
      FAIL(TEST_NAME(ht_add_autogrow), FATAL(0), "ht_add() grew the table "
	   "when callback prohibited it");
    if (err != ((rsize_flags & RSF_RESIZE) ? rsize_err : 0))
      FAIL(TEST_NAME(ht_add_autogrow), FATAL(0), "ht_add() failed with error "
	   "code %lu", err);

    if (rsize_flags & RSF_RESIZE) { /* ok, resize callback called... */
      fprintf(stderr, "Element %d: ht_add() autogrow to %" PRIu32 "\n",
	      i, rsize_new);
      if (rsize_new <= oldsize) /* size make sense? */
	FAIL(TEST_NAME(ht_add_autogrow), FATAL(0), "ht_add() wants to "
	     "shrink the table? (New size %" PRIu32 ", old size %" PRIu32 ")",
	     rsize_new, oldsize);

      rsize_flags = 0; /* turn off inhibition */
      newsize = rsize_new; /* save new size */

      if ((err = ht_add(&tab, &ents[i], &keys[i]))) /* try now... */
	FAIL(TEST_NAME(ht_add_autogrow), FATAL(0), "ht_add() failed with "
	     "error code %lu during autogrow", err);

      /* OK, check the size... */
      if (ht_modulus(&tab) != newsize)
	FAIL(TEST_NAME(ht_add_autogrow), FATAL(0), "ht_add() tried to resize "
	     "to a random value; expecting %" PRIu32 ", got %" PRIu32
	     " (original %" PRIu32 ", callback %" PRIu32 ")",
	     newsize, ht_modulus(&tab), oldsize, rsize_new);
    }
  }
  PASS(TEST_NAME(ht_add_autogrow), "ht_add() autogrows the table properly");

  /* Do some more ht_find() trials to make certain the table makes sense */
  TEST_DECL(t_hashtab, ht_find_autogrow, "Test that ht_find() can still "
	    "find entries for autogrow")
  for (visited = VISIT_INIT; visited; visited &= ~(1 << tmp)) {
    tmp = rand() % HASH_ENT_CNT; /* select an entry at random */
    fprintf(stderr, "Looking up entry %d . . .\n", tmp);
    if ((err = ht_find(&tab, &he, &keys[tmp])))
      FAIL(TEST_NAME(ht_find_autogrow), FATAL(0), "ht_find() failed with "
	   "error %lu", err);
    else if (he != &ents[tmp])
      FAIL(TEST_NAME(ht_find_autogrow), FATAL(0), "ht_find() found wrong "
	   "entry; returned %ld expected %d", he ? -1 : he - ents, tmp);
  }
  PASS(TEST_NAME(ht_find_autogrow), "ht_find() calls successful on "
       "automatically grown table");

  /* Now, do some ht_remove() trials with autoshrink enabled... */
  TEST_DECL(t_hashtab, ht_remove_autoshrink, "Test that ht_remove() "
	    "automatically shrinks the table")
  for (visit_init = VISIT_INIT; ht_count(&tab) > HASH_ENT_CNT / 2;
       visit_init &= ~(1 << tmp)) {
    tmp = rand() % HASH_ENT_CNT;

    if (!(visit_init & (1 << tmp))) /* element's already been removed */
      continue;

    oldsize = ht_modulus(&tab); /* save old size... */
    rsize_flags = RSF_INHIBIT; /* inhibit the growth... */

    fprintf(stderr, "Removing element %d\n", tmp);

    err = ht_remove(&tab, &ents[tmp]); /* remove the entry */

    /* Now, check for errors... */
    if (ht_modulus(&tab) != oldsize)
      FAIL(TEST_NAME(ht_remove_autoshrink), FATAL(0), "ht_remove() shrank the "
	   "table when callback prohibited it");
    if (err != ((rsize_flags & RSF_RESIZE) ? rsize_err : 0))
      FAIL(TEST_NAME(ht_remove_autoshrink), FATAL(0), "ht_remove() failed "
	   "with error code %lu", err);

    if (rsize_flags & RSF_RESIZE) { /* ok, resize callback called... */
      fprintf(stderr, "Element %d: ht_remove() autoshrink to %" PRIu32 "\n",
	      tmp, rsize_new);
      if (rsize_new >= oldsize) /* size make sense? */
	FAIL(TEST_NAME(ht_remove_autoshrink), FATAL(0), "ht_remove() wants to "
	     "grow the table? (New size %" PRIu32 ", old size %" PRIu32 ")",
	     rsize_new, oldsize);

      rsize_flags = 0; /* turn off inhibition */
      newsize = rsize_new; /* save new size */

      if ((err = ht_remove(&tab, &ents[tmp]))) /* try now... */
	FAIL(TEST_NAME(ht_remove_autogrow), FATAL(0), "ht_remove() failed "
	     "with error code %lu during autoshrink", err);

      /* OK, check the size... */
      if (ht_modulus(&tab) != newsize)
	FAIL(TEST_NAME(ht_remove_autoshrink), FATAL(0), "ht_remove() tried "
	     "to resize to a random value; expecting %" PRIu32 ", got %"
	     PRIu32 " (original %" PRIu32 ", callback %" PRIu32 ")",
	     newsize, ht_modulus(&tab), oldsize, rsize_new);
    }
  }
  PASS(TEST_NAME(ht_remove_autoshrink), "ht_remove() autoshrinks the table "
       "properly");

  /* And last, but certainly not least, do some more ht_find() trials */
  TEST_DECL(t_hashtab, ht_find_autoshrink, "Test that ht_find() can still "
	    "find entries for autoshrink")
  for (visited = visit_init; visited; visited &= ~(1 << tmp)) {
    tmp = rand() % HASH_ENT_CNT; /* select an entry at random */
    if (!(visit_init & (1 << tmp))) /* if it's been removed... */
      continue; /* well, skip it! */
    fprintf(stderr, "Looking up entry %d . . .\n", tmp);
    if ((err = ht_find(&tab, &he, &keys[tmp])))
      FAIL(TEST_NAME(ht_find_autoshrink), FATAL(0), "ht_find() failed with "
	   "error %lu", err);
    else if (he != &ents[tmp])
      FAIL(TEST_NAME(ht_find_autoshrink), FATAL(0), "ht_find() found wrong "
	   "entry; returned %ld expected %d", he ? -1 : he - ents, tmp);
  }
  PASS(TEST_NAME(ht_find_autoshrink), "ht_find() calls successful on "
       "automatically shrunk table");

  /* Oh, do some testing of the FNV hash routines */
  TEST_DECL(t_hashtab, hash_fnv1, "Test that hash_fnv1() computes the correct "
	    "hash")
  TEST_DECL(t_hashtab, hash_fnv1a, "Test that hash_fnv1a() computes the "
	    "correct hash")
  for (i = 0; i < HASH_CNT; i++) {
    if ((hash = hash_fnv1(0, &hashes[i].key)) != hashes[i].fnv1)
      FAIL(TEST_NAME(hash_fnv1), FATAL(0), "hash_fnv1() computed incorrect "
	   "hash for key \"%s\"; expected %" PRIu32 ", got %" PRIu32,
	   (char *)dk_key(&hashes[i].key), hashes[i].fnv1, hash);
    if ((hash = hash_fnv1a(0, &hashes[i].key)) != hashes[i].fnv1a)
      FAIL(TEST_NAME(hash_fnv1a), FATAL(0), "hash_fnv1a() computed incorrect "
	   "hash for key \"%s\"; expected %" PRIu32 ", got %" PRIu32,
	   (char *)dk_key(&hashes[i].key), hashes[i].fnv1a, hash);
  }
  PASS(TEST_NAME(hash_fnv1), "hash_fnv1() computes correct hash");
  PASS(TEST_NAME(hash_fnv1a), "hash_fnv1a() computes correct hash");

  return 0;
}
