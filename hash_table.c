#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

#define start_buckets 17

struct entry
{
  elem_t key;    // holds the key
  elem_t value;  // holds the value
  entry_t *next; // points to the next entry (possibly NULL)
};

struct hash_table
{
  entry_t *buckets;                     
  ioopm_hash_function hash_func;      
  ioopm_eq_function key_equiv_func;
  ioopm_eq_function value_equiv_func;
  float load_factor; 
  int num_buckets;
  size_t size;
};

//----------------------------------------------------------------------------------------------

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

ioopm_hash_table_t *ioopm_hash_table_create()
{
    ioopm_hash_table_t *result = calloc(1, sizeof(ioopm_hash_table_t));
    result->buckets = calloc(start_buckets, sizeof(entry_t));
    result->hash_func = key_hash;
    result->key_equiv_func = cmp_int;
    result->value_equiv_func = cmp_str;
    result->num_buckets = start_buckets;
    result->load_factor = 0.75;
    return result;
}

ioopm_hash_table_t *ioopm_hash_table_create_advanced(ioopm_hash_function hash, 
                                            ioopm_eq_function key_func, 
                                            ioopm_eq_function val_func,
                                            float load_factor,
                                            int init_size)
{
    ioopm_hash_table_t *result = calloc(1, sizeof(ioopm_hash_table_t));
    result->buckets = calloc(17, sizeof(entry_t));
    result->hash_func = hash;
    result->key_equiv_func = key_func;
    result->value_equiv_func = val_func;
    result->num_buckets = init_size;
    result->load_factor = load_factor;
    return result;
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

//-----------------------------------------------------------------------------------------------

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

//When loadfactor is reached, run to change values in buckets
static void rehash_buckets(ioopm_hash_table_t *ht)
{
    ioopm_list_t *keys = ioopm_hash_table_keys(ht);
    ioopm_list_t *values = ioopm_hash_table_values(ht);
    int table_size = ht->size;
    ioopm_hash_table_clear(ht);
    ht->num_buckets = ht->num_buckets*2;
    free(ht->buckets);
    ht->buckets = calloc(ht->num_buckets, sizeof(entry_t));
    for(int i = 0; i < table_size; i++)
    {
        ioopm_hash_table_insert(ht, ioopm_linked_list_get(keys, i), ioopm_linked_list_get(values, i));
    }
    ioopm_linked_list_destroy(keys);
    ioopm_linked_list_destroy(values);
}

//-----------------------------------------------------------------------------------------------

static void recurse_destroy(entry_t *chosen)
{
    if(chosen->next) //If next pointer exists, recurse
        recurse_destroy(chosen->next);
    
    free(chosen); //After recursion free current pointer
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
    for(size_t i = 0; i <  ht->num_buckets; i++)
    {
      entry_t *ptr = ht->buckets[i].next; //Gets next pointer
      if(ptr) 
        recurse_destroy(ptr); //Sends next pointer if not null
    }
    free(ht->buckets);
    free(ht);
}

//-----------------------------------------------------------------------------------------------

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value)
{
    /// Calculate the bucket for this entry
    size_t bucket = abs(ht->hash_func(key) % ht->num_buckets);
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
    
    if((float)ht->size/(float)ht->num_buckets > ht->load_factor)
    {
        rehash_buckets(ht);
    }
}

//-----------------------------------------------------------------------------------------------

option_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key)
{
  /// Find the previous entry for key
  entry_t *tmp = find_previous_entry_for_key(&ht->buckets[abs(ht->hash_func(key) % ht->num_buckets)], key, ht->key_equiv_func);
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

//-----------------------------------------------------------------------------------------------

option_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key)
{
  entry_t *prev_ptr = find_previous_entry_for_key(&ht->buckets[ht->hash_func(key) % ht->num_buckets], key, ht->key_equiv_func);
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

//-----------------------------------------------------------------------------------------------

size_t ioopm_hash_table_size(ioopm_hash_table_t *ht)
{
  return ht->size;
}

//-----------------------------------------------------------------------------------------------

bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht)
{
  return ht->size == 0;
}

//-----------------------------------------------------------------------------------------------

void ioopm_hash_table_clear(ioopm_hash_table_t *ht)
{
  for(size_t i = 0; i < ht->num_buckets; i++)
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

//-----------------------------------------------------------------------------------------------

ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht)
{
    ioopm_list_t *list = ioopm_linked_list_create(ht->key_equiv_func);
    size_t p = 0;
    for(size_t i = 0; i < ht->num_buckets; i++)
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

//-----------------------------------------------------------------------------------------------

ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht)
{
    ioopm_list_t *returned_vals = ioopm_linked_list_create(ht->value_equiv_func);
    size_t p = 0;
    for(size_t i = 0; i < ht->num_buckets; i++)
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

//-----------------------------------------------------------------------------------------------

static bool key_equiv(elem_t key, elem_t value_ignored, void *x)
{
  elem_t *other_key_ptr = x;
  elem_t other_key = *other_key_ptr;
  return key.int_value == other_key.int_value;
}

static bool value_equiv(elem_t key_ignored, elem_t value, void *x)
{
  elem_t *other_pointer = x;
  return !strcmp((char *) value.pointer, (char *) other_pointer->pointer); //Strcmp returns 0 when strings are equal, which would equal to false in the
}                                                                          //functions return statement, hence the "!" in the beginning

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

//-----------------------------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------------------------

ioopm_hash_table_t *create_large_table()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create();
  for(int i = 0; i < 50; i++)
    {
      ioopm_hash_table_insert(ht, (elem_t) {.int_value = i}, (elem_t) {.pointer = (char *) {"Value"}});
    }
  return ht;
}
