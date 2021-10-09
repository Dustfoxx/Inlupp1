#include "linked_list.h"
#include <stdlib.h>
#include <stdio.h>

ioopm_list_t *ioopm_linked_list_create(void)
{
    return calloc(1, sizeof(ioopm_list_t));
}

void ioopm_linked_list_destroy(ioopm_list_t *list)
{
    if(list->next) //If next pointer exists, recurse
        ioopm_linked_list_destroy(list->next);
    
    free(list); //After recursion free current pointer
}

void ioopm_linked_list_append(ioopm_list_t *list, int value)
{
    ioopm_list_t *iterator_ptr = list;
    while(iterator_ptr->next)
        iterator_ptr = iterator_ptr->next;
    iterator_ptr->next = malloc(sizeof(ioopm_list_t));
    iterator_ptr->next->value = value;
}

//Steps through a list recursively moving each value one step forward
static void recurse_prepend(ioopm_list_t *list)
{
    if(list->next)
        recurse_prepend(list->next); //Goes to next if it exists
    else
        list->next = malloc(sizeof(ioopm_list_t)); //Creates new space if none exists
    list->next->value = list->value;
}

void ioopm_linked_list_prepend(ioopm_list_t *list, int value)
{
    recurse_prepend(list);
    list->next->value = value;
}

static list_index_t find_index(ioopm_list_t *list, int index)
{
    int i = 0;
    for(; i < index && list->next; i++)
        list = list->next;
    if(i == index)
    {
        return Success(list);
    }
    else
        return Failure();
}

void ioopm_linked_list_insert(ioopm_list_t *list, int index, int value)
{
    list_index_t chk_index = find_index(list, index);
    if(Successful(chk_index))
    {
        recurse_prepend(chk_index.ptr);
        chk_index.ptr->next->value = value;
    }
    else
        puts("Need error here, wrong index");
}

int ioopm_linked_list_remove(ioopm_list_t *list, int index)
{ //Returns zero when wrong
    int return_val = 0;
    list_index_t chk_index = find_index(list, index);
    if(Successful(chk_index))
    {
        return_val = chk_index.ptr->next->value;
        if(chk_index.ptr->next->next)
        {
            ioopm_list_t *new_ptr = chk_index.ptr->next->next;
            free(chk_index.ptr->next);
            chk_index.ptr->next = new_ptr;
        }
        else
        {
            free(chk_index.ptr->next);
            chk_index.ptr->next = NULL;
        }
    }
    else
        puts("Need error here, wrong index");
    
    return return_val;
}

int ioopm_linked_list_get(ioopm_list_t *list, int index)
{
    list_index_t found_index = find_index(list, index);

    if(Successful(found_index))
    {
        return found_index.ptr->next->value;
    }
    else
        return 0;
}

bool ioopm_linked_list_contains(ioopm_list_t *list, int element)
{
    for(; list->next; list = list->next)
    {
        if(list->next->value == element)
            return true;
    }
    return false;
}

int ioopm_linked_list_size(ioopm_list_t *list)
{
    int i = 0;
    for(; list->next; list = list->next, i++){}
    return i;
}

bool ioopm_linked_list_is_empty(ioopm_list_t *list)
{
    return !list->next;
}

void ioopm_linked_list_clear(ioopm_list_t *list)
{
    if(list->next) //If next pointer exists, recurse
        ioopm_linked_list_destroy(list->next);
    list->next = NULL;
}

bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_int_predicate prop, void *extra)
{
    bool result = true;
    for(; list->next && result; list = list->next)
    {
        result = prop(list->next->value, extra);
    }
    return result;
}

bool ioopm_linked_list_any(ioopm_list_t *list, ioopm_int_predicate prop, void *extra)
{
    for(; list->next; list = list->next)
    {
        if(prop(list->next->value, extra))
            return true;  
    }
    return false;
}

bool dividable_by(int value, void *div)
{
    int *division = div;
    return !(value % *division);
}

void ioopm_linked_list_apply_to_all(ioopm_list_t *list, ioopm_apply_int_function fun, void *extra)
{
    for(; list->next; list = list->next)
    {
        fun(list->next->value, extra);
    }
}

void print_key(int key, void *extra)
{
    printf("%d\n", key);
}

static ioopm_list_t *create_list()
{
    ioopm_list_t *list = ioopm_linked_list_create();
    for(int i = 0; i < 15; i++)
        ioopm_linked_list_append(list, i);
    return list;
}

static void print_list(ioopm_list_t *list)
{
    while(list)
    {
        printf("%d\n", list->value);
        list = list->next;
    }
}

//-----------------------------------------------------------------------
static void test_append()
{
    ioopm_list_t *list = ioopm_linked_list_create();
    ioopm_linked_list_append(list, 4);
    ioopm_linked_list_append(list, 7);
    printf("%d\n", list->next->value);
    printf("%d\n", list->next->next->value);
    ioopm_linked_list_destroy(list);
}

static void test_prepend()
{
    ioopm_list_t *list = ioopm_linked_list_create();
    ioopm_linked_list_append(list, 4);
    ioopm_linked_list_append(list, 7);
    ioopm_linked_list_prepend(list, 3);
    printf("%d\n", list->next->value);
    printf("%d\n", list->next->next->value);
    printf("%d\n", list->next->next->next->value);
    ioopm_linked_list_destroy(list);
}

static void test_insert()
{
    ioopm_list_t *list = create_list();

    ioopm_linked_list_insert(list, 5, 54);
    ioopm_linked_list_insert(list, 14, 546);
    ioopm_linked_list_insert(list, 123123, 123);
    print_list(list);
    ioopm_linked_list_destroy(list);
}

static void test_remove()
{
    ioopm_list_t *list = create_list();

    ioopm_linked_list_remove(list, 5);
    ioopm_linked_list_remove(list, 1414);
    print_list(list);
    ioopm_linked_list_destroy(list);
}

static void test_get()
{
    ioopm_list_t *list = create_list();

    int found_val = ioopm_linked_list_get(list, 5);
    ioopm_linked_list_get(list, 1414);
    printf("%d\n", found_val);
    ioopm_linked_list_destroy(list);
}

static void test_contain()
{
    ioopm_list_t *list = create_list();

    if(ioopm_linked_list_contains(list, 4) && !ioopm_linked_list_contains(list, 41))
        puts("Yeee");
    else
        puts("Noooo");
    ioopm_linked_list_destroy(list);
}

static void test_size()
{
    ioopm_list_t *list = create_list();

    printf("%d\n", ioopm_linked_list_size(list));
    ioopm_linked_list_destroy(list);
}

static void test_clear()
{
    ioopm_list_t *list = create_list();

    ioopm_linked_list_clear(list);
    if(ioopm_linked_list_is_empty(list))
        puts("I's cleard");
    ioopm_linked_list_append(list, 23);
    print_list(list);

    ioopm_linked_list_destroy(list);
}

static void test_check()
{
    ioopm_list_t *list = create_list();
    int test_val1 = 1;
    int test_val2 = 2;

    if(ioopm_linked_list_all(list, dividable_by, &test_val1))
        puts("all worked");
    if(ioopm_linked_list_any(list, dividable_by, &test_val2))
        puts("any worked");

    ioopm_linked_list_destroy(list);
}

static void test_apply()
{
    ioopm_list_t *list = create_list();

    ioopm_linked_list_apply_to_all(list, print_key, (void *) {NULL});

    ioopm_linked_list_destroy(list);
}

int main()
{
    //test_append();
    //test_prepend();
    //test_insert();
    //test_remove();
    //test_get();
    //test_contain();
    //test_size();
    //test_clear();
    test_check();
    test_apply();
}