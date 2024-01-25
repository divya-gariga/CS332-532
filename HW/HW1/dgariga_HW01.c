#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

bool isDivisibleBy3(int n)
{
    int sum = 0;
    while (n != 0)
    {
        sum += n % 10;
        n /= 10;
    }

    return (sum % 3 == 0) ? true : false;
}

bool isDivisibleBy5(int n)
{
    int lastDigit = n % 10;
    return (lastDigit == 0 || lastDigit == 5) ? true : false;
}

char *squareOfSum(int n)
{
    int sum = 0;
    while (n != 0)
    {
        sum += n % 10;
        n /= 10;
    }
    int square = sum * sum;
    char *resultString = (char *)malloc(50);
    snprintf(resultString, 50, "%d", square);
    return resultString;
}

bool isPowerOf2(int n)
{
    if (n == 0)
        return false;
    while (n != 1)
    {
        if (n % 2 == 0)
            n = n / 2;
        else
            return false;
    }
    return true;
}

bool isPrime(int n)
{
    if (n <= 1)
        return false;
    for (int i = 2; i * i <= n; i++)
    {
        if (n % i == 0)
            return false;
    }
    return true;
}

int nearestPrime(int n)
{
    for (int i = n - 1; i >= 2; i--)
    {
        if (isPrime(i))
            return i;
    }
    return 0;
}

char *numberTransformer(int n)
{
    int result;
    if (n != 3 && n != 5 && isPrime(n))
        return "Go Blazers";
    else if (isPowerOf2(n))
    {
        result = nearestPrime(n) + n;
        char *resultString = (char *)malloc(50);
        snprintf(resultString, 50, "%d", result);
        return resultString;
    }
    else if (isDivisibleBy3(n) && isDivisibleBy5(n))
        return "UAB CS 332&532";
    else if (isDivisibleBy3(n))
        return "CS";
    else
        return squareOfSum(n);
}

bool UABNumber()
{
    int n2, sum = 0;
    printf("\nenter an integer to check for UAB Number: ");
    scanf("%d", &n2);

    if (n2 == 0)
        return false;

    // calculate divisors
    for (int i = 1; i < n2; i++)
    {
        if (n2 % i == 0)
            sum += i;
    }
    if (sum == n2)
        return true;
    else
        return false;
}

int reverseNum(int n3)
{
    int reverse = 0;
    while (n3 > 0)
    {
        reverse = (reverse * 10) + (n3 % 10);
        n3 = n3 / 10;
    }
    return reverse;
}

int smallerThanIndex()
{
    int arr[5], length = 0, n = 0, count = 0;
    printf("\nenter 5 array numbers to check smaller than index: ");
    for (int i = 0; i < 5; i++)
    {
        scanf("%d", &arr[i]);
    }
    for (int i = 0; i < 5; i++)
    {
        if (arr[i] < i)
            count += 1;
    }
    return count;
}

void arrayDetails()
{
    int a[6], minValue, maxValue, minIndex = 0, maxIndex = 0, sum = 0, length = 0;
    double mean;
    length = sizeof(a) / sizeof(a[0]);

    printf("\nenter 6 array numbers for array details: ");
    for (int i = 0; i < length; i++)
    {
        scanf("%d", &a[i]);
    }
    minValue=maxValue=a[0];
    for (int i = 0; i < length; i++)
    {
        if (a[i] < minValue)
        {
            minIndex = i;
            minValue = a[i];
        }
        if (maxValue<a[i])
        {
            maxIndex = i;
            maxValue = a[i];
        }
        sum += a[i];
    }
    mean = (double)sum / length;
    printf("[ %d, %d, %.2lf, %d, %d]", minValue, minIndex, mean, maxValue, maxIndex);
}

int main()
{
    int n, n3;

    // question 1
    printf("enter an integer n for numberTransformer:");
    scanf("%d", &n);
    printf("%s", numberTransformer(n));

    // question 2
    UABNumber() ? printf("True") : printf("False");

    // question 3
    printf("\nenter a number to reverse: ");
    scanf("%d", &n3);
    printf("%d", reverseNum(n3));

    // question 4
    printf("%d", smallerThanIndex());

    // question 5
    arrayDetails();
    return 0;
}