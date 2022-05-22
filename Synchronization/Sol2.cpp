#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

using namespace std;

unsigned int ms = 1000000;

pthread_cond_t cons,prod;
pthread_mutex_t mutex;

int n = 10;

int BUFFER[10];             // Buffer size same as 'n' above
int Consumed = -1;          // pointer for last comsumed item in buffer
int Produced = -1;          // pointer for last produced item in buffer

/*
At point A will check randomly where it should produce or not.
At point C Producer sleeps randomly between 1-6 seconds before starting again, it shows random time for any I/O to take place.
*/
void* Producer(void *)
{
    int SLEEP = 0;
    while (1)
    {
        int a = rand()%2;
        if(a){                                                  // Point A
            pthread_mutex_lock(&mutex);
            pthread_cond_wait(&prod, &mutex);
            BUFFER[Produced%n] = ++Produced;
            cout<<"Produced produced: "<<Produced<<endl;
            pthread_mutex_unlock(&mutex);
            SLEEP = (rand()%6);                                 // Point C
            usleep(SLEEP*ms);
        }
    }
}

/*
At point D it sleeps for 2 seconds to portray the processing for the consumed item.
*/
void* Consumer(void *)
{
    int Get = 0;
    while (1)
    {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cons, &mutex);
        Get = BUFFER[++Consumed%n];
        cout<<"Consumer consumed: "<<Get<<endl;
        pthread_mutex_unlock(&mutex);
        usleep(2*ms);                                           // Point D
    }
}
/*
If producer reach limit this thread will allow the consumer to continue.
This thread also allows producer to continue if he hasn't filled all the buffer which isn't consumed.
*/
void* SEM(void *){
    while(1){
        pthread_mutex_lock(&mutex);
        if(Consumed<Produced){
            pthread_cond_signal(&cons);
        }if(Consumed+n-2>Produced){
            pthread_cond_signal(&prod);
        }
        pthread_mutex_unlock(&mutex);
    }
}

/*
NOTE: Even if Buffer is of small size it may continue till eternity because of modulus used...
*/
int main()
{

    pthread_t id1, id2, id3;
    pthread_cond_init(&prod, NULL);
    pthread_cond_init(&cons, NULL);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&id1, NULL, &Producer, NULL);
    pthread_create(&id2, NULL, &Consumer, NULL);
    pthread_create(&id3, NULL, &SEM, NULL);

    pthread_join(id1, NULL);
    pthread_join(id2, NULL);
    pthread_join(id3, NULL);

    return 0;
}
