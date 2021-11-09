#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"
#include <CUnit/CUnit.h>

void test_create_destroy()
{
   ioopm_hash_table_t *ht = ioopm_hash_table_create();
   CU_ASSERT_PTR_NOT_NULL(ht);
   ioopm_hash_table_destroy(ht);
}

int main()
{
    test_create_destroy();
}