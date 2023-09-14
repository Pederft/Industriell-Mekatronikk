#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int global_variabel = 0;

void *thread_func(void *arg) {
    int lokal_variabel = 0;

    for (int i = 0; i < 10; i++) {
        global_variabel++;
        lokal_variabel++;
    }
    printf("Thread %d: local_var = %d, global_var = %d\n", (int)arg, lokal_variabel, global_variabel);
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    //Lager to threads, der hver av dem kjører gjennom funksjonen
    pthread_create(&thread1, NULL, thread_func, (void*)1);  
    pthread_create(&thread2, NULL, thread_func, (void*)2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    //Da begge threadsene har tilgang til den globale variabelen vil begge iterere gjennom den samme
    printf("Siste: global_variabel = %d\n", global_variabel);

    return 0;
}