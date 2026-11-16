#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uthash.h>
#define numthreads 3
typedef const char *word;

typedef struct {
  word text;
  size_t total;
  UT_hash_handle hh;
} tallyentry;

tallyentry *makeentry(word, size_t);

typedef tallyentry *tallytable;

typedef struct {
  tallytable *table;
  word *slice;
  size_t slicelen;
  pthread_mutex_t *mtx;
} workerargs;

workerargs *makeargs(tallytable *table, word *slice, size_t slicelen, pthread_mutex_t *mtx);

static void tallyslice(tallytable *table, word *slice, size_t slicelen, pthread_mutex_t *mtx) {

  for (size_t idx = 0; idx < slicelen; idx++) {
    word current = slice[idx];
    tallyentry *hit = NULL;

    if (mtx != NULL) {
      pthread_mutex_lock(mtx);
    }

    HASH_FIND_STR(*table, current, hit);

    if (hit == NULL) {
      hit = makeentry(current, 0);
      HASH_ADD_STR(*table, text, hit);
    }
    hit->total++;

    if (mtx != NULL) {
      pthread_mutex_unlock(mtx);
    }
  }
}

static void *workermain(void *);

static tallytable tallyparallel(word *input, size_t inputlen) {

  tallytable table = NULL;
  pthread_mutex_t tablemtx;

  pthread_t workers[numthreads];
  workerargs *arglist[numthreads];

  size_t baselen = inputlen / numthreads;
  size_t leftover = inputlen % numthreads;
  size_t offset = 0;

  pthread_mutex_init(&tablemtx, NULL);

  for (size_t t = 0; t < numthreads; t++) {
    size_t thislen = baselen;
    if (t == numthreads - 1) {
      thislen += leftover;
    }

    arglist[t] = makeargs(&table, input + offset, thislen, &tablemtx);
    pthread_create(&workers[t], NULL, workermain, arglist[t]);

    offset += thislen;
  }

  for (size_t t = 0; t < numthreads; t++) {
    pthread_join(workers[t], NULL);
    free(arglist[t]);
  }

  pthread_mutex_destroy(&tablemtx);

  return table;
}

static tallyentry *tallysequential(word *input, size_t inputlen) {
  tallyentry *table = NULL;

  tallyslice(&table, input, inputlen, NULL);

  return table;
}

int compareentries(tallyentry *lhs, tallyentry *rhs);

void showtable(tallytable);
void freetable(tallytable);

int main(void) {
  word input[13] = {"the",  "quick", "brown", "fox", "jumps", "over", "the",
                    "lazy", "dog",   "the",   "the", "fox",   "brown"};
  const size_t inputlen = 13;
  tallytable table = NULL;

  table = tallyparallel(input, inputlen);

  if (table != NULL) {
    HASH_SORT(table, compareentries);
    showtable(table);
    freetable(table);
  }

  return 0;
}

tallyentry *makeentry(word text, size_t total) {
  tallyentry *entry = malloc(sizeof(tallyentry));
  entry->text = text;
  entry->total = total;
  return entry;
}

int compareentries(tallyentry *lhs, tallyentry *rhs) { return strcmp(lhs->text, rhs->text); }

void showtable(tallytable table) {
  tallyentry *entry, *next;
  printf("%-32s%-10s\n", "Word", "Count");
  HASH_ITER(hh, table, entry, next) { printf("%-32s%-10zu\n", entry->text, entry->total); }
}

void freetable(tallytable table) {
  tallyentry *entry, *next;
  HASH_ITER(hh, table, entry, next) {
    HASH_DEL(table, entry);
    free(entry);
  }
}

workerargs *makeargs(tallytable *table, word *slice, size_t slicelen, pthread_mutex_t *mtx) {
  workerargs *packed = malloc(sizeof(workerargs));
  packed->table = table;
  packed->slice = slice;
  packed->slicelen = slicelen;
  packed->mtx = mtx;
  return packed;
}

static void *workermain(void *raw) {
  workerargs *packed = (workerargs *)raw;
  tallyslice(packed->table, packed->slice, packed->slicelen, packed->mtx);

  return NULL;
}
