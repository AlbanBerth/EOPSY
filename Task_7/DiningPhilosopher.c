#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define N 5						//Number of philosophers
#define LEFT (ID + N - 1) % N	//ID of the philosopher on the left	
#define RIGHT (ID + 1) % N		//ID of the philosopher on the right

#define THINKING 1 	//Begin with
#define HUNGRY 2 	//Begin with
#define EATING 3 	//Begin with

sem_t m; 		//MUTual EXclusion initialized to 1
int state[N];	//Philosophers' state (initiated to THINKING's)
sem_t s[N];		//Used for synchronization (initialized to 0's)


void *philosopher(void *num);
void grab_forks(int i);
void test(int i);
void put_away_forks(int i) ;


int main()
{
//Create and set the philosophers' ID
	int philosopherID[N];

	for (int i = 0 ; i < N ; i++)
	{
		philosopherID[i] = i;
		printf("Philosopher[%d] is sitting around the table...\n", i);
	}

	printf("==================================================\n");

    pthread_t philosopherThread[N];
	
//Initialization of mutex
    sem_init(&m, 0, 1);		

//Initialization of semaphore for synchronization
    for (int i = 0; i<N; i++) 
    {
        sem_init(&s[i], 0, 0);	
    }

//Create one thread for each philosophers
    for (int i = 0; i< N; i++) 
    {
        pthread_create(&philosopherThread[i], NULL, philosopher, &philosopherID[i]);
    }

//Join all threads
    for (int i = 0; i < N; i++) 
    {
        pthread_join(philosopherThread[i], NULL);
    }
   	
	return 0;
}


void *philosopher(void *num)
{
	//Set the ID of the current philosopher
	int ID = *(int *)num; 

	state[ID] = THINKING;
	printf("Philosopher[%d] is THINKING\n", ID);

	while(1) 
	{
    	sleep(1);
    	grab_forks(ID);		//Philosopher grabs forks
    	sleep(1);			//Philosopher is eating
    	put_away_forks(ID);	//Philosopher puts forks down
    }
}


void grab_forks(int i)
{
	int ID = i;

	sem_wait(&m); //Begin Critical Section
		state[ID] = HUNGRY; //Set state to the philosopher to HUNGRY
   		printf("Philosopher[%d] is HUNGRY.\n", ID);
		test(ID); //Philosopher try to eat
	sem_post(&m); //End Critical Section

	sem_wait(&s[ID]);
	sleep(1);
}


void test(int i)
{
	int ID = i;

//If the philosopher is hungry and his neighbors are not eating then the philosopher take the forks
	if(state[ID] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
	{	
		state[ID] = EATING; //Set state to the philosopher to EATING
		sleep(2);

        printf("Philosopher[%d] takes forks [%d-%d].\n", ID, LEFT, ID);
        printf("Philosopher[%d] is EATING.\n", ID);

		sem_post(&s[ID]);
	}
}


void put_away_forks(int i) 
{
	int ID = i;

	sem_wait(&m); //Begin Critical Section
		state[ID] = THINKING; //Set state to the philosopher to THINKING

		printf("Philosopher[%d] puts fork [%d-%d] down.\n", ID, LEFT, ID);
		printf("Philosopher[%d] is THINKING.\n", ID);

		test(LEFT);		//Test eating conditions for the left philosopher
		test(RIGHT);	//Test eating conditions for the right philosopher
	sem_post(&m); 		//End Critical Section
}