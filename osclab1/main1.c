#include <stdio.h>

int main()
{
    printf("Size of int: %lu bytes\n", sizeof(int));
    printf("Size of float: %lu bytes\n", sizeof(float));
    printf("Size of double: %lu bytes\n", sizeof(double));
    printf("Size of void: %lu bytes\n", sizeof(void *)); // void pointers
    printf("Size of pointer: %lu bytes\n", sizeof(void *));

    printf("Size of short int: %lu bytes\n", sizeof(short int));
    printf("Size of long int: %lu bytes\n", sizeof(long int));

    if ((char)-1 < 0)
    {
        printf("char is signed\n");
    }
    else
    {
        printf("char is unsigned\n");
    }

    return 0;
}
