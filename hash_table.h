#pragma once
#include <stdbool.h>
#include "linked_list.h"
#include "common.h"

/**
 * @file hash_table.h
 * @author Fox Hamrén, Henrik von Bahr
 * @date 1 Sep 2021
 * @brief Simple hash table that maps elem_t keys to elem_t values.
 *
 * Header defines all functions needed to modify the hashtable
 * Some functions need their own defined functions according to the typedefs below
 * One typedef is found in common.h as the function is shared with
 * linked_list.h
 *
 * @see $CANVAS_OBJECT_REFERENCE$/assignments/gf5efa1610dfd73b58fef071f6c1d7a90
 */

typedef struct entry entry_t;
typedef struct hash_table ioopm_hash_table_t;

typedef bool (ioopm_predicate)(elem_t key, elem_t value, void *extra);      //Template for "any" and "all" functions
typedef void(*ioopm_apply_function)(elem_t key, elem_t value, void *extra); //Template for "apply" function
typedef int(*ioopm_hash_function)(elem_t key);                              //Template for key hash function


/// @brief Create a new hash table with preset load_factor, size and comparisonfunctions
/// Meant for a standard table with int keys and string values
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create();

/// @brief Create a new hash table with custom settings for key and value as well as loadfactor and initial buckets
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create_advanced(ioopm_hash_function hash, ioopm_eq_function key, ioopm_eq_function val, float load_factor, int init_size);

/// @brief Delete a hash table and free its memory
/// @param ht a hash table to be deleted
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht);

/// @brief add key => value entry in hash table ht
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value);

/// @brief lookup value for key in hash table ht
/// @param ht hash table operated upon
/// @param key key to lookup
/// @return option_t containing whether operation was successful and value
option_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key);

/// @brief remove any mapping from key to a value
/// @param ht hash table operated upon
/// @param key key to remove
/// @return option_t containing whether operation was successful and value
option_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key);

/// @brief returns the number of key => value entries in the hash table
/// @param h hash table operated upon
/// @return the number of key => value entries in the hash table
size_t ioopm_hash_table_size(ioopm_hash_table_t *ht);

/// @brief checks if the hash table is empty
/// @param h hash table operated upon
/// @return true is size == 0, else false
bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht);

/// @brief clear all the entries in a hash table
/// @param h hash table operated upon
void ioopm_hash_table_clear(ioopm_hash_table_t *ht);

/// @brief return the keys for all entries in a hash map (in no particular order, but same as ioopm_hash_table_values)
/// @param h hash table operated upon
/// @return a linked list of keys for hash table h
ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht);

/// @brief return the values for all entries in a hash map (in no particular order, but same as ioopm_hash_table_keys)
/// @param h hash table operated upon
/// @return an array of values for hash table h
ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht);

/// @brief check if a hash table has an entry with a given key
/// @param h hash table operated upon
/// @param key the key sought
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key);

/// @brief check if a hash table has an entry with a given value
/// @param h hash table operated upon
/// @param value the value sought
bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value);

/// @brief check if a predicate is satisfied by all entries in a hash table
/// @param h hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg);

/// @brief check if a predicate is satisfied by any entry in a hash table
/// @param h hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg);

/// @brief apply a function to all entries in a hash table
/// @param h hash table operated upon
/// @param apply_fun the function to be applied to all elements
/// @param arg extra argument to apply_fun
void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, void *arg);