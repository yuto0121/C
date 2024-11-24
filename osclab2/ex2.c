#include <stdio.h>

// Function to swap the addresses of two integer pointers
void swap_pointers(int **p, int **q)
{
    int *temp = *p; // Store the address pointed by p
    *p = *q;        // Change the address of p to that of q
    *q = temp;      // Change the address of q to the original address of p
}

int main()
{
    int a = 1;
    int b = 2;
    // for testing we use pointers to integers
    int *p = &a;
    int *q = &b;

    printf("Address of p = %p and q = %p\n", (void *)p, (void *)q);

    // Call swap_pointers with the addresses of the pointers p and q
    swap_pointers(&p, &q);

    printf("Address of p = %p and q = %p\n", (void *)p, (void *)q);

    // Optional: print the values pointed by p and q
    printf("Value pointed by p = %d and q = %d\n", *p, *q);

    return 0;
}
