#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "hash_table.h"
#include "linked_list.h"
#include "common.h"
#include "iterator.h"

//------------------------------------------------S T R U C T S-----------------------------------------------------------------------------------
//Compiler was not able to find structs from c-files, so these are copied into here to make it work.
//We know this is not best practice by any means, but since this file is strictly for unit tests, we decided to go with it anyway.

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

struct iter
{
    struct list *current;
    struct list *next;
    struct list *first;
};

struct list
{
    elem_t value;
    struct list *next;
    ioopm_eq_function list_eq;
};

//-----------------------------------------------------------------------------------------------------------------------------------

//------HASH TABLE TESTS------

static ioopm_hash_table_t *create_large_table_test()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create();
  for(int i = 0; i < 50; i++)
    {
      ioopm_hash_table_insert(ht, (elem_t) {.int_value = i}, (elem_t) {.pointer = (char *) {"Value"}});
    }
  return ht;
}

static void test_hash_table_create_destroy()
{
   ioopm_hash_table_t *ht = ioopm_hash_table_create();
   CU_ASSERT_PTR_NOT_NULL(ht);
   ioopm_hash_table_destroy(ht);
}

static void test_hash_table_insert1()
{
    ioopm_hash_table_t *test = ioopm_hash_table_create();
    ioopm_hash_table_insert(test, (elem_t) {.int_value = 2}, (elem_t) {.pointer = "Hej"});
    ioopm_hash_table_insert(test, (elem_t) {.int_value = 5}, (elem_t) {.pointer = "Vad"});
    ioopm_hash_table_insert(test, (elem_t) {.int_value = 8}, (elem_t) {.pointer = "Heter"});
    ioopm_hash_table_insert(test, (elem_t) {.int_value = 15}, (elem_t) {.pointer = "Du"});
    CU_ASSERT_STRING_EQUAL(ioopm_hash_table_lookup(test, (elem_t) {.int_value = 2}).value.pointer, "Hej");
    CU_ASSERT_STRING_EQUAL(ioopm_hash_table_lookup(test, (elem_t) {.int_value = 5}).value.pointer, "Vad");
    CU_ASSERT_STRING_EQUAL(ioopm_hash_table_lookup(test, (elem_t) {.int_value = 8}).value.pointer, "Heter");
    CU_ASSERT_STRING_EQUAL(ioopm_hash_table_lookup(test, (elem_t) {.int_value = 15}).value.pointer, "Du");


    ioopm_hash_table_insert(test, (elem_t) {.int_value = 20}, (elem_t) {.pointer = "Woohoo"});
    CU_ASSERT_STRING_EQUAL(ioopm_hash_table_lookup(test, (elem_t) {.int_value = 20}).value.pointer, "Woohoo");

    ioopm_hash_table_destroy(test);
}

static void test_hash_table_insert2()
{
    ioopm_hash_table_t *test = ioopm_hash_table_create();
    ioopm_hash_table_insert(test, (elem_t) {.int_value = 20}, (elem_t) {.pointer = "Woohoo"});
    CU_ASSERT_STRING_EQUAL(ioopm_hash_table_lookup(test, (elem_t) {.int_value = 20}).value.pointer, "Woohoo");
    ioopm_hash_table_destroy(test);
}

static void test_hash_table_insert3()
{
    ioopm_hash_table_t *test = ioopm_hash_table_create();
    ioopm_hash_table_insert(test, (elem_t) {.int_value = -5}, (elem_t) {.pointer = "Woohoo"}); //Kollar edge-case ifall input-key ??r negativ. Denna g??rs om till positiv i insert, d??rf??r kollar vi i bucket 5 p?? raden nedan
    CU_ASSERT_TRUE(ioopm_hash_table_lookup(test, (elem_t) {.int_value = -5}).success);
    ioopm_hash_table_destroy(test);
}

static void test_hash_table_lookup1()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create();
  elem_t key = {.int_value = 10};
  if (!ioopm_hash_table_lookup(ht, (elem_t) {.int_value = 10}).success)
  {
    ioopm_hash_table_insert(ht, key, (elem_t) {.pointer = "Hejsan"});
  }
  //CU_ASSERT_STRING_EQUAL(ioopm_hash_table_lookup(ht, key).value.pointer, "Hejsan");
  ioopm_hash_table_destroy(ht);
}

static void test_hash_table_lookup2()
{
      ioopm_hash_table_t *ht = ioopm_hash_table_create();
      int check = 0;
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
            check++;
      }
      CU_ASSERT_EQUAL(8, check);
      ioopm_hash_table_destroy(ht);
}

static void test_hash_table_remove()
{
    ioopm_hash_table_t *ht = create_large_table_test();

    ioopm_hash_table_remove(ht, (elem_t) {.int_value = 0});
    ioopm_hash_table_remove(ht, (elem_t) {.int_value = 13});
    ioopm_hash_table_remove(ht, (elem_t) {.int_value = 28});
    ioopm_hash_table_remove(ht, (elem_t) {.int_value = 48});

    CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, (elem_t) {.int_value = 0}).success);
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, (elem_t) {.int_value = 13}).success);
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, (elem_t) {.int_value = 28}).success);
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, (elem_t) {.int_value = 48}).success);
    
    ioopm_hash_table_destroy(ht);
}

static void test_hash_table_size()
{
  ioopm_hash_table_t *ht = create_large_table_test();

  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 50);
  ioopm_hash_table_destroy(ht);
}

static void test_hash_table_empty()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));
  ioopm_hash_table_destroy(ht);
}

static void test_hash_table_clear()
{
    ioopm_hash_table_t *ht = create_large_table_test();
    if(!ioopm_hash_table_is_empty(ht))
      ioopm_hash_table_clear(ht);
    CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));
    ioopm_hash_table_destroy(ht);
}

static void test_hash_table_has_key1()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create();
  elem_t key = {.int_value = 10};
  if (!ioopm_hash_table_lookup(ht, key).success)
  {
    ioopm_hash_table_insert(ht, key, (elem_t) {.pointer = "Hejsan"});
  }
  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, key));
  ioopm_hash_table_destroy(ht);
}

static void test_hash_table_has_value1()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create();
  elem_t key = {.int_value = 10};
  elem_t value = {.pointer = "Hejsan"};
  if (!ioopm_hash_table_lookup(ht, key).success)
  {
    ioopm_hash_table_insert(ht, key, value);
  }
  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, value));
  ioopm_hash_table_destroy(ht);
}

static bool cmp_str_all(elem_t ignored, elem_t a, void *b)
{
    elem_t *temp = b;
    return !strcmp((char *) a.pointer, (char *) temp->pointer);
}

static void test_hash_table_all_values()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create();
  elem_t key = {.int_value = 10};
  elem_t value = {.pointer = "Hejsan"};
  ioopm_hash_table_insert(ht, key, value);
  CU_ASSERT_TRUE(ioopm_hash_table_all(ht, cmp_str_all, &value));
  ioopm_hash_table_destroy(ht);
}

void change_value (elem_t key, elem_t val, void *x)
{
    int **var = x;

    if(**var == key.int_value)
    {
      **var = **var*2; //Multiplies key with 2
    }
}

void test_ioopm_hash_table_apply_to_all(void)
{
  ioopm_hash_table_t *ht = create_large_table_test();

  int var = 2;
  int *tmpptr = &var; //Saves adress to var
  int **key = &tmpptr; //Saves adress to tmpptr

  ioopm_hash_table_apply_to_all(ht, change_value, key); //

  CU_ASSERT_EQUAL(**key, 64);
  ioopm_hash_table_destroy(ht);
}

//------LINKED LIST TESTS------

bool eq_int_test(elem_t a, elem_t b)
{
    return a.int_value == b.int_value;
}

static void test_linked_list_append()
{
  ioopm_list_t *list = ioopm_linked_list_create(eq_int_test);
  ioopm_linked_list_append(list, (elem_t) {.int_value = 4});
  ioopm_linked_list_append(list, (elem_t) {.int_value = 7});
  CU_ASSERT_EQUAL(list->next->value.int_value, 4);
  CU_ASSERT_EQUAL(list->next->next->value.int_value, 7);
  ioopm_linked_list_destroy(list);
}

static void test_linked_list_prepend()
{
  ioopm_list_t *list = ioopm_linked_list_create(eq_int_test);
  ioopm_linked_list_append(list, (elem_t) {.int_value = 4});
  ioopm_linked_list_append(list, (elem_t) {.int_value = 7});
  ioopm_linked_list_prepend(list, (elem_t) {.int_value = 3});
  CU_ASSERT_EQUAL(list->next->value.int_value, 3);
  CU_ASSERT_EQUAL(list->next->next->value.int_value, 4);
  CU_ASSERT_EQUAL(list->next->next->next->value.int_value, 7);
  ioopm_linked_list_destroy(list);
}

static void test_linked_list_get()
{
  ioopm_list_t *list = ioopm_linked_list_create(eq_int_test);
  ioopm_linked_list_append(list, (elem_t) {.int_value = 4});
  ioopm_linked_list_append(list, (elem_t) {.int_value = 7});
  elem_t value1 = ioopm_linked_list_get(list, 0);
  elem_t value2 = ioopm_linked_list_get(list, 1);
  CU_ASSERT_EQUAL(value1.int_value, 4);
  CU_ASSERT_EQUAL(value2.int_value, 7);
  ioopm_linked_list_destroy(list);
}

static void test_linked_list_insert()
{
    ioopm_list_t *list = create_list(eq_int_test);

    ioopm_linked_list_append(list,(elem_t) {.int_value = 1});
    ioopm_linked_list_append(list,(elem_t) {.int_value = 2});
    ioopm_linked_list_append(list,(elem_t) {.int_value = 3});
    ioopm_linked_list_append(list,(elem_t) {.int_value = 4});
    ioopm_linked_list_append(list,(elem_t) {.int_value = 5});
    ioopm_linked_list_append(list,(elem_t) {.int_value = 6});

    CU_ASSERT_TRUE(ioopm_linked_list_insert(list, 4, (elem_t) {.int_value = 54}));
    CU_ASSERT_TRUE(ioopm_linked_list_insert(list, 14, (elem_t) {.int_value = 546}));
    CU_ASSERT_FALSE(ioopm_linked_list_insert(list, 123, (elem_t) {.int_value = 123}));

    elem_t value1 = ioopm_linked_list_get(list, 4);
    elem_t value2 = ioopm_linked_list_get(list, 14);
    
    CU_ASSERT_EQUAL(value1.int_value, 54);
    CU_ASSERT_EQUAL(value2.int_value, 546);

    ioopm_linked_list_destroy(list);
}

static void test_linked_list_remove()
{
  ioopm_list_t *list = ioopm_linked_list_create(eq_int_test);
  ioopm_linked_list_append(list, (elem_t) {.int_value = 4});
  ioopm_linked_list_append(list, (elem_t) {.int_value = 7});
  ioopm_linked_list_append(list, (elem_t) {.int_value = 10});
  elem_t value1 = ioopm_linked_list_get(list, 0);
  elem_t value2 = ioopm_linked_list_get(list, 1);

  CU_ASSERT_EQUAL(value1.int_value, 4);
  CU_ASSERT_EQUAL(value2.int_value, 7);

  elem_t value1_removed = ioopm_linked_list_remove(list, 0);
  elem_t value2_removed = ioopm_linked_list_remove(list, 0);

  CU_ASSERT_EQUAL(value1_removed.int_value, 4);
  CU_ASSERT_EQUAL(value2_removed.int_value, 7);
  
  elem_t new_value1 = ioopm_linked_list_get(list, 0);
  elem_t new_value2 = ioopm_linked_list_get(list, 1);

  CU_ASSERT_EQUAL(new_value1.int_value, 10);
  CU_ASSERT_NOT_EQUAL(new_value2.int_value, 100);

  ioopm_linked_list_destroy(list);
}

static void test_linked_list_contains()
{
  ioopm_list_t *list = create_list(eq_int_test);

  ioopm_linked_list_append(list,(elem_t) {.int_value = 10});
  ioopm_linked_list_append(list,(elem_t) {.int_value = 20});
  ioopm_linked_list_append(list,(elem_t) {.int_value = 30});
  ioopm_linked_list_append(list,(elem_t) {.int_value = 40});
  ioopm_linked_list_append(list,(elem_t) {.int_value = 50});
  ioopm_linked_list_append(list,(elem_t) {.int_value = 69});

  elem_t value1;
  value1.int_value = 50;

  elem_t value2;
  value2.int_value = 500;

  CU_ASSERT_TRUE(ioopm_linked_list_contains(list, value1));

  CU_ASSERT_FALSE(ioopm_linked_list_contains(list, value2));

  ioopm_linked_list_destroy(list);

}

static void test_linked_list_size()
{
  ioopm_list_t *list = ioopm_linked_list_create(eq_int_test);

  elem_t value1;
  value1.int_value = 5;

  ioopm_linked_list_append(list, value1);
  ioopm_linked_list_append(list,(elem_t) {.int_value = 20});
  ioopm_linked_list_append(list,(elem_t) {.int_value = 30});
  ioopm_linked_list_append(list,(elem_t) {.int_value = 40});
  ioopm_linked_list_append(list,(elem_t) {.int_value = 50});
  ioopm_linked_list_append(list,(elem_t) {.int_value = 69});

  size_t size = ioopm_linked_list_size(list);

  CU_ASSERT_EQUAL(size, 6);

  ioopm_linked_list_destroy(list);
}

static void test_linked_list_is_empty()
{
  ioopm_list_t *list = ioopm_linked_list_create(eq_int_test);

  CU_ASSERT_TRUE(ioopm_linked_list_is_empty(list))

  elem_t value1;
  value1.int_value = 5;

  ioopm_linked_list_append(list, value1);

  CU_ASSERT_FALSE(ioopm_linked_list_is_empty(list));

  ioopm_linked_list_destroy(list);
}

static void test_linked_list_iterator()
{
  ioopm_list_t *list = ioopm_linked_list_create(eq_int_test);

  ioopm_linked_list_append(list, (elem_t) {.int_value = 1});
  ioopm_linked_list_append(list, (elem_t) {.int_value = 2});
  ioopm_linked_list_append(list, (elem_t) {.int_value = 3});
  ioopm_linked_list_append(list, (elem_t) {.int_value = 4});
  ioopm_linked_list_append(list, (elem_t) {.int_value = 5});

  ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

  CU_ASSERT_EQUAL(ioopm_iterator_current(iter).int_value, 1);

  ioopm_iterator_next(iter);

  CU_ASSERT_EQUAL(ioopm_iterator_current(iter).int_value, 2);

  ioopm_iterator_next(iter);
  ioopm_iterator_next(iter);
  ioopm_iterator_next(iter);
  ioopm_iterator_next(iter);

  CU_ASSERT_EQUAL(ioopm_iterator_current(iter).int_value, 5);

  ioopm_iterator_reset(iter);

  CU_ASSERT_EQUAL(ioopm_iterator_current(iter).int_value, 1);

  ioopm_iterator_destroy(&iter);

  CU_ASSERT_PTR_NULL(iter);

  ioopm_linked_list_destroy(list);
}


int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}

int main()
{
  CU_pSuite test_suite = NULL;
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  test_suite = CU_add_suite("Test Suite HT", init_suite, clean_suite);
  if (NULL == test_suite)
  {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (
    (NULL == CU_add_test(test_suite, "Test hash_table create/destroy", test_hash_table_create_destroy)) ||
    (NULL == CU_add_test(test_suite, "Test hash_table insert", test_hash_table_insert1)) ||
    (NULL == CU_add_test(test_suite, "Test hash_table insert2", test_hash_table_insert2)) ||
    (NULL == CU_add_test(test_suite, "Test hash_table insert3", test_hash_table_insert3)) ||
    (NULL == CU_add_test(test_suite, "Test hash_table lookup1", test_hash_table_lookup1)) ||
    (NULL == CU_add_test(test_suite, "Test hash_table lookup2", test_hash_table_lookup2)) ||
    (NULL == CU_add_test(test_suite, "Test hash_table remove", test_hash_table_remove)) ||
    (NULL == CU_add_test(test_suite, "Test hash_table size", test_hash_table_size)) ||
    (NULL == CU_add_test(test_suite, "Test hash_table empty", test_hash_table_empty)) ||
    (NULL == CU_add_test(test_suite, "Test hash_table clear", test_hash_table_clear)) ||
    (NULL == CU_add_test(test_suite, "Test hash_table has_key1", test_hash_table_has_key1)) ||
    (NULL == CU_add_test(test_suite, "Test hash_table has_value1", test_hash_table_has_value1)) ||
    (NULL == CU_add_test(test_suite, "Test hash_table has_value_all", test_hash_table_all_values)) ||
    (NULL == CU_add_test(test_suite, "Test hash_table apply_to_all", test_ioopm_hash_table_apply_to_all)) ||
    (NULL == CU_add_test(test_suite, "Test linked_list append", test_linked_list_append)) ||
    (NULL == CU_add_test(test_suite, "Test linked_list prepend", test_linked_list_prepend)) ||
    (NULL == CU_add_test(test_suite, "Test linked_list get", test_linked_list_get)) ||
    (NULL == CU_add_test(test_suite, "Test linked_list insert", test_linked_list_insert)) ||
    (NULL == CU_add_test(test_suite, "Test linked_list remove", test_linked_list_remove)) ||
    (NULL == CU_add_test(test_suite, "Test linked_list contains", test_linked_list_contains)) ||
    (NULL == CU_add_test(test_suite, "Test linked_list size", test_linked_list_size)) ||
    (NULL == CU_add_test(test_suite, "Test linked_list is_empty", test_linked_list_is_empty)) ||
    (NULL == CU_add_test(test_suite, "Test linked_list iterator", test_linked_list_iterator))
  )
  {
    CU_cleanup_registry();
    return CU_get_error();
  }
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}