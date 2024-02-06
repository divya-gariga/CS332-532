#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#define BUFFER_SIZE 4096

int main(int argc, char *argv[])
{
    int sourceFileDescriptor, destinationFileDescriptor;
    char buffer[BUFFER_SIZE];
    long int n;

    if (argc != 3)
    {
        printf("provide both source and destination filenames");
        exit(-1);
    }

    if (strcmp(argv[1], argv[2]) == 0)
    {
        printf("source and destination filenames should be different");
        exit(-1);
    }

    sourceFileDescriptor = open(argv[2], O_RDONLY);

    // Open the destination file in append mode
    destinationFileDescriptor = open(argv[1], O_WRONLY | O_CREAT | O_APPEND, 0700);

    if (sourceFileDescriptor == -1 || destinationFileDescriptor == -1)
    {
        printf("Error opening file\n");
        exit(-1);
    }

    if (write(destinationFileDescriptor, "\n", 1) != 1)
    {
        printf("Error writing newline character to destination file");
    }
    
    // Read from the source file and append to the destination file
    while ((n = read(sourceFileDescriptor, buffer, sizeof(buffer))) > 0)
    {
        if (write(destinationFileDescriptor, buffer, n) != n)
        {
            printf("Error writing to destination file");
            exit(-1);
        }
    }
    
    if (n < 0)
    {
        printf(("Error reading input file"));
        exit(-1);
    }
        close(sourceFileDescriptor);
        close(destinationFileDescriptor);
    return 0;
}
    