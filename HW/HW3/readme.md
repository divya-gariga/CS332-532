<!--
NOTES:
This is the README file home work assignment 2
-->

# Compilation Instructions

```bash
make 
# this will compile the dgariga_HW03.c file and return an executable with the executable name search.exe
```
# Run Instructions

```bash
make run ARGS="-v -e 'ls -l'"
#in Windows run the executable as shown
```

# Code Description

### Functions :

## main function:
1. **Function Signature:**
   ```c
   int main(int argc, char **argv)
   ```
   The program starts with the `main` function, which is the entry point for any C program. It takes command-line arguments, where `argc` is the count of arguments, and `argv` is an array of strings containing the actual arguments.

2. **Variable Declarations:**
   ```c
   int option = 0;
   bool isVerbose = false;
   long fileSizeThreshold = -1;
   const char *searchString = NULL;
   int searchDepth = -1;
   char fileType = 'a';
   ```
   - `option`: Stores the result of `getopt` function, which is used for parsing command-line options.
   - `isVerbose`: A flag indicating whether the program should operate in verbose mode.
   - `fileSizeThreshold`: A threshold value for file size.
   - `searchString`: A string pattern to search for in files.
   - `searchDepth`: The depth to which the program should search for files.
   - `fileType`: Specifies the type of files to be considered.

# Output screenshots

# github link
[Github link](https://github.com/divya-gariga/CS332-532/tree/main/HW/HW3)

# References
https://man7.org/linux/man-pages/man3/getopt.3.html
