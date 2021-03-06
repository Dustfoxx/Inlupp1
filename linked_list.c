#include "iterator.h"
#include "linked_list.h"
#include "common.h"

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

ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list)
{
    ioopm_list_iterator_t *iter = calloc(1, sizeof(ioopm_list_iterator_t));
    if(list->next)
    {
        iter->current = list->next;
        iter->next = list->next->next;
        iter->first = list->next;
        return iter;
    }
    iter->current = NULL;
    iter->next = NULL;
    iter->first = NULL;
    return iter;
}

ioopm_list_t *ioopm_linked_list_create(ioopm_eq_function eq)
{
    ioopm_list_t *list = calloc(1, sizeof(ioopm_list_t));
    list->list_eq = eq;
    return list;
}

void ioopm_linked_list_destroy(ioopm_list_t *list)
{
    assert(list);
    if(list->next) //If next pointer exists, recurse
        ioopm_linked_list_destroy(list->next);
    
    free(list); //After recursion free current pointer
}

void ioopm_linked_list_append(ioopm_list_t *list, elem_t value)
{
    assert(list);
    ioopm_list_t *iterator_ptr = list;
    while(iterator_ptr->next != NULL)
        iterator_ptr = iterator_ptr->next;
    iterator_ptr->next = calloc(1, sizeof(ioopm_list_t));
    iterator_ptr->next->value = value;
}

//Steps through a list recursively moving each value one step forward
static void recurse_prepend(ioopm_list_t *list)
{
    if(list->next)
        recurse_prepend(list->next); //Goes to next if it exists
    else
        list->next = calloc(1, sizeof(ioopm_list_t)); //Creates new space if none exists
    list->next->value = list->value;
}

void ioopm_linked_list_prepend(ioopm_list_t *list, elem_t value)
{
    recurse_prepend(list);
    list->next->value = value;
}

static ioopm_list_iterator_t *find_index(ioopm_list_t *list, int index)
{
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    
    for(int i = 0; i < index; ioopm_iterator_next(iter), i++)
    {
        if(iter->next == NULL)
        {
            ioopm_iterator_destroy(&iter);
            return NULL;
        }
    }
    return iter;
}

static ioopm_list_iterator_t *find_previous_index(ioopm_list_t *list, int index)
{
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

    for(int i = 0; i < index - 1; ioopm_iterator_next(iter), i++)
    {
        if(iter->next == NULL)
        {
            ioopm_iterator_destroy(&iter);
            return NULL;
        }
    }
    return iter;
}

bool ioopm_linked_list_insert(ioopm_list_t *list, int index, elem_t value)
{
    ioopm_list_iterator_t *chk_index = find_previous_index(list, index);
    bool return_val = false;
    if(chk_index)
    {
        recurse_prepend(chk_index->current);
        chk_index->current->next->value = value;
        return_val = true;
    }
    ioopm_iterator_destroy(&chk_index);
    return return_val;
}

elem_t ioopm_linked_list_remove(ioopm_list_t *list, int index)
{ //Returns zero when wrong
    elem_t return_val = (elem_t) {.int_value = 0, .uns_int = 0, .fl_value = 0, .pointer = NULL};

    ioopm_list_iterator_t *chk_index = NULL;

    if(index == 0)
    {
        chk_index = calloc(1, sizeof(ioopm_list_iterator_t));
        chk_index->current = list;
        chk_index->next = list->next;
        chk_index->first = list;
    }
    else
    {
        chk_index = find_previous_index(list, index);
    }
    if(chk_index)
    {
        return_val = chk_index->next->value;
        if(chk_index->next->next)
        {
            ioopm_list_t *new_ptr = chk_index->next->next;
            free(chk_index->next);
            chk_index->current->next = new_ptr;
        }
        else
        {
            free(chk_index->next);
            chk_index->current->next = NULL;
        }
    }
    
    ioopm_iterator_destroy(&chk_index);
    return return_val;
}

elem_t ioopm_linked_list_get(ioopm_list_t *list, int index)
{
    ioopm_list_iterator_t *found_index = find_index(list, index);

    if(found_index)
    {
        elem_t temp = found_index->current->value;
        ioopm_iterator_destroy(&found_index);
        return temp;
    }
    else
        return (elem_t) {.int_value = 0, .uns_int = 0, .fl_value = 0, .pointer = NULL};
}

bool ioopm_linked_list_contains(ioopm_list_t *list, elem_t element)
{
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    for(; ; ioopm_iterator_next(iter))
    {
        if(list->list_eq(iter->current->value, element))
        {
            ioopm_iterator_destroy(&iter);
            return true;
        }
        else if(iter->next == NULL)
        {
            ioopm_iterator_destroy(&iter);
            return false;
        }
    }
}

size_t ioopm_linked_list_size(ioopm_list_t *list)
{
    size_t i = 0;
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

void ioopm_linked_list_apply_to_all(ioopm_list_t *list, ioopm_apply_int_function fun, void *extra)
{
    for(; list->next; list = list->next)
    {
        fun(list->next->value, extra);
    }
}

ioopm_list_t *create_list(ioopm_eq_function eq)
{
    ioopm_list_t *list = ioopm_linked_list_create(eq);
    for(int i = 0; i < 15; i++)
        ioopm_linked_list_append(list, (elem_t) {.int_value = i} );
    return list;
}

//----------------Iterator---------------------------------------------

bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter)
{
    return iter->next;
}

elem_t ioopm_iterator_next(ioopm_list_iterator_t *iter)
{
    if(ioopm_iterator_has_next(iter))
    {
        ioopm_list_t *new_next = iter->next->next;
        iter->current = iter->next;
        iter->next = new_next;
    }
    return iter->current->value;
}

void ioopm_iterator_reset(ioopm_list_iterator_t *iter)
{
    ioopm_list_t *new_next = iter->first->next;
    iter->current = iter->first;
    iter->next = new_next;
}

elem_t ioopm_iterator_current(ioopm_list_iterator_t *iter)
{
    return iter->current->value;
}

void ioopm_iterator_destroy(ioopm_list_iterator_t **iter)
{
    free(*iter);
    *iter = NULL;
}

//-----------------------------------------------------------------------