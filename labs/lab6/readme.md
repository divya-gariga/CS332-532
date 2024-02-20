<!--
NOTES:
This is the README file for lab06
-->

# Compilation Instructions

```bash
gcc -o main main.c 
# this will compile the main.c file and return an executable with the executable name main.exe
```
# Run Instructions

```bash
.\main.exe
#in Windows run the executable as shown
```

# Code Description

# Functions
# Code Description

This C program reads data from a CSV file and organizes the information into a structure, and then performs sorting based on different criteria. Finally, it writes the sorted data to separate CSV files.

## Structure Definition

```c
struct listing {
    int id, host_id, minimum_nights, number_of_reviews, calculated_host_listings_count, availability_365;
    char *host_name, *neighbourhood_group, *neighbourhood, *room_type;
    float latitude, longitude, price;
};
```

Defines a structure `listing` to store information about a listing, such as its ID, host details, location, room type, and pricing.

## `getfields` Function

```c
struct listing getfields(char *line);
```

Parses a comma-separated line from the CSV file, extracts the values, and populates a `struct listing` with the data. It returns the populated structure.

## `displayStruct` Function

```c
void displayStruct(struct listing item);
```

Prints the values of a `struct listing` to the console.

## Sorting Comparison Functions

```c
static int cmpListingByHostName(const void *a, const void *b);
static int cmpListingByPrice(const void *a, const void *b);
```

Comparison functions used for sorting listings based on `host_name` and `price` criteria.

## `readData` Function

```c
void readData(struct listing *list_items, char *firstLine, int count)
```

  - Opens the file "listings.csv" for reading.
  - Reads lines from the file, skips the first line (assuming it's a header), and populates an array of `struct listing` named `list_items`.

## `sortAndWriteData` Function
```c
void sortAndWriteData(struct listing *list_items, int count, char *firstLine, int (*compareFunction)(const void *, const void *), const char *outputFileName);
```
The `sortAndWriteData` function takes an array of `struct listing` elements, the number of elements in the array (`count`), the header or first line of the input data (`firstLine`), a comparison function for sorting (`compareFunction`), and the name of the output file (`outputFileName`). The function uses the `qsort` function to sort the array based on the provided comparison function. It then opens a new file for writing, checks for errors in file creation, writes the header to the file, and iterates through the sorted array, writing each element in a formatted CSV line to the file. Finally, it closes the file, resulting in a modular and reusable function for sorting and writing structured data to a new file based on different sorting criteria.

# github link
[Github link](https://github.com/divya-gariga/CS332-532/tree/main/labs/lab6)
