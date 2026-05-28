#include "sort.h"
#include <string.h>   /* memcpy */
#include <stdlib.h>   /* malloc, free */

/* -------------------------------------------------------
 * Закрытые (private) параметры.
 * static — видны только внутри этого файла.
 * Пользователь не может их изменить напрямую.
 * ------------------------------------------------------- */
static sort_method_t    current_method    = BUBBLE_SORT;
static sort_direction_t current_direction = SORT_ASC;

/* -------------------------------------------------------
 * Вспомогательная функция: пересылка элемента.
 * Копирует elemsize байт из src в dst.
 * ------------------------------------------------------- */
static void elem_move(void *dst, const void *src, int elemsize)
{
    memcpy(dst, src, elemsize);
}

/* -------------------------------------------------------
 * Вспомогательная функция: обмен двух элементов.
 * Использует временный буфер на стеке (до 256 байт)
 * или heap для больших элементов.
 * ------------------------------------------------------- */
static void elem_swap(void *a, void *b, int elemsize)
{
    char tmp[256];
    char* buf;
    if (elemsize <= 256) {
        buf = tmp;
    }
    else {
        buf = (char *)malloc(elemsize);
    }
    if (buf == NULL) {
        return;
    }
    memcpy(buf, a, elemsize);
    memcpy(a,   b, elemsize);
    memcpy(b, buf, elemsize);
    if (buf != tmp) {
        free(buf);
    }
}

/* -------------------------------------------------------
 * Вспомогательная функция: получить указатель на i-й элемент.
 * Арифметика на void* запрещена стандартом C —
 * приводим к char* (1 байт), потом умножаем на elemsize.
 * ------------------------------------------------------- */
static char *elem_at(void *array, int i, int elemsize)
{
    return (char *)array + i * elemsize;
}

/* -------------------------------------------------------
 * Обёртка над пользовательской функцией сравнения.
 * Инвертирует результат при SORT_DESC.
 * ------------------------------------------------------- */
static int cmp(void *a, void *b, int (*comp)(void *, void *))
{
    int result = comp(a, b);
    if (current_direction == SORT_DESC) result = -result;
    return result;
}

/* =====================================================
 * РЕАЛИЗАЦИИ МЕТОДОВ СОРТИРОВКИ (все static — закрыты)
 * ===================================================== */

/* Простой обмен (пузырёк) */
static int bubble_sort(void *arr, int n, int es,
                       int (*comp)(void *, void *))
{
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            void *a = elem_at(arr, j,     es);
            void *b = elem_at(arr, j + 1, es);
            if (cmp(a, b, comp) > 0) {
                elem_swap(a, b, es);
            }
        }
    }
    return SORT_OK;
}

/* Простая вставка */
static int insertion_sort(void *arr, int n, int es,
                          int (*comp)(void *, void *))
{
    char *tmp = (char *)malloc(es);
    if (tmp == NULL) return SORT_ERR_NULL;

    for (int i = 1; i < n; i++) {
        elem_move(tmp, elem_at(arr, i, es), es);
        int j = i - 1;
        while (j >= 0 && cmp(elem_at(arr, j, es), tmp, comp) > 0) {
            elem_move(elem_at(arr, j + 1, es), elem_at(arr, j, es), es);
            j--;
        }
        elem_move(elem_at(arr, j + 1, es), tmp, es);
    }
    free(tmp);
    return SORT_OK;
}

/* Простой выбор */
static int selection_sort(void *arr, int n, int es,
                          int (*comp)(void *, void *))
{
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (cmp(elem_at(arr, j, es), elem_at(arr, min_idx, es), comp) < 0) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            elem_swap(elem_at(arr, i, es), elem_at(arr, min_idx, es), es);
        }
    }
    return SORT_OK;
}

/* Расчёска */
static int comb_sort(void *arr, int n, int es,
                     int (*comp)(void *, void *))
{
    int gap = n;
    int sorted = 0;
    while (!sorted) {
        gap = (gap * 10) / 13;
        if (gap <= 1) { gap = 1; sorted = 1; }
        for (int i = 0; i + gap < n; i++) {
            void *a = elem_at(arr, i,        es);
            void *b = elem_at(arr, i + gap,  es);
            if (cmp(a, b, comp) > 0) {
                elem_swap(a, b, es);
                sorted = 0;
            }
        }
    }
    return SORT_OK;
}

/* Шелл */
static int shell_sort(void *arr, int n, int es,
                      int (*comp)(void *, void *))
{
    char *tmp = (char *)malloc(es);
    if (tmp == NULL) return SORT_ERR_NULL;

    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            elem_move(tmp, elem_at(arr, i, es), es);
            int j = i;
            while (j >= gap &&
                   cmp(elem_at(arr, j - gap, es), tmp, comp) > 0) {
                elem_move(elem_at(arr, j, es), elem_at(arr, j - gap, es), es);
                j -= gap;
            }
            elem_move(elem_at(arr, j, es), tmp, es);
        }
    }
    free(tmp);
    return SORT_OK;
}

/* Быстрая сортировка — вспомогательный раздел */
static int partition(void *arr, int lo, int hi, int es,
                     int (*comp)(void *, void *))
{
    void *pivot = elem_at(arr, hi, es);
    int i = lo - 1;
    for (int j = lo; j < hi; j++) {
        if (cmp(elem_at(arr, j, es), pivot, comp) <= 0) {
            i++;
            elem_swap(elem_at(arr, i, es), elem_at(arr, j, es), es);
        }
    }
    elem_swap(elem_at(arr, i + 1, es), elem_at(arr, hi, es), es);
    return i + 1;
}

static void quick_sort_rec(void *arr, int lo, int hi, int es,
                           int (*comp)(void *, void *))
{
    if (lo < hi) {
        int p = partition(arr, lo, hi, es, comp);
        quick_sort_rec(arr, lo,     p - 1, es, comp);
        quick_sort_rec(arr, p + 1,  hi,    es, comp);
    }
}

static int quick_sort(void *arr, int n, int es,
                      int (*comp)(void *, void *))
{
    quick_sort_rec(arr, 0, n - 1, es, comp);
    return SORT_OK;
}

/* Слияние */
static void merge(void *arr, int lo, int mid, int hi, int es,
                  int (*comp)(void *, void *))
{
    int ln = mid - lo + 1;
    int rn = hi - mid;
    char *L = (char *)malloc(ln * es);
    char *R = (char *)malloc(rn * es);
    if (L == NULL || R == NULL) { free(L); free(R); return; }

    memcpy(L, elem_at(arr, lo,      es), ln * es);
    memcpy(R, elem_at(arr, mid + 1, es), rn * es);

    int i = 0, j = 0, k = lo;
    while (i < ln && j < rn) {
        if (cmp(L + i * es, R + j * es, comp) <= 0) {
            elem_move(elem_at(arr, k++, es), L + i++ * es, es);
        } else {
            elem_move(elem_at(arr, k++, es), R + j++ * es, es);
        }
    }
    while (i < ln) elem_move(elem_at(arr, k++, es), L + i++ * es, es);
    while (j < rn) elem_move(elem_at(arr, k++, es), R + j++ * es, es);

    free(L);
    free(R);
}

static void merge_sort_rec(void *arr, int lo, int hi, int es,
                           int (*comp)(void *, void *))
{
    if (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        merge_sort_rec(arr, lo,      mid, es, comp);
        merge_sort_rec(arr, mid + 1, hi,  es, comp);
        merge(arr, lo, mid, hi, es, comp);
    }
}

static int merge_sort(void *arr, int n, int es,
                      int (*comp)(void *, void *))
{
    merge_sort_rec(arr, 0, n - 1, es, comp);
    return SORT_OK;
}

/* Пирамидальная (куча) */
static void heapify(void *arr, int n, int i, int es,
                    int (*comp)(void *, void *))
{
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < n && cmp(elem_at(arr, l, es), elem_at(arr, largest, es), comp) > 0)
        largest = l;
    if (r < n && cmp(elem_at(arr, r, es), elem_at(arr, largest, es), comp) > 0)
        largest = r;
    if (largest != i) {
        elem_swap(elem_at(arr, i, es), elem_at(arr, largest, es), es);
        heapify(arr, n, largest, es, comp);
    }
}

static int heap_sort(void *arr, int n, int es,
                     int (*comp)(void *, void *))
{
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i, es, comp);
    for (int i = n - 1; i > 0; i--) {
        elem_swap(elem_at(arr, 0, es), elem_at(arr, i, es), es);
        heapify(arr, i, 0, es, comp);
    }
    return SORT_OK;
}

/* =====================================================
 * СЕТТЕРЫ — единственный способ изменить параметры
 * ===================================================== */

/**
 * sort_set_method — устанавливает метод сортировки.
 * Возвращает SORT_OK или SORT_ERR_METHOD при неверном значении.
 */
int sort_set_method(sort_method_t method)
{
    switch (method) {
        case BUBBLE_SORT:
        case INSERTION_SORT:
        case SELECTION_SORT:
        case COMB_SORT:
        case SHELL_SORT:
        case QUICK_SORT:
        case MERGE_SORT:
        case HEAP_SORT:
            current_method = method;
            return SORT_OK;
        default:
            return SORT_ERR_METHOD;
    }
}

/**
 * sort_set_direction — устанавливает направление сортировки.
 * Возвращает SORT_OK или SORT_ERR_DIRECTION при неверном значении.
 */
int sort_set_direction(sort_direction_t direction)
{
    if (direction == SORT_ASC || direction == SORT_DESC) {
        current_direction = direction;
        return SORT_OK;
    }
    return SORT_ERR_DIRECTION;
}

/* =====================================================
 * ГЛАВНАЯ ИНТЕРФЕЙСНАЯ ФУНКЦИЯ
 * ===================================================== */

/**
 * sort — единственная публичная функция сортировки.
 *
 * array    — сортируемый массив
 * size     — количество элементов
 * elemsize — размер одного элемента в байтах
 * comp     — функция сравнения: возвращает -1, 0 или 1
 *
 * Возвращает код ошибки (SORT_OK = 0 при успехе).
 */
int sort(void *array, int size, int elemsize,
         int (*comp)(void *a1, void *a2))
{
    if (array == NULL || comp == NULL) return SORT_ERR_NULL;
    if (size <= 0 || elemsize <= 0)    return SORT_ERR_SIZE;
    if (size == 1) return SORT_OK; /* уже отсортировано */

    switch (current_method) {
        case BUBBLE_SORT:    return bubble_sort   (array, size, elemsize, comp);
        case INSERTION_SORT: return insertion_sort(array, size, elemsize, comp);
        case SELECTION_SORT: return selection_sort(array, size, elemsize, comp);
        case COMB_SORT:      return comb_sort     (array, size, elemsize, comp);
        case SHELL_SORT:     return shell_sort    (array, size, elemsize, comp);
        case QUICK_SORT:     return quick_sort    (array, size, elemsize, comp);
        case MERGE_SORT:     return merge_sort    (array, size, elemsize, comp);
        case HEAP_SORT:      return heap_sort     (array, size, elemsize, comp);
        default:             return SORT_ERR_METHOD;
    }
}