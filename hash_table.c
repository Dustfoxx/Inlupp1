#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

#define No_Buckets 17

static entry_t *find_previous_entry_for_key(entry_t *bucket, int key)
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

static void recurse_destroy(entry_t *chosen)
{
    if(chosen->next) //If next pointer exists, recurse
        recurse_destroy(chosen->next);
    
    free(chosen); //After recursion free current pointer
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
    for(size_t i = 0; i <  No_Buckets; i++)
    {
      entry_t *ptr = ht->buckets[i].next; //Gets next pointer
      if(ptr) 
        recurse_destroy(ptr); //Sends next pointer if not null
    }
    free(ht);
}

option_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, int key)
{
  /// Find the previous entry for key
  entry_t *tmp = find_previous_entry_for_key(&ht->buckets[key % No_Buckets], key);
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

static entry_t *entry_create(int key, char *value, entry_t *entry)
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
  size_t bucket = abs(key % No_Buckets);
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
      ht->size += 1;
    }
}

option_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, int key)
{
  entry_t *prev_ptr = find_previous_entry_for_key(&ht->buckets[key % No_Buckets], key);
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
    ht->size -= 1; 
    return Success(removed_val);
  }
  else
    return Failure()
}

size_t ioopm_hash_table_size(ioopm_hash_table_t *ht)
{
  return ht->size;
}

bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht)
{
  return ht->size == 0;
}

void ioopm_hash_table_clear(ioopm_hash_table_t *ht)
{
  for(size_t i = 0; i < No_Buckets; i++)
    {
      entry_t *ptr = ht->buckets[i].next; //Gets next pointer
      entry_t *new_ptr;
      while(ptr)
      {
          new_ptr = ptr->next;
          free(ptr);
          ht->size -= 1;
          ptr = new_ptr;
      }
      ht->buckets[i].next = NULL;
    }
}

ioopm_hash_table_t *create_large_table()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create();
  for(size_t i = 0; i < 50; i++)
    {
      ioopm_hash_table_insert(ht, i, "Value");
    }
  return ht;
}

ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht)
{
    ioopm_list_t *list = ioopm_linked_list_create();
    size_t p = 0;
    for(size_t i = 0; i < No_Buckets; i++) //TODO: Denna typ av loop sker ofta
    //Försök komma på sätt att bryta ut
    {
        entry_t *ptr = ht->buckets[i].next;
        
        while(ptr && p < ht->size)
        {
            ioopm_linked_list_append(list, ptr->key);
            p++;
            ptr = ptr->next;
        }
    }
    return list;
}

char **ioopm_hash_table_values(ioopm_hash_table_t *ht)
{
    char **returned_vals = calloc(ht->size + 1, sizeof(char *));
    size_t p = 0;
    for(size_t i = 0; i < No_Buckets; i++) //Denna typ av loop sker ofta
    //Försök komma på sätt att bryta ut
    {
        entry_t *ptr = ht->buckets[i].next; 

        while(ptr && p < ht->size)
        {
            returned_vals[p] = malloc(strlen(ptr->value) + 1);
            strcpy(returned_vals[p], ptr->value);
            p++;
            ptr = ptr->next;
        }
    }
    returned_vals[p] = NULL;
    return returned_vals;
}

void clear_aloccated_char_array(char **arg)
{
  for(size_t i = 0; arg[i]; i++)
  {
      free(arg[i]);
  }
  free(arg);
}

static bool key_equiv(int key, char *value_ignored, void *x)
{
  int *other_key_ptr = x;
  int other_key = *other_key_ptr;
  return key == other_key;
}

static bool value_equiv(int key_ignored, char *value, void *x)
{
  return !strcmp(value, x);
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, int key)
{
    return ioopm_hash_table_any(ht, key_equiv, &key);
}

bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, char *value)
{
    return ioopm_hash_table_any(ht, value_equiv, value);
}

bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
    size_t size = ioopm_hash_table_size(ht);
    ioopm_list_t *keys = ioopm_hash_table_keys(ht);
    char **values = ioopm_hash_table_values(ht);
    bool result = true;
    for (size_t i = 0; i < size && result; ++i)
      {
        result = result && pred(ioopm_linked_list_get(keys, i), values[i], arg);
      }
    clear_aloccated_char_array(values);
    ioopm_linked_list_destroy(keys);
    return result;
}

bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
    size_t size = ioopm_hash_table_size(ht);
    ioopm_list_t *keys = ioopm_hash_table_keys(ht);
    char **values = ioopm_hash_table_values(ht);
    bool result = false;
    for (size_t i = 0; i < size && !result; ++i)
      {
        result = pred(ioopm_linked_list_get(keys, i), values[i], arg);
      }
    clear_aloccated_char_array(values);
    ioopm_linked_list_destroy(keys);
    return result;
}

void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, void *arg)
{
    size_t size = ioopm_hash_table_size(ht);
    ioopm_list_t *keys = ioopm_hash_table_keys(ht);
    char **values = ioopm_hash_table_values(ht);
    for (size_t i = 0; i < size; ++i)
      {
        apply_fun(ioopm_linked_list_get(keys, i), values[i], arg);
      }
    clear_aloccated_char_array(values);
    ioopm_linked_list_destroy(keys);
}

static void check_modulo(int key, char *value, void *arg)
{
  int *modulo_val = arg;
  if(key % *modulo_val == 0)
  {
    puts("Yes");
  }
  else
  {
    puts("No");
  }
  printf("%d\n", key);
}

//-----------------------------------------------------------------------------------------------------

static void test_create_destroy()
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

static void test_lookup1()
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

static void test_lookup2()
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

static void test_insert()
{
    ioopm_hash_table_t *test = ioopm_hash_table_create();
    ioopm_hash_table_insert(test, 5, "test");
    if(test->buckets[5].next)
        printf("Found string: %s\n", test->buckets[5].next->value);
    else
        printf("Found Nothing");

    ioopm_hash_table_destroy(test);
}

static void test_remove()
{
    ioopm_hash_table_t *ht = create_large_table();

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

static void test_size()
{
  ioopm_hash_table_t *ht = create_large_table();

    printf("%zu\n", ht->size);
  ioopm_hash_table_destroy(ht);
}

static void test_clear()
{
    ioopm_hash_table_t *ht = create_large_table();
    if(!ioopm_hash_table_is_empty(ht))
        puts("List is created");
    ioopm_hash_table_clear(ht);
    if(ioopm_hash_table_is_empty(ht))
        puts("Successfully cleared");
    ioopm_hash_table_destroy(ht);
}

static void test_value_list()
{
    ioopm_hash_table_t *test = create_large_table();
    char **happy = ioopm_hash_table_values(test);
    if(happy[12] && happy[23] && happy[45])
        puts("Valid ptrs");
    else
        puts("Failed");
    ioopm_hash_table_destroy(test);
    clear_aloccated_char_array(happy);
}

static void test_key_list()
{
    ioopm_hash_table_t *test = create_large_table();
    ioopm_list_t *happy = ioopm_hash_table_keys(test);
    if(ioopm_linked_list_get(happy, 3) == 35 && ioopm_linked_list_get(happy, 21) == 41 && ioopm_linked_list_get(happy, 49) == 16)
        puts("Valid keys");
    else
        puts("Failed");
    ioopm_hash_table_destroy(test);
    ioopm_linked_list_destroy(happy);
}

static void test_has_key()
{
    ioopm_hash_table_t *ht = create_large_table();
    if(ioopm_hash_table_has_key(ht, 12) && !ioopm_hash_table_has_key(ht, 51))
        puts("Has key works");
    else
        puts("Failed key search");
    ioopm_hash_table_destroy(ht);
}

static void test_has_value()
{
    ioopm_hash_table_t *ht = create_large_table();
    if(ioopm_hash_table_has_value(ht, "Value") && !ioopm_hash_table_has_value(ht, "51"))
        puts("Has value works");
    else
        puts("Failed value search");
    ioopm_hash_table_destroy(ht);
}

static void test_all_values()
{
    ioopm_hash_table_t *ht = create_large_table();
    if(ioopm_hash_table_all(ht, value_equiv, "Value") && !ioopm_hash_table_all(ht, value_equiv, "Valuedasd"))
        puts("All works");
    else
        puts("Failed all");
    ioopm_hash_table_destroy(ht);
}

static void test_apply_to_all()
{
    ioopm_hash_table_t *ht = create_large_table();
    int modulo_val = 5;
    ioopm_hash_table_apply_to_all(ht, check_modulo, &modulo_val);
    ioopm_hash_table_destroy(ht);
}

int main()
{
  test_create_destroy();
  test_lookup1();
  test_lookup2();
  test_insert();
  test_remove();
  test_size();
  test_clear();
  test_value_list();
  test_key_list();
  test_has_key();
  test_has_value();
  test_all_values();
  test_apply_to_all();
}