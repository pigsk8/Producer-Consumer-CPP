//Leonard Mendoza CI 20060359
//Juan Scrocchi CI 20425629

#ifdef __linux__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define tam_buf 50

typedef struct {
    int buf[tam_buf]; 
    int pos; 
    pthread_mutex_t mutex; 
    pthread_cond_t produce; 
    pthread_cond_t consume; 
} buffer_t;

int t=0;

void* productor(void *arg) {
    buffer_t *buffer = (buffer_t*)arg;
    int valorp;
    float timep;

    while(1) {

        valorp = rand()%20;
        timep=(float)valorp/10;
        sleep(timep);

        pthread_mutex_lock(&buffer->mutex);

        if(buffer->pos == tam_buf) { 
            pthread_cond_wait(&buffer->produce, &buffer->mutex);
        }


        t = t+1;

        buffer->buf[buffer->pos] = t;

        printf("Producido: %d\n",buffer->buf[buffer->pos]);
        ++buffer->pos;
        pthread_cond_signal(&buffer->consume);
        pthread_mutex_unlock(&buffer->mutex);

    }

    return NULL;
}


void* consumidor(void *arg) {
    buffer_t *buffer = (buffer_t*)arg;
    int valorc;
    float timec;
    while(1) {

        valorc = rand()%20;
        timec=(float)valorc/10;
        sleep(timec);	

        pthread_mutex_lock(&buffer->mutex);

        if(buffer->pos == 0) { 
            pthread_cond_wait(&buffer->consume, &buffer->mutex);
        }

        --buffer->pos;
        printf("Consumido: %d\n", buffer->buf[buffer->pos]); 

        t=buffer->buf[buffer->pos]-1; 
        pthread_cond_signal(&buffer->produce);
        pthread_mutex_unlock(&buffer->mutex);

    }

    return NULL;
}

int main(int argc, char *argv[]) {


    buffer_t buffer = {
        .pos = 0,
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .produce = PTHREAD_COND_INITIALIZER,
        .consume = PTHREAD_COND_INITIALIZER
    };

    pthread_t prod, cons;
    pthread_create(&prod, NULL, productor, (void*)&buffer);
    pthread_create(&cons, NULL, consumidor, (void*)&buffer);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    return 0;

}


#elif _WIN32

#include <windows.h>
#include <process.h>
#include <iostream>
#include <time.h>
using namespace std;


#define NUM_HILOS 2
#define	TAM_BUF	10


DWORD WINAPI Productor(LPVOID param);
DWORD WINAPI Consumidor(LPVOID param);


HANDLE	mutex = CreateSemaphore(0, 1, 1, 0);
HANDLE	space = CreateSemaphore(0, TAM_BUF, TAM_BUF, 0);
HANDLE	element = CreateSemaphore(0, 0, TAM_BUF, 0);


int	buffer[TAM_BUF];
int pos=0;
int	item=0;  


int main()
{
    DWORD	ThreadId;
    HANDLE	threads[NUM_HILOS];
    int	params[] = { 0, 1 };

    threads[0] = CreateThread(0, 0, Productor, &params[0], 0, &ThreadId);
    threads[1] = CreateThread(0, 0, Consumidor, &params[1], 0, &ThreadId);

    for (int i = 0; i < NUM_HILOS; i++)
        WaitForSingleObject(threads[i], INFINITE);
} 


DWORD WINAPI Productor(LPVOID param)
{

    float valorp;
    float timep;


    for (int i=0;i<1000;i++)
    {
        valorp = rand()%25;
        timep=(float)valorp/10;
        Sleep(timep);


        WaitForSingleObject(space, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        item++;
        buffer[pos] = item;
        cout << "Producido: " << buffer[pos] << endl;

        pos++;

        ReleaseSemaphore(mutex, 1, 0);
        ReleaseSemaphore(element, 1, 0);

    }

    return 0;
}



DWORD WINAPI Consumidor(LPVOID param)
{

    float valorc;
    float timec;


    for (int i=0;i<1000;i++)
    {
        valorc = rand()%20;
        timec=(float)valorc/10;
        Sleep(timec);

        WaitForSingleObject(element, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        pos--;
        cout << "consumido: "<< buffer[pos] << endl;
        item = buffer[pos]-1;


        ReleaseSemaphore(mutex, 1, 0);
        ReleaseSemaphore(space, 1, 0);

    }

    return 0;
}

#endif


