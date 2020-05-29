#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <pthread.h>
#include <semaphore.h>

#include <stdbool.h>

#define NBRCLIENT 3     //Number of clients
#define NBRSEAT 1       //Number of seat in the waiting room
#define NBRBARBERM 1    //Number of barbers for man
#define NBRBARBERW 1    //Number of barbers for woman
#define NBRBARBERB 0    //Number of barbers for both

//Declaration of functions
void *clientFunction(void *id);
void *barberFunctionM(void *id);
void *barberFunctionW(void *id);
void *barberFunctionB(void *id);

//Declaration global variable
sem_t seatWaitingRoom; //Define the umber of place in the waiting room  (NBRSEAT)

sem_t barberChairM; //Define the number of barber chair man
sem_t barberChairW; //Define the number of barber chair woman
sem_t barberChairB; //Define the number of barber chair both

sem_t barberAwakeM; //Define if the barber is awake (1==yes ; 0==no)
sem_t barberAwakeW; //Define if the barber is awake (1==yes ; 0==no)
sem_t barberAwakeB; //Define if the barber is awake (1==yes ; 0==no)

sem_t freeChair; //Use to make client wait until the barber finish is job

bool NMC = false; //NMC stand for No More Client, this variale is to end the program when all clients are served
int attribute = 0; //Use to define the sex of the client (0==Man ; 1==Woman)

//==========================================================================================
//==========================================================================================
//=========================================== Main =========================================
//==========================================================================================
//==========================================================================================
int main()
{
    srand(time(NULL));

    printf("Number of clients: %d\n", NBRCLIENT);
    printf("Number of seats in the waiting room: %d\n\n", NBRSEAT);
    printf("Number of barbers:\n\tMan: %d\n\tWomen: %d\n\tBoth: %d\n\n", NBRBARBERM, NBRBARBERW, NBRBARBERB);

//Declaration of variable
    pthread_t barberThreadM[NBRBARBERM];
    pthread_t barberThreadW[NBRBARBERW];
    pthread_t barberThreadB[NBRBARBERB];

    pthread_t clientThread[NBRCLIENT];

    int idArrayClient[NBRCLIENT];

    int idArrayBarberM[NBRBARBERM];
    int idArrayBarberW[NBRBARBERW];
    int idArrayBarberB[NBRBARBERB];

//Initialization of the arrays of ID
    for (int i = 0 ; i < NBRCLIENT ; i++)
    {
        idArrayClient[i] = i;
    }

    for (int i = 0 ; i < NBRBARBERM ; i++)
    {
        idArrayBarberM[i] = i;
    }

    for (int i = 0 ; i < NBRBARBERW ; i++)
    {
        idArrayBarberW[i] = i;
    }

    for (int i = 0 ; i < NBRBARBERB ; i++)
    {
        idArrayBarberB[i] = i;
    }

//Initialization of the semaphores
    sem_init(&seatWaitingRoom, 0, NBRSEAT);

    sem_init(&barberChairM, 0, NBRBARBERM);
    sem_init(&barberChairW, 0, NBRBARBERW);
    sem_init(&barberChairB, 0, NBRBARBERB);

    sem_init(&barberAwakeM, 0, 0);
    sem_init(&barberAwakeW, 0, 0);
    sem_init(&barberAwakeB, 0, 0);

    sem_init(&freeChair, 0, 0);

//Creation of one barbers
    for (int i = 0 ; i < NBRBARBERM ; i++)
    {
        pthread_create(&barberThreadM[i], NULL, barberFunctionM, &idArrayBarberM[i]);
    }

    for (int i = 0 ; i < NBRBARBERW ; i++)
    {
        pthread_create(&barberThreadW[i], NULL, barberFunctionW, &idArrayBarberW[i]);
    }

    for (int i = 0 ; i < NBRBARBERB ; i++)
    {
        pthread_create(&barberThreadB[i], NULL, barberFunctionB, &idArrayBarberB[i]);
    }

    sleep(1);

//Creation of clients
    for (int i = 0 ; i < NBRCLIENT ; i++)
    {
        pthread_create(&clientThread[i], NULL, clientFunction, &idArrayClient[i]);
    }

//Join all client threads
    for (int i = 0 ; i < NBRCLIENT ; i++)
    {
        pthread_join(clientThread[i], NULL);
    }

//When all the client are served
    printf("========================================\n");
    NMC = 1;
    for (int i = 0 ; i < NBRBARBERM ; i++)
    {
        sem_post(&barberAwakeM);
        pthread_join(barberThreadM[i], NULL);
    }

    for (int i = 0 ; i < NBRBARBERW ; i++)
    {
        sem_post(&barberAwakeW);
        pthread_join(barberThreadW[i], NULL);
    }

    for (int i = 0 ; i < NBRBARBERB ; i++)
    {
        sem_post(&barberAwakeB);
        pthread_join(barberThreadB[i], NULL);
    }
}
//==========================================================================================
//==========================================================================================
//================================== Declaration of client =================================
//==========================================================================================
//==========================================================================================
void *clientFunction(void *id)
{
    int ID = *(int *) id;
    int freeSeats;
    char sex;

    int freeChairM;
    int freeChairW;
    int freeChairB;

    bool BB;

//Define sex of the client
    if (attribute == 0)
    {
        attribute = 1;
        sex = 'W';
    }
    else
    {
        attribute = 0;
        sex = 'M';
    }

//Client leave home and go to the barber shop
    printf("Client [%c%d] leaving home.\n", sex, ID);
    sleep(rand()%4);
    printf("Client [%c%d] enter the barber shop.\n", sex, ID);

//Can the client seat in the waiting room ?
    sem_getvalue(&seatWaitingRoom, &freeSeats);

    if (freeSeats != 0)
    {
    //Client is in the waiting room
        sem_wait(&seatWaitingRoom);
        printf("Client [%c%d] is in the waiting room.\n", sex, ID);

        if (sex == 'M') //Client is a Man
        {
            do
            {
                sem_getvalue(&barberChairM, &freeChairM);
                sem_getvalue(&barberChairB, &freeChairB);

                if(freeChairM != 0)
                {
                    BB = false; //Remember that barber is not both

                //Client wait for an available barber chair
                    sem_wait(&barberChairM);

                //Free a seat in the waiting room
                    sem_post(&seatWaitingRoom);

                //Barber is sleeping so client waking him up
                    printf("Client [%c%d] wake up the barber.\n", sex, ID);
                    sem_post(&barberAwakeM);
                }
                else if (freeChairB != 0)
                {
                    BB = true; //Remember that barber is both

                //Client wait for an available barber chair
                    sem_wait(&barberChairB);

                //Free a seat in the waiting room
                    sem_post(&seatWaitingRoom);

                //Barber is sleeping so client waking him up
                    printf("Client [%c%d] wake up the barber.\n", sex, ID);
                    sem_post(&barberAwakeB);
                }
            }while(freeChairM == 0 && freeChairB == 0);
        }
        else //Client is a Woman
        {
            do
            {
                sem_getvalue(&barberChairW, &freeChairW);
                sem_getvalue(&barberChairB, &freeChairB);

                if(freeChairW != 0)
                {
                    BB = false; //Remember that barber is not both

                //Client wait for an available barber chair
                    sem_wait(&barberChairW);

                //Free a seat in the waiting room
                    sem_post(&seatWaitingRoom);

                //Barber is sleeping so client waking him up
                    printf("Client [%c%d] wake up the barber.\n", sex, ID);
                    sem_post(&barberAwakeW);
                }
                else if (freeChairB != 0)
                {
                    BB = true; //Remember that barber is both

                //Client wait for an available barber chair
                    sem_wait(&barberChairB);

                //Free a seat in the waiting room
                    sem_post(&seatWaitingRoom);

                //Barber is sleeping so client waking him up
                    printf("Client [%c%d] wake up the barber.\n", sex, ID);
                    sem_post(&barberAwakeB);
                }
            }while(freeChairW == 0 && freeChairB == 0);
        }

    //Wait the end of the barber's work
        sem_wait(&freeChair);

    //Free the barber chair
        if (BB == true)
        {
            sem_post(&barberChairB); //Free a chair for both
        }
        else
        {
            if (sex = 'M')
            {
                sem_post(&barberChairM); //Free a chair for Man
            }
            if (sex = 'W')
            {
                sem_post(&barberChairW); //Free a chair for Woman
            }
        }
        printf("Customer [%c%d] leaving barber shop.\n", sex, ID);
    }
    else
    {
        printf("No more seat available in the waiting. The client [%c%d] go home.\n", sex, ID);
    }
}
//==========================================================================================
//==========================================================================================
//================================= Declaration of barbers =================================
//==========================================================================================
//==========================================================================================
void *barberFunctionM(void *id)
{
    int ID = *(int *) id;
    char typeClient = 'M';

    while (!NMC)
    {
        printf("========== The barber [%c%d] is sleeping.==========\n", typeClient, ID);
        sem_wait(&barberAwakeM);

        if (!NMC)
        {
        //Barber take care of the client
            printf("Barber [%c%d] is cutting hair...\n", typeClient, ID);
            sleep(rand()%7);
            printf("Barber [%c%d] has finished is job.\n", typeClient, ID);

        //Release the client
            sem_post(&freeChair);
        }
        else
        {
            printf("End of the day.\nThe barber [%c%d] wake up and go home.\n", typeClient, ID);
            sem_post(&barberAwakeM);
        }
    }
}



void *barberFunctionW(void *id)
{
    int ID = *(int *) id;
    char typeClient = 'W';

    while (!NMC)
    {
        printf("========== The barber [%c%d] is sleeping.==========\n", typeClient, ID);
        sem_wait(&barberAwakeW);

        if (!NMC)
        {
        //Barber take care of the client
            printf("Barber [%c%d] is cutting hair...\n", typeClient, ID);
            sleep(rand()%10);
            printf("Barber [%c%d] has finished is job.\n", typeClient, ID);

        //Release the client
            sem_post(&freeChair);
        }
        else
        {
            printf("End of the day.\nThe barber [%c%d] wake up and go home.\n", typeClient, ID);
            sem_post(&barberAwakeW);
        }
    }
}



void *barberFunctionB(void *id)
{
    int ID = *(int *) id;
    char typeClient = 'B';

    while (!NMC)
    {
        printf("========== The barber [%c%d] is sleeping.==========\n", typeClient, ID);
        sem_wait(&barberAwakeB);

        if (!NMC)
        {
        //Barber take care of the client
            printf("Barber [%c%d] is cutting hair...\n", typeClient, ID);
            sleep(rand()%8);
            printf("Barber [%c%d] has finished is job.\n", typeClient, ID);

        //Release the client
            sem_post(&freeChair);
        }
        else
        {
            printf("End of the day.\nThe barber [%c%d] wake up and go home.\n", typeClient, ID);
            sem_post(&barberAwakeB);
        }
    }
}