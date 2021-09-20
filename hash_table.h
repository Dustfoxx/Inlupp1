#pragma once

/**
 * @file hash_table.h
 * @author Fox Hamrén, Henrik von Bahr
 * @date 1 Sep 2019
 * @brief Simple hash table that maps integer keys to string values.
 *
 * Here typically goes a more extensive explanation of what the header
 * defines. Doxygens tags are words preceeded by either a backslash @\
 * or by an at symbol @@.
 *
 * @see $CANVAS_OBJECT_REFERENCE$/assignments/gf5efa1610dfd73b58fef071f6c1d7a90
 */

typedef struct entry entry_t;
typedef struct hash_table ioopm_hash_table_t;

struct entry
{
  int key;       // holds the key
  char *value;   // holds the value
  entry_t *next; // points to the next entry (possibly NULL)
};

struct hash_table
{
  entry_t buckets[17];
};

/// @brief Create a new hash table
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create();

/// @brief Delete a hash table and free its memory
/// @param ht a hash table to be deleted
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht);

/// @brief add key => value entry in hash table ht
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, int key, char *value);

/// @brief lookup value for key in hash table ht
/// @param ht hash table operated upon
/// @param key key to lookup
/// @return the value mapped to by key (FIXME: incomplete)
char *ioopm_hash_table_lookup(ioopm_hash_table_t *ht, int key);

/// @brief remove any mapping from key to a value
/// @param ht hash table operated upon
/// @param key key to remove
/// @return the value mapped to by key (FIXME: incomplete)
char *ioopm_hash_table_remove(ioopm_hash_table_t *ht, int key);