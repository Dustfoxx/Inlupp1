This module is made to enable you to store any type of data in a hash table.
It is stored using a typing unique to this module called "elem_t". The definition for the typing can be found in
common.h.

elem_t is a struct containing an int, unsigned int, float and a void pointer that can be used for anything.
Please note that when using the void pointer you will need to translate it yourself into the type you want.
For example the function ioopm_hash_table_lookup returns an option_t type (Which can also be found in common.h)
which contains an elem_t. So if you've stored a string within elem_t you will need to extract it by casting the pointer.
char *retreived = ioopm_hash_table_lookup(hash_table, i).value.pointer;

hash_table.h contains all functions needed to properly modify and use the hash_table and some internal functionalities
can be found in linked_list.h but if you're only using it for the hash table that information isn't needed.

The hash_table needs to be destroyed using ioopm_hash_table_destroy after each creation. If you have any data stored
that you yourself has allocated memory for we recommend that you use the function ioopm_hash_table_apply_to_all with
your own written function to remove all data before using destroy as the data will otherwise be lost and subsequently 
cause a leak.


Performance tests:
    -Test on document with long words 1k.
        0 seconds

        Top three:
        string_eq
        find_previous_entry_for_key
        string_sum_hash

    -Test on document with 10k words
        0 seconds

        Top three:
        ioopm_iterator_has_next
        ioopm_iterator_next
        string_eq

    - Test on document with 16k words
        0 seconds
        ioopm_iterator_has_next
        ioopm_iterator_next
        string_eq

    All of the top three are our own code
    The top three aren't equal. When the hash_table has more entries it needs to iterate more. The extreme change between executions of the top two functions in 10k and 16k implies a great increase in hash positions.
    If we limit sizes of buckets and use that to increase bucket amounts as well we may need to iterate less and so make it go faster.