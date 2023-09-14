#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_ITERATIONS 20

int var1 = 0;
int var2 = 0;
volatile int running = 1; //Bruker volatile for å sørge for synkronisering mellom trådene, 
pthread_mutex_t mutex;    //Uten "volatile" vil tråd1 lage en lokal kopi av variabelen og dermed alltid
                          //kjøre fordi den alltid er 1, og ikke blir påvirket av tråd2 som gir running=0.

void* thread1_func(void* arg) {
    while (running) {
        pthread_mutex_lock(&mutex);  // låser mutex
        var1 = var1 + 1;
        var2 = var1;
        pthread_mutex_unlock(&mutex);  // unlocker mutex
    }
    pthread_exit(NULL);
}

void* thread2_func(void* arg) {
    for (int i = 1; i <= NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);  // låser mutex
        printf("Number 1 is %d, number 2 is %d\n", var1, var2);
        pthread_mutex_unlock(&mutex);  // unlocker mutex
        usleep(1000000);
    }
    running = 0;
    pthread_exit(NULL);
}

int main() {
    pthread_t thread1, thread2;
    int ret;
    // Initialiserer mutex'en
    pthread_mutex_init(&mutex, NULL);

    // Lager tråd 1 og 2
    ret = pthread_create(&thread1, NULL, thread1_func, NULL);
    ret = pthread_create(&thread2, NULL, thread2_func, NULL);

    // Venter til de er ferdige
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    // Ødelegger mutexen til slutt
    pthread_mutex_destroy(&mutex);

    return 0;
}