#include <stdio.h> /*For function printf()*/
#include <signal.h> /*For function signal()*/
#include <stdlib.h> /*For function const EXIT_SUCCESS and EXIT_FAILURE*/
#include <unistd.h> /*For function fork() and sleep()*/
#include <sys/types.h> /*For function type pid_t*/
#include <sys/wait.h> /*For function wait()*/

//#define WITH_SIGNALS

//Define global variable
#define NUM_CHILD 5 //Define how many child we want to creat
int aChildren[NUM_CHILD]; //Declare a array which hold all child PID

int mask = 0; //mask = 1 if signal INT happened

int PoC; //Parent or Child indicate if the current process is a parent (1) or a child (0)


//Define the handlers
void int_handler(int num)
{
		if (PoC == 1)
		{
			mask = 1; //signal INT happened
			printf("\nParent[%d]: received SIGINT signal.\n", getpid());
		}
}

void term_handler(int num)
{
	if (PoC == 0)
	{
		printf("Terminate child[%d] ...\n", getpid());
		kill(getpid(), 9);
	}
}



int main()
{
	int cTerm = 0; //Counter of children terminated
	int counter = 0; //Counter of child successfully created
	
	pid_t pid;
//Start the creation of the NUM_CHILD children
	for (int i = 1 ; i <= NUM_CHILD ; i++)
	{
		pid = fork(); //Creation of one child

		if (pid >= 0)
		{
			if (pid == 0) //In child process
			{
				PoC = 0;
				#ifdef WITH_SIGNALS
					signal(SIGINT, SIG_IGN); //Ignore SIGINT signal

					signal(SIGTERM, term_handler);
				#endif
				sleep(10);
				printf("\nChild PID: %d Parent PID: %d\n", getpid(), getppid());
								printf("Child[%d] process finished!\n", getpid());
				exit(EXIT_SUCCESS);
			}
			else //In parent process
			{
				PoC = 1;
				#ifdef WITH_SIGNALS
					for (int i = 0 ; i <= 64 ; i++) //Ignore all signal
					{
						signal(i, SIG_IGN);
					}

					signal(SIGCHLD, SIG_DFL); //Restore by default the signal SIGCHDL

					signal(SIGINT, int_handler);

					if (mask == 1) //Check if SIGINT signal happened
					{
						printf("Parent[%d]: sending SIGTERM signal to all created children.\n", getpid());
						for (int i = 0 ; i < counter ; i ++)
						{
							kill(aChildren[i], SIGTERM);
							cTerm ++;
						}
						mask = 0; //Reset the default value of mask
					}
					else //SIGINT signal does not occure
					{
						aChildren[i - 1] = pid; //Stock the new child PID in the aChildren array
						counter++; //Increase the number of children successfuly created by one
					}
				#endif
				printf("Parent PID: %d Child (PID: %d) was created!\n", getpid(), pid);
			}
		}
		else //Error in fork()
		{
			perror("Error in the function child.");
			kill(0, SIGTERM); //Send to all created children SIGTERM
			exit(1);
		}
		sleep(1); //Wait one second between the creation of two children
	}
	
	while (1)
	{
        if (wait(NULL) == -1) 
		{
            printf("\nNo more child to synchronize.\n");
            #ifdef WITH_SIGNALS
            	printf("%d children terminating.\n", cTerm);
            #endif
            exit(0);
        }
	}

	for (int i = 0 ; i <= 64 ; i++) //Restore the old service handlers of all signals
	{
		signal(i, SIG_DFL);
	}
	return 0;
}
