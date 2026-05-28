#include <stdio.h>
#include "sort.h"
int compint(void* a, void* b) {
    int a1 = *(int*)a;
    int a2 = *(int*)b;
    if (a1 < a2) return -1;
    if (a1 > a2) return 1;
    return 0;
}
int compdouble(void* a, void* b) {
    double a1 = *(double*)a;
    double a2 = *(double*)b;
    if (a1 < a2) return -1;
    if (a1 > a2) return 1;
    return 0;
}
typedef struct student {
    int course;
    int grade;
} student_t;
int comp_student(void* a, void* b) {
    student_t* student1 = (student_t*)a;
    student_t* student2 = (student_t*)b;
    if (student1->grade > student2->grade) return 1;
    if (student1->grade < student2->grade) return -1;
    return 0;
}
int main() {
    sort_method_t methods[] = {BUBBLE_SORT, INSERTION_SORT, SELECTION_SORT, COMB_SORT, SHELL_SORT,
        QUICK_SORT, MERGE_SORT, HEAP_SORT};
    for (int i = 0; i <= 7; i++) {
        /*Тест массива Double*/
        //printf("%i\n", sort_set_method(methods[i]));
        sort_set_direction(SORT_ASC);
        sort_set_method(methods[i]);
        double doublemass[] = {3.7, 6.626, 67.23, 0.000142, 42.12, 11.52, -1523.24};
        int size = sizeof(doublemass) / sizeof(doublemass[0]);
        if (sort(doublemass, size, sizeof(doublemass[0]), compdouble) != SORT_OK) {
            printf("Sort failed DOUBLE\n");
            printf("%i", methods[i]);
        }
        else {
            printf("OK DOUBLE\n");
        }
        /*Тест INT*/
        int intmass[] = {3, 6, 67, 0, 42, 11, -1523, 12};
        sort_set_direction(SORT_DESC);
        size = sizeof(intmass) / sizeof(intmass[0]);
        if (sort(intmass, size, sizeof(intmass[0]), compint) != 0) {
            printf("Sort failed INT\n");
        }
        else {
            printf("OK INT\n");
        }
        /*Тест структуры*/
        student_t students[] = {{1, 2}, {42, 14}, {12, 24}, {41, 2}, {3, 2}, {0, -12}, {-21, 4}};
        sort_set_direction(SORT_ASC);
        size = sizeof(students) / sizeof(student_t);
        if (sort(students, size, sizeof(student_t), comp_student) != 0) {
            printf("Sort failed STUDENT\n");
        }
        else {
            printf("OK STUDENT\n");
        }
    }
    return 0;
}