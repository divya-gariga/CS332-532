#include<stdio.h>
#include <stdbool.h>

bool isEven(int n){
    if(n%2==0)
       return true;
    else
        return false;
}

int main()
{
    int given_number;
    printf("enter a number: ");
    scanf("%d",&given_number);
    if(isEven(given_number))
        printf("The number is an even number");
    else
        printf("The number is an odd number");
    return 0;
}