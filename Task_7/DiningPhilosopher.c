#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>


#define NBR_PHILO 5 //Define the number of philosopher around the table 
#define ID_RIGHT (ID + NBR_PHILO - 1) % NBR_PHILO //ID of the philosopher on the right of the current philosopher 
#define ID_LEFT (ID + 1) % NBR_PHILO //ID of the philosopher on the left of the current philosopher

pthread_mutex_t mutex; //Declaration of the mutex (mutual exclusion)
pthread_t philosopherThread[NBR_PHILO]; //Declaration for philosopher threads

int array_ID[NBR_PHILO]; //Declaration of an array to store ID of philisophers

enum {THINKING, HUNGRY, EATING} array_state[NBR_PHILO]; //Array to store the philosopher state

//Declarations of functions
void grab_forks(int num);
void put_away_forks(int num);
void test(int num);
void *createPhilosopher(void *num);


int main()
{
   srand(time(NULL));

   pthread_mutex_init(&mutex, NULL); //Initialize the mutex

//Initialize philosophers id
   for (int i = 0; i < NBR_PHILO; i++)
   {
      array_ID[i] = i;
   }

//Create philosophers threads 
   for (int i = 0; i < NBR_PHILO; i++)
   {
      pthread_create(&philosopherThread[i], NULL, createPhilosopher, &array_ID[i]);
      printf("Philosopher[%d] is sitting around the table...\n", i);
   }

   printf("==================================================\n");

//Join all philosopher's threads
   for (int i = 0; i < NBR_PHILO; i++)
   {
      pthread_join(philosopherThread[i], NULL); 
   }

//End the program correctly
   pthread_mutex_destroy(&mutex); //Clean mutex
   
   return 0;
}

void *createPhilosopher(void *num)
{
   int ID = *(int *)num; //Set the ID of the current philosopher

   sleep(1); //To display correctly

   while (1)
   {
      printf("Philosopher[%d] is THINKING\n", ID);
      sleep(rand()%5); //Sleep a random time between 0 and 4 seconds

      pthread_mutex_lock(&mutex); //Grab token
         grab_forks(ID);
         put_away_forks(ID);
      pthread_mutex_unlock(&mutex); //Throw token
   }
}
 

void grab_forks(int num) 
{
   int ID = num;
   printf("Philosopher[%d] is HUNGRY\n", ID);
   array_state[ID] = HUNGRY;   

   test(ID);
}

 

void put_away_forks(int num) 
{  
   int ID = num;
   array_state[ID] = THINKING;  
}

void test(int num) 
{  
   int ID = num;

   if(array_state[ID] == HUNGRY && array_state[ID_RIGHT] != EATING && array_state[ID_LEFT] != EATING)
   {
      printf("Philosopher[%d] is EATING\n", ID);
      array_state[ID] = EATING;
      sleep(rand()%4);
   }  
 }