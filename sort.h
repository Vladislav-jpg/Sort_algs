#ifndef SORT_H
#define SORT_H

/* Коды ошибок */
#define SORT_OK             0
#define SORT_ERR_NULL       1   /* NULL-указатель */
#define SORT_ERR_SIZE       2   /* size <= 0 или elemsize <= 0 */
#define SORT_ERR_METHOD     3   /* неизвестный метод */
#define SORT_ERR_DIRECTION  4   /* неизвестное направление */

/* Методы сортировки */
typedef enum {
    BUBBLE_SORT    = 1,
    INSERTION_SORT = 2,
    SELECTION_SORT = 3,
    COMB_SORT      = 4,
    SHELL_SORT     = 5,
    QUICK_SORT     = 6,
    MERGE_SORT     = 7,
    HEAP_SORT      = 8
} sort_method_t;

/* Направление сортировки */
typedef enum {
    SORT_ASC  = 1,
    SORT_DESC = 2
} sort_direction_t;

/* Сеттеры — единственный способ изменить параметры */
int sort_set_method(sort_method_t method);
int sort_set_direction(sort_direction_t direction);

/* Главная интерфейсная функция */
int sort(void *array, int size, int elemsize,
         int (*comp)(void *a1, void *a2));

#endif /* SORT_H */