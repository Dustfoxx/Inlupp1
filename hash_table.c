#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"
//#include "CUnit/Basic.h"


ioopm_hash_table_t *ioopm_hash_table_create()
{
    /// Allocate space for a ioopm_hash_table_t = 17 pointers to
    /// entry_t's, which will be set to NULL
    ioopm_hash_table_t *result = calloc(1, sizeof(ioopm_hash_table_t));
    return result;
}

void recurse_destroy(entry_t *chosen)
{
    if(chosen->next)
        recurse_destroy(chosen->next);
    
    free(chosen);
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
    for(int i = 0; i < 17; i++)
    {
      entry_t *ptr = ht->buckets[i].next;
      if(ptr)
        recurse_destroy(ptr);
    }
    free(ht);
}

char *ioopm_hash_table_lookup(ioopm_hash_table_t *ht, int key)
{
    return NULL;
}

entry_t *find_previous_entry_for_key(entry_t *bucket, int key)
{
  entry_t *cursor = bucket;
  entry_t *old_cursor = NULL;
  while (cursor != NULL)
    {
      if (cursor->key == key)
        {
          return old_cursor; /// Ends the whole function!
        }
      old_cursor = cursor;
      cursor = cursor->next; /// Step forward to the next entry, and repeat loop
    }
    return bucket;
}

entry_t *entry_create(int key, char *value, entry_t *entry)
{
    /// Create an object for the new entry
  entry_t *new_entry = calloc(1, sizeof(entry_t));
  /// Set the key and value fields to the key and value
  new_entry->key = key;
  new_entry->value = value;
  /// Make the first entry the next entry of the new entry
  new_entry->next = entry;

  return new_entry;
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, int key, char *value)
{
  /// Calculate the bucket for this entry
  int bucket = key % 17;
  /// Search for an existing entry for a key
  entry_t *entry = find_previous_entry_for_key(&ht->buckets[bucket], key);
  entry_t *next = entry->next;

  /// Check if the next entry should be updated or not
  if (next != NULL && next->key == key)
    {
      next->value = value;
    }
  else
    {
      entry->next = entry_create(key, value, next);
    }
}

/*void test_create_destroy()
{
   ioopm_hash_table_t *ht = ioopm_hash_table_create();
   CU_ASSERT_PTR_NOT_NULL(ht);
   ioopm_hash_table_destroy(ht);
}*/

void test_create_destroy()
{
   ioopm_hash_table_t *ht = ioopm_hash_table_create();
   if(ht)
   {
        ioopm_hash_table_destroy(ht);
        puts("Destroyed");
   }
   else
   {
        puts("Failed to create");
   }
}

void test_lookup1()
{
      ioopm_hash_table_t *ht = ioopm_hash_table_create();
      for(int i = 0; i < 18; i++)
      {
        char *test_string = ioopm_hash_table_lookup(ht, i);
        if(test_string)
            printf("%s", test_string);
        else
            puts("Lookup Failed");
      }
}

void test_lookup2()
{
    ioopm_hash_table_t *test = ioopm_hash_table_create();
    for (int i = 0; i < 18; i++)
    {
        if(ioopm_hash_table_lookup(test, i))
        {
            if(i == 12/*k*/)
                puts("k exists");
        }
    }
    ioopm_hash_table_insert(test, 12, "v");
    char *test_var = ioopm_hash_table_lookup(test, 12);
    if(!strcmp(test_var, "v"))
        puts("Insert Works");
    else
        puts("Insert Fail");

    ioopm_hash_table_destroy(test);

}

void test_insert()
{
    ioopm_hash_table_t *test = ioopm_hash_table_create();
    ioopm_hash_table_insert(test, 5, "test");
    if(test->buckets[5].next)
        printf("Found string: %s\n", test->buckets[5].next->value);
    else
        printf("Found Nothing");

    ioopm_hash_table_destroy(test);
}

int main()
{
    test_create_destroy();
    test_insert();
}