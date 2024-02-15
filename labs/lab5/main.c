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

void listFilesRecursively(char *basePath, int depth)
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
    printf("%s\n", dirName);
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
      for (int i = 0; i <= depth; i++)
      {
        printf("\t");
      }
      printf("%s\n", dirent->d_name);
      if (S_ISDIR(fileStat.st_mode))
      {
        listFilesRecursively(path, depth + 1);
      }
    }
  }
  closedir(parentDir);
}

int main(int argc, char **argv)
{
  struct dirent *dirent;
  DIR *parentDir;

  if (argc < 2)
  {
    printf("Usage: %s <dirname>\n", argv[0]);
    exit(-1);
  }
  listFilesRecursively(argv[1], 0);
  return 0;
}
