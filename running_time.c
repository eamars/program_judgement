/*
   Judge system

   This is the judge system for any executable program with standard input and output.
   The program can be set to run under certain parameter and maximun running time. The
   Output of program are compared with given example.

	Author:	 Ran Bao
	Date:	 27/Nov/2014
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define TIMEOUT 2
#define ERROR 1
#define SUCCESS 0

/*
	sig_alrm is used to replace the default action for SIGALRM. This function
    is used to terminate the process that running out of the time.
*/
static void
sig_alrm(int signo){
	exit(TIMEOUT);
}

/* handy function that takes the error code and print the error message */
void
error_msg(int errno, char *message){
	printf("%s\n", message);
	exit(errno);
}




/* Main function takes two parameters  */
int
main(int argc, char **argv){
	if (argc != 3){
		printf("Usage: %s target_program timeout\n", argv[0]);
		error_msg(TIMEOUT, "Invalid input");
	}

	pid_t pid;

	/* Create a new process that is used to run the test program */
	pid = fork();

	/* If failed to create the child process, exit with code ERROR */
	if (pid < 0){
		error_msg(ERROR, "Failed to fork");
	}

	/* The child process only */
	if (pid == 0){
		/* Register the SIGALRM with sig_alrm function */
		if (signal(SIGALRM, sig_alrm) == SIG_ERR){
			error_msg(ERROR, "Failed to catch SIGALRM");
		}

		/* Create a buffer that capture the output from stream */
		char buf[80];

		/* Create a new pipe that execute the command and forward to stdout */
		FILE *fp = NULL;

		/* Start the timer that counts [timeout] seconds */
		alarm(atoi(argv[2]));

		printf("You have %d second to finish your task.\n", atoi(argv[2]));

		/* Execute the command and terminate if timeout */
		if ((fp = popen(argv[1], "r")) == NULL){
			error_msg(ERROR, "Failed to open pipe");
		}

		/* Read the output from pipe until read the NULL */
		while (fgets(buf, 80, fp) != NULL){
			printf(">> %s", buf);
		}
		pclose(fp);

		/* Terminate the process normally */
		alarm(0);

		exit(SUCCESS);
	}

	/* Parent process */
	else {
		int status;
		int exit_code;

		/* Parent just wait until task to finish or timeout */
		wait(&status);

		exit_code = WEXITSTATUS(status);

		/* Print out the result */
		if (exit_code == SUCCESS){
			printf("Task finished before timeout.\n");
		}
		else if (exit_code == ERROR){
			printf("Task abort due to some error.\n");
		}
		else if (exit_code == TIMEOUT){
			printf("Task didn't make it.\n");
		}
		else {
			printf("Unregistered return value %d.\n", exit_code);
		}

		printf("Main process terminated.\n");
	}

	exit(SUCCESS);
}

