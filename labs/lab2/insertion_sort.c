#include <stdio.h>
#define ARRAY_SIZE 10
void main()
{
    int a[ARRAY_SIZE], i, j, key;
    printf("enter 10 array elements: ");
    for (i = 0; i < ARRAY_SIZE; i++)
    {
        scanf("%d", &a[i]);
    }

    printf("The given array is: ");
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        printf("%d ", a[i]);
    }

    // insertion sort
    for (i = 1; i < ARRAY_SIZE; i++)
    {
        key = a[i];
        j = i - 1;
        while (j >= 0 && key <= a[j])
        {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }

    printf("\nThe sorted array is: ");
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        printf("%d ", a[i]);
    }
}