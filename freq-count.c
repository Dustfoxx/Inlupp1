#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "hash_table.h"
#include "linked_list.h"
#include "common.h"
#include "iterator.h"

#define Delimiters "+-#@()[]{}.,:;!? \t\n\r"

static int cmpstringp(const void *p1, const void *p2)
{
  return strcmp(*(char *const *)p1, *(char *const *)p2);
}

void sort_keys(char *keys[], size_t no_keys)
{
  qsort(keys, no_keys, sizeof(char *), cmpstringp);
}

void process_word(char *word, ioopm_hash_table_t *ht)
{
  // FIXME: Rewrite to match your own interface, error-handling, etc.
  option_t freq = ioopm_hash_table_lookup(ht, (elem_t) {.pointer = word});

  if(Unsuccessful(freq))
    ioopm_hash_table_insert(ht, (elem_t) {.pointer = strdup(word)}, (elem_t) {.int_value = freq.value.int_value + 1});
  else
    ioopm_hash_table_insert(ht, (elem_t) {.pointer = word}, (elem_t) {.int_value = freq.value.int_value + 1});
}

void process_file(char *filename, ioopm_hash_table_t *ht)
{
  FILE *f = fopen(filename, "r");

  while (true)
  {
    char *buf = NULL;
    size_t len = 0;
    getline(&buf, &len, f);

    if (feof(f))
    {
      free(buf);
      break;
    }

    for (char *word = strtok(buf, Delimiters);
         word && *word;
         word = strtok(NULL, Delimiters))
    {
      process_word(word, ht);
    }

    free(buf);
  }

  fclose(f);
}

int string_sum_hash(elem_t e)
{
  char *str = e.pointer;
  int result = 0;
  do
    {
      result += *str;
    }
  while (*++str != '\0');
  return result;
}

bool string_eq(elem_t e1, elem_t e2)
{
  return (strcmp(e1.pointer, e2.pointer) == 0);
}

void destroy_word(elem_t key, elem_t value, void *extra)
{
    free(key.pointer);
}

void destroy_word_hash(ioopm_hash_table_t *ht)
{
    ioopm_hash_table_apply_to_all(ht, destroy_word, (void *) {NULL});
    ioopm_hash_table_destroy(ht);
}

int main(int argc, char *argv[])
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create((ioopm_hash_function) string_sum_hash, string_eq, NULL);

  if (argc > 1)
  {
    for (int i = 1; i < argc; ++i)
    {
      process_file(argv[i], ht);
    }

    ioopm_list_t *keys_list = ioopm_hash_table_keys(ht);
    ioopm_list_iterator_t *iter = ioopm_list_iterator(keys_list);

    int size = ioopm_hash_table_size(ht);
    char **keys = calloc(size, sizeof(char *));

    for(int i = 0; i < size; i++)
      {
        keys[i] = (char *) iter->current->value.pointer;
        ioopm_iterator_next(iter);
      }
    ioopm_iterator_destroy(iter);

    sort_keys(keys, size);

    for (int i = 0; i < size; ++i)
    {
      // FIXME: Update to match your own interface, error handling, etc.
      int freq = (ioopm_hash_table_lookup(ht, (elem_t) {.pointer = keys[i]})).value.int_value;
      printf("%s: %d\n", keys[i], freq);
    }
    free(keys);
    ioopm_linked_list_destroy(keys_list);
  }
  else
  {
    puts("Usage: freq-count file1 ... filen");
  }
  destroy_word_hash(ht);
}

