#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#define THREAD_NUM 5

sem_t semaphore;

void* thread_func(void* arg) {
    printf("\nThread %d is waiting to access a resource.", *(int*)arg);
    sem_wait(&semaphore); // Venter til en resurs blir tilgjengleig fra sephamore'en
    printf("\nThread %d has access to a resource.", *(int*)arg);
    for(int i=0; i<4; i++){
        usleep(10000000);
        printf("\nThread %d says hello", *(int*)arg);
    }
    printf("\nThread %d is releasing the resource: \n",*(int*)arg);
    sem_post(&semaphore); // Release the resource
    return NULL;
}


int main(){
    pthread_t threads[THREAD_NUM];
    int thread_nums[THREAD_NUM] = {1, 2, 3, 4, 5};

    // Om vi bruker flere threads skal vi sette inn "0" i den andre. Vi definerer her at semaphoren skal ha tilgang til 3 resurser
	sem_init(&semaphore, 0, 1);    

    for(int i = 0; i<THREAD_NUM; i++){
        pthread_create(&threads[i], NULL, thread_func, &thread_nums[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&semaphore);
    return 0;
}