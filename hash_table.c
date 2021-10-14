#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

#define No_Buckets 17

static entry_t *find_previous_entry_for_key(entry_t *bucket, elem_t key, ioopm_eq_function eq)
{
  entry_t *cursor = bucket->next;
  entry_t *old_cursor = bucket;
  while (cursor != NULL)
    {
      if (eq(cursor->key, key))
        {
          return old_cursor; /// Ends the whole function!
        }
      old_cursor = cursor;
      cursor = cursor->next; /// Step forward to the next entry, and repeat loop
    }
    return bucket;
}
 
int key_hash(elem_t a)
{
    return a.int_value;
}

bool cmp_int(elem_t a, elem_t b)
{
    return a.int_value == b.int_value;
}

bool cmp_str(elem_t a, elem_t b)
{
    return strcmp((char *) a.pointer, (char *) b.pointer);
}

ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function hash, ioopm_eq_function key_func, ioopm_eq_function val_func)
{
    /// Allocate space for a ioopm_hash_table_t = 17 pointers to
    /// entry_t's, which will be set to NULL
    ioopm_hash_table_t *result = calloc(1, sizeof(ioopm_hash_table_t));
    result->hash_func = hash;
    result->key_equiv_func = key_func;
    result->value_equiv_func = val_func;
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

option_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key)
{
  /// Find the previous entry for key
  entry_t *tmp = find_previous_entry_for_key(&ht->buckets[ht->hash_func(key) % No_Buckets], key, ht->key_equiv_func);
  entry_t *next = tmp->next;

  if (next && ht->key_equiv_func(next->key, key))
  {
    return Success(next->value);
  }
  else
  {
    return Failure();
  }
}

static entry_t *entry_create(elem_t key, elem_t value, entry_t *entry)
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

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value)
{
  /// Calculate the bucket for this entry
  size_t bucket = abs(ht->hash_func(key) % No_Buckets);
  /// Search for an existing entry for a key
  entry_t *entry = find_previous_entry_for_key(&ht->buckets[bucket], key, ht->key_equiv_func);
  entry_t *next = entry->next;

  /// Check if the next entry should be updated or not
  if (next != NULL && ht->key_equiv_func(next->key, key))
    {
      next->value = value;
    }
  else
    {
      entry->next = entry_create(key, value, next);
      ht->size += 1;
    }
}

option_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key)
{
  entry_t *prev_ptr = find_previous_entry_for_key(&ht->buckets[ht->hash_func(key) % No_Buckets], key, ht->key_equiv_func);
  entry_t *replacing_ptr;
  elem_t removed_val;
  
  if(prev_ptr->next && ht->key_equiv_func(prev_ptr->next->key, key))//If next value exists (Entry 1)
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
  ioopm_hash_table_t *ht = ioopm_hash_table_create(key_hash, cmp_int, cmp_str);
  for(size_t i = 0; i < 50; i++)
    {
      ioopm_hash_table_insert(ht, (elem_t) {.int_value = i}, (elem_t) {.pointer = (char *) {"Value"}});
    }
  return ht;
}

ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht)
{
    ioopm_list_t *list = ioopm_linked_list_create(ht->key_equiv_func);
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

ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht)
{
    ioopm_list_t *returned_vals = ioopm_linked_list_create(ht->value_equiv_func);
    size_t p = 0;
    for(size_t i = 0; i < No_Buckets; i++) //Denna typ av loop sker ofta
    //Försök komma på sätt att bryta ut
    {
        entry_t *ptr = ht->buckets[i].next; 

        while(ptr && p < ht->size)
        {
            ioopm_linked_list_append(returned_vals, ptr->value);
            p++;
            ptr = ptr->next;
        }
    }
    return returned_vals;
}

static bool key_equiv(elem_t key, elem_t value_ignored, void *x)
{
  elem_t *other_key_ptr = x;
  elem_t other_key = *other_key_ptr;
  return key.int_value == other_key.int_value;
}

static bool value_equiv(elem_t key_ignored, elem_t value, void *x)
{
  elem_t *other_pointer = x;
  return !strcmp((char *) value.pointer, (char *) other_pointer->pointer);
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key)
{
    return ioopm_hash_table_any(ht, key_equiv, &key);
}

bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value)
{
    return ioopm_hash_table_any(ht, value_equiv, &value);
}

bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
    size_t size = ioopm_hash_table_size(ht);
    ioopm_list_t *keys = ioopm_hash_table_keys(ht);
    ioopm_list_t *values = ioopm_hash_table_values(ht);
    bool result = true;
    for (size_t i = 0; i < size && result; ++i)
      {
        result = result && pred(ioopm_linked_list_get(keys, i), ioopm_linked_list_get(values, i), arg);
      }
    ioopm_linked_list_destroy(values);
    ioopm_linked_list_destroy(keys);
    return result;
}

bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
    size_t size = ioopm_hash_table_size(ht);
    ioopm_list_t *keys = ioopm_hash_table_keys(ht);
    ioopm_list_t *values = ioopm_hash_table_values(ht);
    bool result = false;
    for (size_t i = 0; i < size && !result; ++i)
      {
        result = pred(ioopm_linked_list_get(keys, i), ioopm_linked_list_get(values, i), arg);
      }
    ioopm_linked_list_destroy(values);
    ioopm_linked_list_destroy(keys);
    return result;
}

void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, void *arg)
{
    size_t size = ioopm_hash_table_size(ht);
    ioopm_list_t *keys = ioopm_hash_table_keys(ht);
    ioopm_list_t *values = ioopm_hash_table_values(ht);
    for (size_t i = 0; i < size; ++i)
      {
        apply_fun(ioopm_linked_list_get(keys, i), ioopm_linked_list_get(values, i), arg);
      }
    ioopm_linked_list_destroy(values);
    ioopm_linked_list_destroy(keys);
}

static void check_modulo(elem_t key, elem_t value, void *arg)
{
  int *modulo_val = arg;
  if(key.int_value % *modulo_val == 0)
  {
    puts("Yes");
  }
  else
  {
    puts("No");
  }
  printf("%d\n", key.int_value);
}

//-----------------------------------------------------------------------------------------------------

static void test_create_destroy()
{
   ioopm_hash_table_t *ht = ioopm_hash_table_create(key_hash, cmp_int, cmp_str);
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
      ioopm_hash_table_t *ht = ioopm_hash_table_create(key_hash, cmp_int, cmp_str);
      ioopm_hash_table_insert(ht, (elem_t) {.int_value = 0}, (elem_t) {.pointer = "testing\n"});
      ioopm_hash_table_insert(ht, (elem_t) {.int_value = 6}, (elem_t) {.pointer = "testingwww\n"});
      ioopm_hash_table_insert(ht, (elem_t) {.int_value = 23}, (elem_t) {.pointer = "testingwww\n"});
      ioopm_hash_table_insert(ht, (elem_t) {.int_value = 5}, (elem_t) {.pointer = "testingasdasd\n"});
      ioopm_hash_table_insert(ht, (elem_t) {.int_value = 7}, (elem_t) {.pointer = "testingasdasd\n"});
      ioopm_hash_table_insert(ht, (elem_t) {.int_value = 8}, (elem_t) {.pointer = "testingaadsad\n"});
      ioopm_hash_table_insert(ht, (elem_t) {.int_value = 10}, (elem_t) {.pointer = "testing\n"});
      ioopm_hash_table_insert(ht, (elem_t) {.int_value = 11}, (elem_t) {.pointer = "testingsda\n"});
      for(int i = 0; i < 26; i++)
      {
        option_t test_string = ioopm_hash_table_lookup(ht, (elem_t) {.int_value = i});
        if(Successful(test_string))
            printf("%s key: %d\n", (char *) test_string.value.pointer, i);
        else
            puts("Lookup Failed");
      }
      ioopm_hash_table_destroy(ht);
}

static void test_lookup2()
{
    ioopm_hash_table_t *test = ioopm_hash_table_create(key_hash, cmp_int, cmp_str);
    for (int i = 0; i < 18; i++)
    {
        if(ioopm_hash_table_lookup(test, (elem_t) {.int_value = i}).success)
        {
            if(i == 12/*k*/)
                puts("k exists");
        }
    }
    ioopm_hash_table_insert(test, (elem_t) {.int_value = 12}, (elem_t) {.pointer = "v"});
    option_t test_var = ioopm_hash_table_lookup(test, (elem_t) {.int_value = 12});
    if(Successful(test_var) && !strcmp((char *) test_var.value.pointer, "v"))
        puts("Insert Works"); //First checks if valid and then checks value
    else
        puts("Insert Fail");

    ioopm_hash_table_destroy(test);

}

static void test_insert()
{
    ioopm_hash_table_t *test = ioopm_hash_table_create(key_hash, cmp_int, cmp_str);
    ioopm_hash_table_insert(test, (elem_t) {.int_value = 5}, (elem_t) {.pointer = "test"});
    if(test->buckets[5].next)
        printf("Found string: %s\n", (char *) test->buckets[5].next->value.pointer);
    else
        printf("Found Nothing");

    ioopm_hash_table_destroy(test);
}

static void test_remove()
{
    ioopm_hash_table_t *ht = create_large_table();

    printf("Removed %s\n", (char *) ioopm_hash_table_remove(ht, (elem_t) {.int_value = 0}).value.pointer);
    printf("Removed %s\n", (char *) ioopm_hash_table_remove(ht, (elem_t) {.int_value = 13}).value.pointer);
    printf("Removed %s\n", (char *) ioopm_hash_table_remove(ht, (elem_t) {.int_value = 28}).value.pointer);
    printf("Removed %s\n", (char *) ioopm_hash_table_remove(ht, (elem_t) {.int_value = 48}).value.pointer);

    for(int i = 0; i < 50; i++)
    {
      if(!ioopm_hash_table_lookup(ht, (elem_t) {.int_value = i}).success)
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
    ioopm_list_t *happy = ioopm_hash_table_values(test);
    if((char *) ioopm_linked_list_get(happy, 12).pointer && (char *) ioopm_linked_list_get(happy, 23).pointer && (char *) ioopm_linked_list_get(happy, 45).pointer)
        puts("Valid ptrs");
    else
        puts("Failed");
    ioopm_hash_table_destroy(test);
    ioopm_linked_list_destroy(happy);
}

static void test_key_list()
{
    ioopm_hash_table_t *test = create_large_table();
    ioopm_list_t *happy = ioopm_hash_table_keys(test);
    if(ioopm_linked_list_get(happy, 3).int_value == 35 && ioopm_linked_list_get(happy, 21).int_value == 41 && ioopm_linked_list_get(happy, 49).int_value == 16)
        puts("Valid keys");
    else
        puts("Failed");
    ioopm_hash_table_destroy(test);
    ioopm_linked_list_destroy(happy);
}

static void test_has_key()
{
    ioopm_hash_table_t *ht = create_large_table();
    if(ioopm_hash_table_has_key(ht, (elem_t) {.int_value = 12}) && !ioopm_hash_table_has_key(ht, (elem_t) {.int_value = 51}))
        puts("Has key works");
    else
        puts("Failed key search");
    ioopm_hash_table_destroy(ht);
}

static void test_has_value()
{
    ioopm_hash_table_t *ht = create_large_table();
    if(ioopm_hash_table_has_value(ht, (elem_t) {.pointer = "Value"}) && !ioopm_hash_table_has_value(ht, (elem_t) {.pointer = "51"}))
        puts("Has value works");
    else
        puts("Failed value search");
    ioopm_hash_table_destroy(ht);
}

static void test_all_values()
{
    ioopm_hash_table_t *ht = create_large_table();
    elem_t test1 = {.pointer = "Value"};
    elem_t test2 = {.pointer = "aavasv"};
    if(ioopm_hash_table_all(ht, value_equiv, &test1) && !ioopm_hash_table_all(ht, value_equiv, &test2))
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
  // test_create_destroy();
  // test_lookup1();
  // test_lookup2();
  // test_insert();
  // test_remove();
  // test_size();
  // test_clear();
  //test_value_list();
  //test_key_list();
  //test_has_key();
  //test_has_value();
  //test_all_values();
  test_apply_to_all();
}