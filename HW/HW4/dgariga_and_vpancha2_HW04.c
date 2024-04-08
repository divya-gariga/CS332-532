/*
Name: Divya Gariga, Vaishnavi Panchalingala
BlazerId: DGARIGA, vpancha2
Project #: HW4
To compile: gcc queue.c -pthread ./dgariga_and_vpancha2_HW04.c
program> To run: ./a.out 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>
#include "queue.h"
#include <time.h>

#define MAX_QUEUE_LEN 100
#define MAX_JOBS_LEN 1000

queue *q;
int p=1; 
Job jobs[MAX_JOBS_LEN];
int running_jobs_count = 0;

//function to get current date and time
time_t getCurrentDateTime()
{
    time_t current_time;
    time(&current_time);
    return current_time;
}

// Function to execute a job
void *execute_job(void *arg)
{
    Job *job = (Job *)arg;
    running_jobs_count++;
    job->status = "running";
    job->start_time = getCurrentDateTime();

    pid_t pid = fork();
    if (pid == 0)
    { // Child process
        // Redirect stdout and stderr to appropriate files
        char out_file[20], err_file[20];
        sprintf(out_file, "%d.out", job->job_id);
        sprintf(err_file, "%d.err", job->job_id);
        freopen(out_file, "w", stdout);
        freopen(err_file, "w", stderr);

        // Execute the command
        char *argv[11];         // Maximum 10 arguments plus 1 for the NULL terminator
        argv[0] = job->command; // First argument is the command itself

        // Tokenize arguments
        int i = 1;
        char *token = strtok(job->arguments, " ");
        while (token != NULL && i < 11)
        {
            argv[i++] = token;
            token = strtok(NULL, " ");
        }
        argv[i] = NULL; // Terminate the argument list with NULL

        // Execute the command
        execvp(job->command, argv);
        // If execvp fails, print error and exit
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    { // Parent process
        // Wait for the child process to terminate
        int status;
        wait(&status);

        if (!WIFEXITED(status))
        {
            fprintf(stderr, "Child process %d did not terminate normally!\n", pid);
        }
        job->end_time = getCurrentDateTime();
        job->status = "success"; // Update job status
    }
    else
    { // Error
        perror("fork");
    }
    --running_jobs_count;
}

//function to show running or waiting jobs
void show_jobs(int n)
{
    printf(" Jobid \t command \t status:\n");
    for (int i = 0; i < n; i++)
    {
        if (strcmp(jobs[i].status, "success") != 0)
            printf("%d \t %s \t  %s \t %s\n", jobs[i].job_id, jobs[i].command, jobs[i].arguments, jobs[i].status);
    }
}

//function to show the submit history
void submit_history(int n)
{
    printf(" Jobid \t command starttime \t endtime \t status:\n");
    for (int i = 0; i < n; i++)
    {
        if (strcmp(jobs[i].status, "success") == 0)
            printf("%d \t %s %s \t %s \t %s \t %s\n", jobs[i].job_id, jobs[i].command, jobs[i].arguments, strtok(ctime(&(jobs[i].start_time)), "\n"), strtok(ctime(&(jobs[i].end_time)), "\n"), jobs[i].status);
    }
}

//function to create a new job
Job create_job(char *command, char *arguments, int jobId)
{
    Job new_job;
    new_job.job_id = jobId;            // Assign job ID based on the number of jobs in the queue
    new_job.command = strdup(command); // Copy command string
    if (arguments != NULL)
        new_job.arguments = strdup(arguments); // Copy arguments string
    else
        new_job.arguments = NULL;       // No arguments provided
    new_job.status = strdup("waiting"); // Initial status is waiting
    new_job.start_time = new_job.end_time = time(NULL);
    return new_job;
}

// Function to manage job execution
void *job_manager(void *arg)
{
    Job *j;
    while (1)
    {
        if (!queue_empty(q) && running_jobs_count < p)
        {
            j = queue_delete(q); // Get the next job from the queue
            pthread_create(&j->tid, NULL, execute_job, (void *)j);
            pthread_detach(j->tid);
        }
        else
        {
            sleep(1); // Sleep for a while before checking again
        }
    }
}


int main(int argc, char *argv[])
{
    int i = 0;
    if (argc != 2){
        p=1;
    } else if (atoi(argv[1]) > 8)
    {
        p = 8;
    }
    else
    {
        p = atoi(argv[1]);
    }

    q = queue_init(MAX_QUEUE_LEN); // Initialize the queue

    pthread_t manager_thread;
    pthread_create(&manager_thread, NULL, job_manager, NULL); // Create manager thread

    printf("Welcome to the job scheduler.\n");
    printf("Enter commands (submit, showjobs, submithistory):\n");
    while (1)
    {
        char input[100];
        printf("Enter Command>");
        fgets(input, sizeof(input), stdin);
        input[strlen(input) - 1] = '\0'; // Remove newline character
        char *command = strtok(input, " ");
        if (strcmp(command, "submit") == 0)
        {
            char *program = strtok(NULL, " ");
            char *args = strtok(NULL, ""); // Get the rest of the input as arguments
            jobs[i] = create_job(program, args, i);
            queue_insert(q, jobs + i); // Insert job into the queue
            printf("Added job %d to the job queue\n", i++);
        }
        else if (strcmp(command, "showjobs") == 0)
        {
            show_jobs(i);
        }
        else if (strcmp(command, "submithistory") == 0)
        {
            submit_history(i);
        }
        else
        {
            printf("Invalid command. Please try again.\n");
        }
    }
    return 0;
}
