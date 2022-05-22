#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

using namespace std;

unsigned int ms = 1000000;

sem_t sem1;
sem_t sem2;

int n = 10;

int BUFFER[10];             // Buffer size same as 'n' above
int Consumed = -1;          // pointer for last comsumed item in buffer
int Produced = -1;          // pointer for last produced item in buffer
bool cons = false;          // Check if the consumer is in process of consumption
bool prod = false;          // Check if the producer is in process of production

/*
At point A will check randomly where it should produce or not.
At point B Producer after producing check if comsumption is not at the end if not it allows the consumption.
At point C Producer sleeps randomly between 1-6 seconds before starting again, it shows random time for any I/O to take place.
*/
void* Producer(void *)
{
    int SLEEP = 0;
    while (1)
    {
        sem_wait(&sem1);
        prod = false;
        int a = rand()%2;
        if(a){                                                  // Point A
            BUFFER[Produced%n] = ++Produced;
            prod = true;
            cout<<"Produced produced: "<<Produced<<endl;
            if(Consumed!=Produced){                             // Point B
                sem_post(&sem2);
            }
            SLEEP = (rand()%6);                                 // Point C
            usleep(SLEEP*ms);
        }prod = true;
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
        sem_wait(&sem2);
        cons = false;
        Get = BUFFER[++Consumed%n];
        cons = true;
        cout<<"Consumer consumed: "<<Get<<endl;
        usleep(2*ms);                                           // Point D
    }
}
/*
If producer reach limit this thread will allow the consumer to continue.
This thread also allows producer to continue if he hasn't filled all the buffer which isn't consumed.
*/
void* SEM(void *){
    while(1){
        if(Consumed+2<Produced){
            if(cons){
                sem_post(&sem2);
            }
        }if(Consumed+n-2>Produced){
            if(prod){
                sem_post(&sem1);
            }
        }
    }
}

/*
NOTE: Even if Buffer is of small size it may continue till eternity because of modulus used...
*/
int main()
{

  pthread_t id1, id2, id3;
  sem_init(&sem1, 0, 1);
  sem_init(&sem2, 0, 0);

  pthread_create(&id1, NULL, &Producer, NULL);
  pthread_create(&id2, NULL, &Consumer, NULL);
  pthread_create(&id3, NULL, &SEM, NULL);

  pthread_join(id1, NULL);
  pthread_join(id2, NULL);
  pthread_join(id3, NULL);

  return 0;
}
