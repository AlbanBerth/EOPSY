#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <pthread.h>
#include <semaphore.h>

#include <windows.h>

#define NBRCLIENT 2
#define NBRSEAT 2

//Declaration of functions
void *clientFunction(void *id);
void *barberFunction();

//Declaration global variable
sem_t seatWaitingRoom; //Define the umber of place in the waiting room  (NBRSEAT)
sem_t barberChair; //Define the number of barber chair (1 for the moment)
sem_t barberAwake; //Define if the barber is awake (1==yes ; 0==no)
sem_t freeChair; //Use to make client wait until the barber finish is job

int NMC = 0; //NMC stand for No More Client, this variale is to end the program when all clients are served

int main()
{
    printf("Number of clients: %d\n", NBRCLIENT);
    printf("Number of seats in the waiting room: %d\n\n", NBRSEAT);

//Declaration of variable
    pthread_t barberThread;
    pthread_t clientThread[NBRCLIENT];

    int idArray[NBRCLIENT];

//Initialization of the array of ID
    for (int i = 0 ; i < NBRCLIENT ; i++)
    {
        idArray[i] = i;
    }

//Initialization of the semaphores
    sem_init(&seatWaitingRoom, 0, NBRSEAT);
    sem_init(&barberChair, 0, 1);
    sem_init(&barberAwake, 0, 0);
    sem_init(&freeChair, 0, 0);

//Creation of one barbers
    pthread_create(&barberThread, NULL, barberFunction, NULL);

//Creation of clients
    for (int i = 0 ; i < NBRCLIENT ; i++)
    {
        pthread_create(&clientThread[i], NULL, clientFunction, (void *)&idArray[i]);
        Sleep(1000);
    }

//Join all client threads
    for (int i = 0 ; i < NBRCLIENT ; i++)
    {
        pthread_join(clientThread[i], NULL);
        Sleep(1000);
    }

//When all the client are served
    NMC = 1;
    sem_post(&barberAwake);
    pthread_join(barberThread, NULL);
}

void *clientFunction(void *id)
{
    int ID = *(int *) id;

//Client leave home and go to the barber shop
    printf("Client [%d] leaving home.\n", ID);
    Sleep(2000);
    printf("Client [%d] enter the barber shop.\n", ID);

//Client wait a seat in the waiting room
    sem_wait(&seatWaitingRoom);
    printf("Client [%d] is in the waiting room.\n", ID);

//Client wait for an available barber chair
    sem_wait(&barberChair);

//Free a seat in the waiting room
    sem_post(&seatWaitingRoom);

//Barber is sleeping so client waking him up
    printf("Client [%d] wake up the barber.\n", ID);
    sem_post(&barberAwake);

//Wait the end of the barber's work
    sem_wait(&freeChair);

//Free the barber chair
    sem_post(&barberChair);
    printf("Customer [%d] leaving barber shop.\n", ID);
}

void *barberFunction()
{
    while (!NMC)
    {
        printf("\nThe barber is sleeping.\n\n");
        sem_wait(&barberAwake);

        if (!NMC)
        {
        //Barber take care of the client
            printf("Barber is cutting hair...\n");
            Sleep(2000);
            printf("Barber has finished is job.\n");

        //Release the client
            sem_post(&freeChair);
        }
        else
        {
            printf("End of the day.\nThe barber wake up and go home.\n");
        }
    }
}
