#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

pid_t child_pid; // Global variable to store the PID of the child process

// Signal handler for SIGINT (Ctrl+C)
void sigint_handler(int signum)
{
    // Ignore SIGINT in the parent process
    if (getpid() != child_pid)
        signal(SIGINT, SIG_IGN);
    raise(SIGINT);
}

// Signal handler for SIGTSTP (Ctrl+Z)
void sigtstp_handler(int signum)
{
    // Ignore SIGTSTP in the parent process
    if (getpid() != child_pid)
        signal(SIGTSTP, SIG_IGN);
    else {
        // Handle SIGTSTP in the child process by pausing it
        printf("\nChild process paused by SIGTSTP\n");
    }
}


// Signal handler for SIGQUIT (Ctrl+\)
void sigquit_handler(int signum)
{
    if (getpid() != child_pid)
    {
        printf("\nsignal ctrl - \\ received, quitting parent process!...\n");
        exit(EXIT_SUCCESS); // Exit the program if parent process receives Ctrl+\ signal
    }
}

int main(int argc, char **argv)
{
    pid_t pid;
    int status;
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
    signal(SIGQUIT, SIG_IGN);
    // Install signal handlers

    if (argc < 2)
    {
        printf("Usage: %s <command> [args]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == 0)
    { // Childprocess
        printf("performing child process tasks!...\n");
        execvp(argv[1], &argv[1]);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {                    // Parent process
        child_pid = pid; // Store child PID
        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGQUIT, sigquit_handler);
        printf("Wait for the child process to terminate\n");
        while (1)
        {
            wait(&status); // Wait for the child process to terminate
            if (WIFEXITED(status))
            {
                printf("Child process exited with status = %d\n", WEXITSTATUS(status));
                break;
            }
            else if (WIFSIGNALED(status))
            {
                printf("Child process terminated by signal %d\n", WTERMSIG(status));
                break;
            }
        }
        printf("performing parent process task!...\n");
        sleep(10);
    }
    else
    { // Error
        perror("fork");
        exit(EXIT_FAILURE);
    }

    return 0;
}