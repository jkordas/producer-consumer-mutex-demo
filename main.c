#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

int *buffer;
int bufferSize; //agr1
int taken = 0;
int producerMaxSleep;//arg2
int consumerMaxSleep;//arg3

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//display operation and taken variable modifier locker

pthread_mutex_t mutexP = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condP = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutexC = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condC = PTHREAD_COND_INITIALIZER;

void *Produce(void *arg);

void *Consume(void *arg);

void Display(int n);

int main(int argc, char *argv[]) {

    if (argc < 4) {
        printf("Not enough arguments\n 1. Buffer Size\n 2. Producer Max Sleep [ms]\n 3. Consumer Max Sleep[ms]\n");
        return 0;
    } else {
        bufferSize = atoi(argv[1]);
        buffer = malloc(bufferSize * sizeof(int));
        producerMaxSleep = atoi(argv[2]);
        consumerMaxSleep = atoi(argv[3]);
    }
    srand(time(NULL));

    int i;
    for (i = 0; i < bufferSize; ++i) //prepare empty array
        buffer[i] = 0;

    pthread_t producer, consumer;

    if (pthread_create(&producer, NULL, Produce, NULL)) {
        printf("Create thread error\n");
        abort();
    }

    if (pthread_create(&consumer, NULL, Consume, NULL)) {
        printf("Create thread error\n");
        abort();
    }
    if (pthread_join(producer, NULL)) {
        printf("Join thread error\n");

    }
    if (pthread_join(consumer, NULL)) {
        printf("Join thread error\n");

    }

    free(buffer);

    return 0;
}

void *Produce(void *arg) {
    int counter = 1000;
    int indexProducer = 0;
    int sleepTime;

    while (1) {
        sleepTime = rand() % producerMaxSleep;  //[0 - producerMaxSleep)

        pthread_mutex_lock(&mutexP);
        if (taken == bufferSize) {
            pthread_cond_wait(&condP, &mutexP); //Wait for a place in buffer
        }
        pthread_mutex_unlock(&mutexP);

        buffer[indexProducer] = counter++;      //Element produced

        pthread_mutex_lock(&mutex);
            taken++;                            //Adjust elements number in buffer
        pthread_mutex_unlock(&mutex);

        indexProducer = (indexProducer + 1) % bufferSize;

        pthread_mutex_lock(&mutex);
            printf("%6d ->  ", counter - 1);
            Display(bufferSize);
            printf("\n\n");
        pthread_mutex_unlock(&mutex);

        pthread_cond_signal(&condC);            //Buffer is not empty. I have added something.

        sleep(sleepTime / 1000);
    }

    return NULL;
}

void *Consume(void *arg) {
    int indexConsumer = 0;
    int sleepTime;
    int tmp;

    while (1) {
        sleepTime = rand() % consumerMaxSleep;

        pthread_mutex_lock(&mutexC);
            if (taken == 0) {
                pthread_cond_wait(&condC, &mutexC);     //Wait until buffer is not empty
            }
        pthread_mutex_unlock(&mutexC);

        tmp = buffer[indexConsumer];
        buffer[indexConsumer] = 0;                      //Element consumed

        pthread_mutex_lock(&mutex);
            taken--;                                    //Adjust elements number in buffer
        pthread_mutex_unlock(&mutex);

        indexConsumer = (indexConsumer + 1) % bufferSize;

        pthread_mutex_lock(&mutex);
            printf("           ");
            Display(bufferSize);
            printf("->  %6d  \n\n", tmp);
        pthread_mutex_unlock(&mutex);

        pthread_cond_signal(&condP);                    //There is empty space in buffer. You can add something.


        sleep(sleepTime / 1000);
    }

    return NULL;
}

void Display(int n) {    //display buffer

    int i;
    for (i = 0; i < n; ++i) {
        printf("| %6d ", buffer[i]);
    }
    printf("|");
}