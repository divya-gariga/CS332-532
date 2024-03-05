/*
Name: Divya Gariga
BlazerId: DGARIGA
Project #: HW03 
To compile: gcc dgariga_hw03.c -o search 
or make
program> To run: ./search -v -s read 4 -e "wc -l"
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <wait.h>

#define vIndex 0
#define lIndex 1
#define sIndex 2
#define s2Index 3
#define tIndex 4
#define eIndex 5
#define EIndex 6
#define PATH_SIZE 1024
#define TEMP_SIZE 25
#define MAX_FILES 100

struct command
{
	char **argv;
	char **flagArgs;
	char *path;
	int argc;
	int numFlags;
};

const char *eUnixCommand;
const char *EUnixCommand;
char *matchedFiles[MAX_FILES];
int matchedFilesCount=0;

typedef struct command Command;
// CHATGPT: https://chat.openai.com/c/1990640c-02e5-46ea-9c1c-8ab556ed1243
void print_permissions(mode_t mode, char *retPerms)
{
	char perms[10] = "---------";
	mode_t flags[] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};
	char chars[] = "rwxrwxrwx";

	for (int i = 0; i < 9; ++i)
	{
		if (mode & flags[i])
		{
			perms[i] = chars[i];
		}
	}

	strcpy(retPerms, perms);
}

char *getSymLink(struct stat s, char *path)
{
	char *linkTo = NULL;

	if (S_ISLNK(s.st_mode))
	{
		linkTo = (char *)malloc(PATH_SIZE * sizeof(char));
		char *linkTarget = malloc(s.st_size + 1);
		ssize_t len = readlink(path, linkTarget, s.st_size + 1);
		if (len == -1)
		{
			perror("readlink");
			free(linkTarget);
		}

		strcpy(linkTo, linkTarget);
	}

	return linkTo;
}

char *getDetails(struct stat s, char *details, char *path)
{
	details = (char *)malloc(PATH_SIZE * sizeof(char));

	long bytes;
	if (S_ISDIR(s.st_mode))
	{
		bytes = 0;
	}
	else
	{
		bytes = s.st_size;
	}

	char *perms = (char *)malloc(10 * sizeof(char));
	print_permissions(s.st_mode, perms);
	time_t at = s.st_atime;
	char *accessTime = ctime(&at);
	accessTime[strlen(accessTime) - 1] = '\0';
	sprintf(details, "%ld | %s | %s", bytes, perms, accessTime);
	return details;
}

void choosePrint(struct stat s, char *currentPath, char *details, int det)
{
	char *linkTo = NULL;
	int sym = 0;

	if (S_ISLNK(s.st_mode))
	{
		linkTo = getSymLink(s, currentPath);
		sym = 1;
	}

	char *lastBack = strrchr(currentPath, '/');
	char *lastDir = &lastBack[1];

	if (sym && det)
	{
		printf("%s (%s) %s\n", lastDir, linkTo, details);
	}
	else if (sym)
	{
		printf("%s (%s)\n", lastDir, linkTo);
	}
	else if (det)
	{
		printf("%s %s\n", lastDir, details);
	}
	else
	{
		printf("%s\n", lastDir);
	}
}

// -e argument: to execute given unix command on all matched files at once
void executeUnixCommand(char *fileName, const char *commandString) {
    // Make a copy of the command string for tokenization
    char *commandCopy = strdup(commandString);
    if (commandCopy == NULL) {
        perror("strdup failed");
        exit(EXIT_FAILURE);
    }

    // Tokenize the command string
    char *tokens[100];  
    int tokenCount = 0;
    int status;
    char *token = strtok(commandCopy, " ");
    while (token != NULL) {
        tokens[tokenCount++] = token;
        token = strtok(NULL, " ");
    }

    tokens[tokenCount++] = fileName; // Add filename as the last argument
    tokens[tokenCount] = NULL;  // Add a NULL pointer to the end of the array for exec functions

    // Fork and execute the command
    pid_t pid = fork();

     if (pid == 0) {
        // Child process
        // Use execvp to run the command with its arguments
        execvp(tokens[0], tokens);
		printf("\n");
    } else if (pid > 0) {
         wait(&status); // wait for the child process to terminate
            if (WIFEXITED(status)) {
			   // child process terminated normally
                // printf("Child process exited with status = %d\n", WEXITSTATUS(status));
                } 
            else {
			   // child process did not terminate normally
                // printf("Child process did not terminate normally!\n"); 
                }
			}
        else {
         perror("fork"); /* use perror to print the system error message */
         exit(EXIT_FAILURE);
    }
    free(commandCopy);
}

//-E argument: to execute unix command for each matched file
void executeUnixCommandWithE() {
    // Make a copy of the command string for tokenization
    char *commandCopy = strdup(EUnixCommand);
    if (commandCopy == NULL) {
        perror("strdup failed");
        exit(EXIT_FAILURE);
    }

    // Tokenize the command string
    char *tokens[100];  // Adjust the array size based on your needs
    int tokenCount = 0;
    int status;
    char *token = strtok(commandCopy, " ");
    while (token != NULL) {
        tokens[tokenCount++] = token;
        token = strtok(NULL, " ");
    }

    // Add all matched filenames
    for(int i=0;i<matchedFilesCount;i++)
    {
      tokens[tokenCount++]=matchedFiles[i];
    }

    tokens[tokenCount] = NULL;  // Add a NULL pointer to the end of the array for exec functions

    // Fork and execute the command
    pid_t pid = fork();

     if (pid == 0) {
        // Use execvp to run the command with its arguments
        execvp(tokens[0], tokens);
		printf("\n");
    } 
	else if (pid > 0) {
		wait(&status); // wait for the child process to terminate 
        if (WIFEXITED(status)) { 
			// child process terminated normally
            // printf("Child process exited with status = %d\n", WEXITSTATUS(status));
			} 
			else {
				//child process did not terminate normally 
			   printf("Child process did not terminate normally!\n");     
                }
				}
    else {
        perror("fork"); 
        exit(EXIT_FAILURE);
    }
    free(commandCopy);
}

void search(char *path, int indent, char **flagArgs, int numFlags){
	struct dirent *dirent;
	DIR *parentDir;
	parentDir = opendir(path);
	if (parentDir == NULL)
	{
		printf("Error opening directory: '%s'\n", path);
		exit(-1);
	}
	int count = 1;
	while ((dirent = readdir(parentDir)) != NULL)
	{
		int nameLen = strlen((*dirent).d_name);
		char *fileName = (char *)malloc(nameLen * sizeof(char));
		strcpy(fileName, (*dirent).d_name);
		char fullFilePath[PATH_MAX];
        snprintf(fullFilePath, sizeof(fullFilePath), "%s/%s", path, fileName);

		if (strcmp(fileName, ".") != 0 && strcmp(fileName, "..") != 0)
		{
			struct stat s;
			char *currentPath = (char *)malloc(PATH_SIZE * sizeof(char));
			strcpy(currentPath, path);
			strcat(currentPath, "/");
			strcat(currentPath, fileName);
			if (lstat(currentPath, &s) == -1)
			{
				perror("stat");
				exit(EXIT_FAILURE);
			}
			if (S_ISDIR(s.st_mode))
			{
				if (flagArgs[tIndex] != NULL)
				{
					if (strcmp(flagArgs[tIndex], "f") == 0)
					{
						continue;
					}
				}

				int count = indent;
				while (count > 0)
				{
					printf("  ");
					count--;
				}

				char *details = NULL;
				int det = 0;

				if (flagArgs[vIndex] != NULL)
				{
					details = getDetails(s, details, currentPath);
					det = 1;
				}
				choosePrint(s, currentPath, details, det);
				search(currentPath, ++indent, flagArgs, numFlags);
				--indent;
			}
			else
			{
				char *details = NULL;
				int checkFlag = 0;
				int flagCount = 0;

				char *linkTo = NULL;
				int sym = 0;
				int det = 0;
        
				for (int i = 0; i < numFlags; i++)
				{
					if (flagArgs[i] != NULL && i != s2Index)
					{
						flagCount++;
					}
				}

				if (flagCount > 0)
				{
					if (flagArgs[vIndex] != NULL)
					{
						details = getDetails(s, details, currentPath);
						det = 1;
						checkFlag++;
					}

					if (flagArgs[lIndex] != NULL)
					{
						if (s.st_size < atoi(flagArgs[lIndex]))
							continue;
						checkFlag++;
					}

					if (flagArgs[sIndex] != NULL)
					{
						if ((strstr(fileName, flagArgs[sIndex]) == NULL) || indent > atoi(flagArgs[s2Index]))
							continue;
						checkFlag++;
					}

					if (flagArgs[tIndex] != NULL)
					{
						if (strcmp(flagArgs[tIndex], "d") == 0)
							continue;
						checkFlag++;
					}
					if (flagArgs[eIndex] != NULL)
					{
						checkFlag++;
					}
                    if (flagArgs[EIndex] != NULL)
					{
						checkFlag++;
					}
					if (flagCount == checkFlag)
					{
                        matchedFiles[matchedFilesCount++]=strdup(fullFilePath);
						int count = indent;
						while (count > 0)
						{
							printf("  ");
							count--;
						}
						if (S_ISLNK(s.st_mode))
						{
							linkTo = getSymLink(s, currentPath);
							sym = 1;
						}
						choosePrint(s, currentPath, details, det);
						if (flagArgs[eIndex] != NULL)
						{
							executeUnixCommand(fullFilePath,eUnixCommand);
							}
						}
				}
				else
				{
					if (S_ISLNK(s.st_mode))
					{
						linkTo = getSymLink(s, currentPath);
						sym = 1;
					}

					int count = indent;
					while (count > 0)
					{
						printf("  ");
						count--;
					}
					choosePrint(s, currentPath, details, 0);
				}
			}
		}
	}
	closedir(parentDir);
}

int main(int argc, char **argv)
{
	int indent = 0;
	char *path = NULL;

	char **modArgv = &argv[1];
	int numArguments = 7;
	char **flagArgs = (char **)malloc(numArguments * sizeof(char *));

	Command currentCommand;
	currentCommand.path = path;
	currentCommand.argv = modArgv;
	currentCommand.argc = argc - 1;
	currentCommand.flagArgs = flagArgs;
	currentCommand.numFlags = numArguments;

	// Flag argument array declaration and malloc initialization
	for (int i = 0; i < currentCommand.numFlags; i++)
	{
		flagArgs[i] = NULL;
	}

	int opt;

	extern char *optarg;
	extern int optind, opterr, optopt;

	char *temp;
	int length;

	while ((opt = getopt(argc, argv, ":vL:s:t:e:E:")) != -1)
	{
		switch (opt)
		{
		case 'v':
			printf("INSIDE V CASE\n");
			temp = "-v is active!";
			length = strlen(temp);
			flagArgs[vIndex] = (char *)malloc(length * sizeof(char));
			strcpy(flagArgs[vIndex], temp);
			break;
		case 'L':
			printf("INSIDE L CASE\n");
			temp = optarg;
			flagArgs[lIndex] = (char *)malloc(strlen(temp) * sizeof(char));
			strcpy(flagArgs[lIndex], temp);
			break;
		case 's':
			printf("INSIDE S CASE\n");
			if (optarg)
			{
				int len = strlen(optarg);
				flagArgs[sIndex] = (char *)malloc((len + 1) * sizeof(char));
				strcpy(flagArgs[sIndex], optarg);

				if (optind < argc && argv[optind][0] != '-')
				{
					len = strlen(argv[optind]);
					flagArgs[sIndex + 1] = (char *)malloc((len + 1) * sizeof(char));
					strcpy(flagArgs[sIndex + 1], argv[optind]);

					optind++;
				}
				else
				{
					fprintf(stderr, "Option -%c requires a file name as an argument. Example: -s <pattern> <depth>\n", optopt);
					exit(EXIT_FAILURE);
				}
			}
			break;
		case 'e':
			printf("INSIDE e CASE\n");
			if (optarg)
			{
				int len = strlen(optarg);
				flagArgs[eIndex] = (char *)malloc((len + 1) * sizeof(char));
				eUnixCommand = (char *)malloc((len + 1) * sizeof(char));
				strcpy(flagArgs[eIndex], optarg);
                eUnixCommand=optarg;
			}
			break;
        case 'E':
			printf("INSIDE E CASE\n");
			if (optarg)
			{
				int len = strlen(optarg);
				flagArgs[EIndex] = (char *)malloc((len + 1) * sizeof(char));
				EUnixCommand = (char *)malloc((len + 1) * sizeof(char));
				strcpy(flagArgs[EIndex], optarg);
                EUnixCommand=optarg;
			}
			break;
		case 't':
			if (strcmp(optarg, "f") == 0 || strcmp(optarg, "d") == 0)
			{
				temp = optarg;
				flagArgs[tIndex] = (char *)malloc(strlen(temp) * sizeof(char));
				strcpy(flagArgs[tIndex], temp);
			}
			else
			{
				fprintf(stderr, "Option -%c requires an argument. Example: -t <file type>\n", optopt);
				exit(EXIT_FAILURE);
			}
			break;
		case ':':
			switch (optopt)
			{
			case 'L':
				fprintf(stderr, "Option -%c requires an argument. Example: -L <min size>\n", optopt);
				exit(EXIT_FAILURE);
			case 's':
				if (optarg == NULL || optind < argc && (argv[optind] == NULL || argv[optind][0] == '-'))
				{
					fprintf(stderr, "Option -%c requires a file name as an argument. Example: -s <pattern> <depth>\n", optopt);
					exit(EXIT_FAILURE);
				}
			case 't':
				printf("OPTARG: %s\n", optarg);
				if (optarg == NULL)
				{
					fprintf(stderr, "Option -%c requires an argument. Example: -t <file type>\n", optopt);
					exit(EXIT_FAILURE);
				}
			case 'e':
				printf("OPTARG: %s\n", optarg);
				if (optarg == NULL)
				{
					fprintf(stderr, "Option -%c requires an argument. Example: -e 'unix command'\n", optopt);
					exit(EXIT_FAILURE);
				}
		    case 'E':
				printf("OPTARG: %s\n", optarg);
				if (optarg == NULL)
				{
					fprintf(stderr, "Option -%c requires an argument. Example: -E 'unix command'\n", optopt);
					exit(EXIT_FAILURE);
				}
			default:
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				exit(EXIT_FAILURE);
			}
			break;
		}
	}
	if (argc == 1)
	{
		currentCommand.path = (char *)malloc(PATH_SIZE * sizeof(char));
		getcwd(currentCommand.path, PATH_SIZE);
	}
	else
	{
		for (int i = 0; i < currentCommand.argc; i++)
		{
			if (i != s2Index && (strstr(currentCommand.argv[i], "/") != NULL) && (strstr(currentCommand.argv[i], "..") != NULL) || (strstr(currentCommand.argv[i], ".") != NULL))
			{
				if (strstr(currentCommand.argv[i], "..") != NULL)
				{
					if (strcmp(currentCommand.argv[i], "../") == 0)
					{
						currentCommand.path = (char *)malloc(PATH_SIZE * sizeof(char));
						char *tempPath = (char *)malloc(PATH_SIZE * sizeof(char));
						getcwd(tempPath, PATH_SIZE);

						char *lastBack = strrchr(tempPath, '/');
						if (lastBack != NULL)
						{
							*lastBack = '\0';
						}
						strcpy(currentCommand.path, tempPath);
					}
					else if (strstr(currentCommand.argv[i], "../") != NULL)
					{
						currentCommand.path = (char *)malloc(PATH_SIZE * sizeof(char));
						char *tempPath = (char *)malloc(PATH_SIZE * sizeof(char));
						getcwd(tempPath, PATH_SIZE);

						char *lastBack = strrchr(tempPath, '/');
						if (lastBack != NULL)
						{
							*lastBack = '\0';
						}

						*currentCommand.argv[i]++;
						*currentCommand.argv[i]++;
						strcpy(currentCommand.path, tempPath);
						strcat(currentCommand.path, currentCommand.argv[i]);
					}
					break;
				}
				else if (strstr(currentCommand.argv[i], ".") != NULL)
				{
					if (strcmp(currentCommand.argv[i], "./") == 0)
					{
						currentCommand.path = (char *)malloc(PATH_SIZE * sizeof(char));
						getcwd(currentCommand.path, PATH_SIZE);
					}
					else if (strstr(currentCommand.argv[i], "./") != NULL)
					{
						currentCommand.path = (char *)malloc(PATH_SIZE * sizeof(char));
						getcwd(currentCommand.path, PATH_SIZE);

						*currentCommand.argv[i]++;

						strcat(currentCommand.path, currentCommand.argv[i]);
					}
					break;
				}
				else
				{
					currentCommand.path = (char *)malloc(PATH_SIZE * sizeof(char));
					strcpy(currentCommand.path, currentCommand.argv[i]);
				}
			}
		}

		if (currentCommand.path == NULL)
		{
			currentCommand.path = (char *)malloc(PATH_SIZE * sizeof(char));
			getcwd(currentCommand.path, PATH_SIZE);
		}
	}

	int pathLen = strlen(currentCommand.path);
	if (currentCommand.path[pathLen - 1] == '/')
	{
		currentCommand.path[pathLen - 1] = '\0';
	}

	char *lastBack = strrchr(currentCommand.path, '/');
	char *baseDir = &lastBack[1];
	printf("%s\n", baseDir);
	indent++;

	search(currentCommand.path, indent, currentCommand.flagArgs, currentCommand.numFlags);
  if(flagArgs[EIndex]!=NULL){
	printf("\n");
  executeUnixCommandWithE();
  }
	return 0;
}
