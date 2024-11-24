// using heap instead of stack
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    short day, month;
    unsigned year;
} date_t;

date_t *date_struct(int day, int month, int year)
{
    date_t *dummy = (date_t *)malloc(sizeof(date_t));
    dummy->day = (short)day;
    dummy->month = (short)month;
    dummy->year = (unsigned)year;
    return dummy;
}

int main(void)
{
    int day, month, year;
    date_t *d;
    printf("\nGive day, month, year:");
    scanf("%d %d %d", &day, &month, &year);
    d = date_struct(day, month, year);
    printf("\ndate struct values: %d-%d-%d", d->day, d->month, d->year);
    free(d);
    return 0;
}
