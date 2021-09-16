#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"


ioopm_hash_table_t *ioopm_hash_table_create()
{
    return NULL;
}
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{}

char *ioopm_hash_table_lookup(ioopm_hash_table_t *ht, int key)
{
    return NULL;
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

void test_lookup()
{
   ioopm_hash_table_t *ht = ioopm_hash_table_create();
   for (int i = 0; i < 18; ++i) /// 18 is a bit magical
     {
       CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, i));
     }
   CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, -1));
   ioopm_hash_table_destroy(ht);
}

int main()
{
    test_create_destroy();
}