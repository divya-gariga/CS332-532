/*
Name: Divya Gariga
BlazerId: DGARIGA
Project #: HW02
To compile: gcc -o search dgariga_HW02.c
To run: ./search 
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#define PATH_MAX_LENGTH 4096

void printFileProperties(struct stat stats, const char *filePath)
{
    //file size
    printf("%ld bytes ", (S_ISDIR(stats.st_mode)) ? 0 : (long)stats.st_size);

    // Permissions
    printf((S_ISDIR(stats.st_mode)) ? "d" : "-");
    printf((stats.st_mode & S_IRUSR) ? "r" : "-");
    printf((stats.st_mode & S_IWUSR) ? "w" : "-");
    printf((stats.st_mode & S_IXUSR) ? "x" : "-");
    printf((stats.st_mode & S_IRGRP) ? "r" : "-");
    printf((stats.st_mode & S_IWGRP) ? "w" : "-");
    printf((stats.st_mode & S_IXGRP) ? "x" : "-");
    printf((stats.st_mode & S_IROTH) ? "r" : "-");
    printf((stats.st_mode & S_IWOTH) ? "w" : "-");
    printf((stats.st_mode & S_IXOTH) ? "x" : "-");
    printf(" ");

    // Last access date & time
    struct tm *atime_tm = localtime(&stats.st_atime);
    char atime_str[20];
    strftime(atime_str, sizeof(atime_str), "%b %d %H:%M:%S %Y", atime_tm);
    printf("Last accessed: %s", atime_str);

    // Check if it's a symbolic link
    if (S_ISLNK(stats.st_mode))
    {
        char linkTarget[PATH_MAX_LENGTH];
        ssize_t bytesRead = readlink(filePath, linkTarget, sizeof(linkTarget) - 1);
        if (bytesRead != -1)
        {
            linkTarget[bytesRead] = '\0';
            printf("Symbolic Link: %s", linkTarget);
        }
    }
    printf("\n");
}

void listFilesRecursively(char *basePath, int depth, bool isVerbose, long fileSizeThreshold, const char *searchString, int searchDepth, char fileType)
{

    DIR *parentDir = opendir(basePath);
    if (parentDir == NULL)
    {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    if (depth == 0)
    {
        const char *dirName = basename(basePath);
        printf("base directory %s\n", dirName);
    }

    struct dirent *dirent;
    while ((dirent = readdir(parentDir)) != NULL)
    {
        if (strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..") != 0)
        {
            char path[PATH_MAX_LENGTH];
            snprintf(path, sizeof(path), "%s/%s", basePath, dirent->d_name);
            struct stat fileStat;
            if (lstat(path, &fileStat) < 0)
            {
                perror("Error getting file information");
                exit(EXIT_FAILURE);
            }
            if ((fileType == 'a') || (fileType == 'f' && S_ISREG(fileStat.st_mode)) || (fileType == 'd' && S_ISDIR(fileStat.st_mode)))
            {
                if (isVerbose)
                {
                    for (int i = 0; i <= depth; i++)
                    {
                        printf("\t");
                    }
                    printf("%s ", dirent->d_name);
                    printFileProperties(fileStat, path);
                }
                else
                {
                    // Check if both -L and -s options are specified
                    if (fileSizeThreshold != -1 && searchString != NULL && depth <= searchDepth)
                    {
                        if ((fileStat.st_size >= fileSizeThreshold) && (strstr(dirent->d_name, searchString) != NULL))
                        {
                            printf("%s\n", dirent->d_name);
                        }
                    }
                    else if (fileSizeThreshold != -1)
                    {
                        // Only -L option is specified
                        if (fileStat.st_size >= fileSizeThreshold)
                        {
                            printf("%s\n", dirent->d_name);
                        }
                    }
                    else if (searchString != NULL)
                    {
                        // Only -s option is specified
                        if (depth <= searchDepth && strstr(dirent->d_name, searchString) != NULL)
                        {
                            printf("%s\n", dirent->d_name);
                        }
                    }
                    else
                    {
                        // Neither -L nor -s options are specified
                        for (int i = 0; i <= depth; i++)
                        {
                            printf("\t");
                        }
                        printf("%s\n", dirent->d_name);
                    }
                }
            }
            if (S_ISDIR(fileStat.st_mode))
            {
                listFilesRecursively(path, depth + 1, isVerbose, fileSizeThreshold, searchString, searchDepth, fileType);
            }
        }
    }
    closedir(parentDir);
}

int main(int argc, char **argv)
{
    int option = 0;
    bool isVerbose = false;
    long fileSizeThreshold = -1;
    const char *searchString = NULL;
    int searchDepth = -1;
    char fileType = 'a';

    while ((option = getopt(argc, argv, "vL:s:t:")) != -1)
    {
        switch (option)
        {
        case 'v':
            isVerbose = true;
            break;
        case 'L':
            fileSizeThreshold = atol(optarg);
            break;
        case 's':
            searchString = optarg;
            searchDepth = atoi(argv[optind]);
            break;
        case 't':
            fileType = *optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s [-v] [-L <file size>] [-s <string pattern> <depth>] [directory]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind == argc)
    {
        char currentDir[100];
        if (getcwd(currentDir, sizeof(currentDir)) != NULL)
        {
            listFilesRecursively(currentDir, 0, isVerbose, fileSizeThreshold, searchString, searchDepth, fileType);
        }
        else
        {
            perror("Error getting current directory");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        listFilesRecursively(".", 0, isVerbose, fileSizeThreshold, searchString, searchDepth, fileType);
    }
    return 0;
}
