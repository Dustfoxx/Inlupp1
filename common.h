#pragma once

#include <stdlib.h>
#include <stdbool.h>

typedef struct elem elem_t;
typedef struct option option_t;

typedef bool(*ioopm_eq_function)(elem_t a, elem_t b);

#define Success(v)      (option_t) { .success = true, .value = v };
#define Failure()       (option_t) { .success = false };
#define Successful(o)   (o.success == true)
#define Unsuccessful(o) (o.success == false)

struct elem
{
    int int_value;
    unsigned int uns_int;
    float fl_value;
    void *pointer;
};

struct option
{
  bool success;
  elem_t value;
};