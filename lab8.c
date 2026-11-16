#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uthash.h>
#define NUM_THREADS 3
typedef const char *word_t;

// One row of the hash table: the word and how many times we saw it
typedef struct {
  word_t text;
  size_t total;
  UT_hash_handle hh;
} tally_entry_t;

tally_entry_t *make_entry(word_t, size_t);

typedef tally_entry_t *tally_table_t;

// What each worker thread gets handed
typedef struct {
  tally_table_t *table;
  word_t *slice;
  size_t slice_len;
  pthread_mutex_t *mtx;
} worker_args_t;

worker_args_t *make_args(tally_table_t *table, word_t *slice, size_t slice_len,
                         pthread_mutex_t *mtx);

// Counts every word in slice into the shared table.
// Pass NULL for mtx when only one thread touches the table.
static void tally_slice(tally_table_t *table, word_t *slice, size_t slice_len,
                        pthread_mutex_t *mtx) {

  for (size_t idx = 0; idx < slice_len; idx++) {
    word_t current = slice[idx];
    tally_entry_t *hit = NULL;

    if (mtx != NULL) {
      pthread_mutex_lock(mtx);
    }

    HASH_FIND_STR(*table, current, hit);

    if (hit == NULL) {
      // Not in the table yet, so add it at 0 and let the line below bump it
      hit = make_entry(current, 0);
      HASH_ADD_STR(*table, text, hit);
    }
    hit->total++;

    if (mtx != NULL) {
      pthread_mutex_unlock(mtx);
    }
  }
}

static void *worker_main(void *);

static tally_table_t tally_parallel(word_t *input, size_t input_len) {

  tally_table_t table = NULL;
  pthread_mutex_t table_mtx;

  pthread_t workers[NUM_THREADS];
  worker_args_t *worker_args[NUM_THREADS];

  size_t base_len = input_len / NUM_THREADS;
  size_t leftover = input_len % NUM_THREADS;
  size_t offset = 0;

  pthread_mutex_init(&table_mtx, NULL);

  // Start the workers. The last one also takes the words that didn't divide evenly
  for (size_t t = 0; t < NUM_THREADS; t++) {
    size_t this_len = base_len;
    if (t == NUM_THREADS - 1) {
      this_len += leftover;
    }

    worker_args[t] = make_args(&table, input + offset, this_len, &table_mtx);
    pthread_create(&workers[t], NULL, worker_main, worker_args[t]);

    offset += this_len;
  }

  // Wait on each worker, then free the args it was using
  for (size_t t = 0; t < NUM_THREADS; t++) {
    pthread_join(workers[t], NULL);
    free(worker_args[t]);
  }

  pthread_mutex_destroy(&table_mtx);

  return table;
}

// Takes in an array of words of size input_len and
// returns a hash table keyed on the word, where the
// value is how many times that word appears
static tally_entry_t *tally_sequential(word_t *input, size_t input_len) {
  tally_entry_t *table = NULL;

  // No threads here, so the whole array goes in as one slice with no lock
  tally_slice(&table, input, input_len, NULL);

  return table;
}

int compare_entries(tally_entry_t *lhs, tally_entry_t *rhs);

void show_table(tally_table_t);
void free_table(tally_table_t);

int main(void) {
  word_t input[13] = {"the",  "quick", "brown", "fox", "jumps", "over", "the",
                      "lazy", "dog",   "the",   "the", "fox",   "brown"};
  const size_t input_len = 13;
  tally_table_t table = NULL;

  // table = tally_sequential(input, input_len);
  table = tally_parallel(input, input_len);

  // Sort, print, then clean up (nothing to do if the table came back empty)
  if (table != NULL) {
    HASH_SORT(table, compare_entries);
    show_table(table);
    free_table(table);
  }

  return 0;
}

tally_entry_t *make_entry(word_t text, size_t total) {
  tally_entry_t *entry = malloc(sizeof(tally_entry_t));
  entry->text = text;
  entry->total = total;
  return entry;
}

int compare_entries(tally_entry_t *lhs, tally_entry_t *rhs) {
  return strcmp(lhs->text, rhs->text);
}

void show_table(tally_table_t table) {
  tally_entry_t *entry, *next;
  printf("%-32s%-10s\n", "Word", "Count");
  HASH_ITER(hh, table, entry, next) { printf("%-32s%-10zu\n", entry->text, entry->total); }
}

void free_table(tally_table_t table) {
  tally_entry_t *entry, *next;
  HASH_ITER(hh, table, entry, next) {
    HASH_DEL(table, entry);
    free(entry);
  }
}

worker_args_t *make_args(tally_table_t *table, word_t *slice, size_t slice_len,
                         pthread_mutex_t *mtx) {
  worker_args_t *packed = malloc(sizeof(worker_args_t));
  packed->table = table;
  packed->slice = slice;
  packed->slice_len = slice_len;
  packed->mtx = mtx;
  return packed;
}

static void *worker_main(void *raw) {
  // Unpack the args and count this thread's slice
  worker_args_t *packed = (worker_args_t *)raw;
  tally_slice(packed->table, packed->slice, packed->slice_len, packed->mtx);

  return NULL;
}
