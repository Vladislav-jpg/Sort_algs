#include <stdio.h>
#include "sort.h"

/* -------------------------------------------------------
 * Структура с двумя полями для демонстрации.
 * ------------------------------------------------------- */
typedef struct {
    int    id;
    double value;
} item_t;

/* Сравнение int */
int comp_int(void *a, void *b)
{
    int n1 = *((int *)a);
    int n2 = *((int *)b);  /* было n1<n1 — опечатка! */
    if (n1 < n2) return -1;
    if (n1 > n2) return  1;
    return 0;
}

/* Сравнение double */
int comp_double(void *a, void *b)
{
    double d1 = *((double *)a);
    double d2 = *((double *)b);
    if (d1 < d2) return -1;
    if (d1 > d2) return  1;
    return 0;
}

/* Сравнение структуры по полю value */
int comp_item(void *a, void *b)
{
    /* Было: оба указателя s1, оба приведены к a — ошибка! */
    item_t *s1 = (item_t *)a;
    item_t *s2 = (item_t *)b;
    if (s1->value < s2->value) return -1;
    if (s1->value > s2->value) return  1;
    return 0;
}

/* Передаём функцию сравнения в max() — пример из задания */
int max_elem(void *a, void *b, int (*compare)(void *, void *))
{
    /* compare возвращает 1, если a > b */
    if (compare(a, b) == 1) return 1; /* a больше */
    return 2;                          /* b больше или равны */
}

/* -------------------------------------------------------
 * Тест: прогон всех методов на массиве int
 * ------------------------------------------------------- */
int main(void)
{
    sort_method_t methods[] = {
        BUBBLE_SORT, INSERTION_SORT, SELECTION_SORT, COMB_SORT,
        SHELL_SORT,  QUICK_SORT,     MERGE_SORT,     HEAP_SORT
    };
    const char *names[] = {
        "bubble", "insertion", "selection", "comb",
        "shell",  "quick",     "merge",     "heap"
    };
    int nmethods = sizeof(methods) / sizeof(methods[0]);

    int ref[] = {1, 2, 3, 4, 5};

    for (int m = 0; m < nmethods; m++) {
        int arr[] = {5, 3, 1, 4, 2};
        int n = sizeof(arr) / sizeof(arr[0]);

        sort_set_method(methods[m]);
        sort_set_direction(SORT_ASC);
        int err = sort(arr, n, sizeof(int), comp_int);

        if (err != SORT_OK) {
            printf("FAIL method=%s err=%d\n", names[m], err);
            continue;
        }
        int ok = 1;
        for (int i = 0; i < n; i++) {
            if (arr[i] != ref[i]) { ok = 0; break; }
        }
        printf("%s: %s\n", names[m], ok ? "OK" : "FAIL");
    }

    /* Тест структуры */
    item_t items[] = {{3, 3.0}, {1, 1.0}, {2, 2.0}};
    sort_set_method(QUICK_SORT);
    sort_set_direction(SORT_DESC);
    sort(items, 3, sizeof(item_t), comp_item);
    printf("struct desc: %.0f %.0f %.0f (expect 3 2 1)\n",
           items[0].value, items[1].value, items[2].value);

    return 0;
}