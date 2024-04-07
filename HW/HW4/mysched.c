#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>
#include "queue.h"

#define MAX_QUEUE_LEN 100  /* maximum JOBQ length */
#define MAX_JOBS_LEN 1000 /* maximum JOBS length */

queue *q; // Global queue to store submitted jobs
int p; // Maximum number of jobs that can be executed simultaneously
Job jobs[MAX_JOBS_LEN];
int running_jobs_count = 0;

// Function to execute a job
void *execute_job(void *arg) {
    Job *job = (Job *)arg;
    running_jobs_count++;
    job->status = "running";
    pid_t pid = fork();
    if (pid == 0) { // Child process
        // Redirect stdout and stderr to appropriate files
        char out_file[20], err_file[20];
        sprintf(out_file, "%d.out", job->job_id);
        sprintf(err_file, "%d.err", job->job_id);
        freopen(out_file, "w", stdout);
        freopen(err_file, "w", stderr);

        // Execute the command
        char *argv[11]; 
        argv[0] = job->command; 

        // Tokenize arguments
        int i = 1;
        char *token = strtok(job->arguments, " ");
        while (token != NULL && i < 11) {
            argv[i++] = token;
            token = strtok(NULL, " ");
        }
        argv[i] = NULL; // Terminate the argument list with NULL
        // Execute the command
        execvp(job->command, argv);
        // If execvp fails, print error and exit
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid > 0) { // Parent process
        int status;
        wait(&status);
        if (!WIFEXITED(status)) {
                        fprintf(stderr, "Child process %d did not terminate normally!\n", pid);

        }
        job->status = "complete"; 
    } else { 
        perror("fork");
    }
    --running_jobs_count;
}

void show_jobs(int n) {
    printf(" Jobid \t command \t status:\n");
    for (int i = 0; i < n; i++) {
        if (strcmp(jobs[i].status, "complete") != 0)
            printf("%d \t %s \t  %s \t %s\n", jobs[i].job_id, jobs[i].command,jobs[i].arguments, jobs[i].status);
    }
}

void submit_history(int n){
    printf(" Jobid \t command \t status:\n");
for (int i = 0; i < n; i++)
            {
                if (strcmp(jobs[i].status, "complete") == 0)
                    printf("%d %s %s\n",jobs[i].job_id,jobs[i].command,jobs[i].arguments);
            }}

Job create_job(char *command, char *arguments, int jobId) {
    Job new_job;

    new_job.job_id = jobId; 
    new_job.command = strdup(command); 
    new_job.arguments = strdup(arguments);
    new_job.status = strdup("waiting"); 

    return new_job;
}


// Function to manage job execution
void *job_manager(void *arg) {
    Job *j;
    while (1) {
        if (!queue_empty(q) && running_jobs_count < p) {
            j = queue_delete(q); // Get the next job from the queue
            pthread_create(&j->tid, NULL, execute_job, (void *)j);
            pthread_detach(j->tid);
        } else {
            sleep(1); // Sleep for a while before checking again
        }
    }
}

int main(int argc, char *argv[]) {
    int i = 0;
    if (argc != 2 || atoi(argv[1]) > 8) {
        p = 8;
    } else {
        p = atoi(argv[1]);
    }

    q = queue_init(MAX_QUEUE_LEN); // Initialize the queue

    pthread_t manager_thread;
    pthread_create(&manager_thread, NULL, job_manager, NULL); // Create manager thread

    printf("Welcome to the job scheduler.\n");
    printf("Enter commands (submit, showjobs, submithistory):\n");

    while (1) {
        char input[100];
        printf("Enter Command>");
        fgets(input, sizeof(input), stdin);
        input[strlen(input) - 1] = '\0'; // Remove newline character
        char *command = strtok(input, " ");
        if (strcmp(command, "submit") == 0) {
            char *program = strtok(NULL, " ");
            char *args = strtok(NULL, ""); // Get the rest of the input as arguments
           jobs[i] = create_job(program, args, i);
                queue_insert(q, jobs+i); // Insert job into the queue
                printf("Added job %d to the job queue\n", i++);
        } else if (strcmp(command, "showjobs") == 0) {
            show_jobs(i);
        }
        else if (strcmp(command, "submithistory") == 0) {
            submit_history(i);
        }
         else {
            printf("Invalid command. Please try again.\n");
        }
    }
    return 0;
}


