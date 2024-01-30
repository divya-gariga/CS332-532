#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_STRING_LENGTH 100

// Function to read strings from the user
char **readArrayofStrings(int n)
{
    char **stringArray = (char **)malloc(n * sizeof(char *));
    char stringBuffer[MAX_STRING_LENGTH];
    for (int i = 0; i < n; i++)
    {
        printf("\nenter string %d :",i+1);
        scanf("%s", stringBuffer);
        stringArray[i] = (char *)malloc((strlen(stringBuffer) + 1) * sizeof(char));
        strcpy(stringArray[i], stringBuffer);
    }
    return stringArray;
}

// Function to perform insertion sort
void insertionSort(char **stringArray, int n)
{
    for (int i = 1; i < n; i++)
    {
        char *key = stringArray[i];
        int j = i - 1;
        while (j >= 0 && strcmp(key, stringArray[j]) < 0)
        {
            stringArray[j + 1] = stringArray[j];
            j--;
        }
        stringArray[j + 1] = key;
    }
}

// Function to display sorted string array
void displayStrings(char **stringArray, int n)
{
    printf("\nSorted String:");
    for (int i = 0; i < n; i++)
    {
        printf("%s ", stringArray[i]);
    }
}

// Function to free memory
void freeStrings(char** stringArray, int n) {
    for (int i = 0; i < n; i++) {
        free(stringArray[i]);
    }
    free(stringArray);
}

void main()
{
    int n;

    // Enter the number of strings
    printf("Enter the number of strings: ");
    scanf("%d", &n);

    // Read the strings
    char **arrayOfStrings = readArrayofStrings(n);

    // Sort the strings
    insertionSort(arrayOfStrings, n);

    // display sorted string array
    displayStrings(arrayOfStrings, n);

    // Free memory
    freeStrings(arrayOfStrings,n);
}