#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"
//#include <CUnit/CUnit.h>


entry_t *find_previous_entry_for_key(entry_t *bucket, int key)
{
  entry_t *cursor = bucket->next;
  entry_t *old_cursor = bucket;
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

ioopm_hash_table_t *ioopm_hash_table_create()
{
    /// Allocate space for a ioopm_hash_table_t = 17 pointers to
    /// entry_t's, which will be set to NULL
    ioopm_hash_table_t *result = calloc(1, sizeof(ioopm_hash_table_t));
    return result;
}

void recurse_destroy(entry_t *chosen)
{
    if(chosen->next) //If next pointer exists, recurse
        recurse_destroy(chosen->next);
    
    free(chosen); //After recursion free current pointer
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
    for(int i = 0; i < 17; i++)
    {
      entry_t *ptr = ht->buckets[i].next; //Gets next pointer
      if(ptr) 
        recurse_destroy(ptr); //Sends next pointer if not null
    }
    free(ht);
}

/*void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
    for(int i = 0; i < 17; i++)
    {
      entry_t *ptr = ht->buckets[i].next; //Gets next pointer
      entry_t *new_ptr;
      while(ptr)
      {
          new_ptr = ptr->next;
          free(ptr);
          ptr = new_ptr;
      }
    }
    free(ht);
}*/

option_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, int key)
{
  /// Find the previous entry for key
  entry_t *tmp = find_previous_entry_for_key(&ht->buckets[key % 17], key);
  entry_t *next = tmp->next;

  if (next && next->value && next->key == key)
  {
    return Success(next->value);
  }
  else
  {
    return Failure();
  }
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

option_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, int key)
{
  entry_t *prev_ptr = find_previous_entry_for_key(&ht->buckets[key % 17], key);
  entry_t *replacing_ptr;
  char *removed_val;
  
  if(prev_ptr->next && prev_ptr->next->key == key)//If next value exists (Entry 1)
  {
    removed_val = prev_ptr->next->value; //Save value to be removed
      if(prev_ptr->next->next)//If entry exists two steps forward (Entry 2)
      {
          replacing_ptr = prev_ptr->next->next;//Save entry 2
          free(prev_ptr->next);//Remove Entry 1
          prev_ptr->next = replacing_ptr; //Move entry 2
      }
      else //Entry 2 doesn't exist
      {
          free(prev_ptr->next);
          prev_ptr->next = NULL;
      }
    return Success(removed_val);
  }
  else
    return Failure()
}

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
      ioopm_hash_table_insert(ht, 0, "testing\n");
      ioopm_hash_table_insert(ht, 6, "testingwww\n");
      ioopm_hash_table_insert(ht, 23, "testingwww\n");
      ioopm_hash_table_insert(ht, 5, "testingasdasd\n");
      ioopm_hash_table_insert(ht, 7, "testingasdasd\n");
      ioopm_hash_table_insert(ht, 8, "testingaadsad\n");
      ioopm_hash_table_insert(ht, 10, "testing\n");
      ioopm_hash_table_insert(ht, 11, "testingsda\n");
      for(int i = 0; i < 26; i++)
      {
        option_t test_string = ioopm_hash_table_lookup(ht, i);
        if(Successful(test_string))
            printf("%s key: %d\n", test_string.value, i);
        else
            puts("Lookup Failed");
      }
      ioopm_hash_table_destroy(ht);
}

void test_lookup2()
{
    ioopm_hash_table_t *test = ioopm_hash_table_create();
    for (int i = 0; i < 18; i++)
    {
        if(ioopm_hash_table_lookup(test, i).success)
        {
            if(i == 12/*k*/)
                puts("k exists");
        }
    }
    ioopm_hash_table_insert(test, 12, "v");
    option_t test_var = ioopm_hash_table_lookup(test, 12);
    if(Successful(test_var) && !strcmp(test_var.value, "v"))
        puts("Insert Works"); //First checks if valid and then checks value
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

void test_remove()
{
    ioopm_hash_table_t *ht = ioopm_hash_table_create();
    ioopm_hash_table_insert(ht, 0, "First Removed");

    for(int i = 0; i < 50; i++)
    {
      if(i % 17 != 0)
          ioopm_hash_table_insert(ht, i, "Value");
    }

    printf("Removed %s\n", ioopm_hash_table_remove(ht, 0).value);
    printf("Removed %s\n", ioopm_hash_table_remove(ht, 13).value);
    printf("Removed %s\n", ioopm_hash_table_remove(ht, 28).value);
    printf("Removed %s\n", ioopm_hash_table_remove(ht, 48).value);

    for(int i = 0; i < 50; i++)
    {
      if(!ioopm_hash_table_lookup(ht, i).success)
          printf("Did not find %d\n", i);
    }
    ioopm_hash_table_destroy(ht);
}

int main()
{
    test_create_destroy();
    test_insert();
    test_lookup1();
    test_lookup2();
    test_remove();
}