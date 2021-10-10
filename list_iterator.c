#include "iterator.h"

bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter)
{
    return iter->next;
}

int ioopm_iterator_next(ioopm_list_iterator_t *iter)
{//Needs check if there is no next
    if(ioopm_iterator_has_next(iter))
    {
        ioopm_list_t *new_next = iter->next->next;
        iter->current = iter->next;
        iter->next = new_next;
        return iter->current->value;
    }
    return 0;
}

void ioopm_iterator_reset(ioopm_list_iterator_t *iter)
{
    ioopm_list_t *new_next = iter->first->next;
    iter->current = iter->first;
    iter->next = new_next;
}

int ioopm_iterator_current(ioopm_list_iterator_t *iter)
{
    return iter->current->value;
}

void ioopm_iterator_destroy(ioopm_list_iterator_t *iter)
{
    free(iter);
    iter = NULL;
}

static void test_iter()
{
    ioopm_list_t *list = create_list();
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

    if(ioopm_iterator_has_next(iter))
        puts("Found next");

    int val1 = ioopm_iterator_next(iter);
    int val2 = ioopm_iterator_next(iter);
    printf("%d, %d\n", val1, val2);
    ioopm_iterator_reset(iter);
    printf("%d\n", ioopm_iterator_current(iter));

    ioopm_iterator_destroy(iter);

    ioopm_linked_list_destroy(list);
}

int main()
{
    test_iter();
}