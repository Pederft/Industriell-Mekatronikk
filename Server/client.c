#include <stdio.h>     // perror, printf
#include <stdlib.h>    // exit, atoi
#include <unistd.h>    // write, read, close
#include <arpa/inet.h> // sockaddr_in, AF_INET, SOCK_STREAM, INADDR_ANY, socket etc...
#include <string.h>    // strlen, memset
#include <pthread.h>


void *client_handler(void *);


int main(int argc, char const *argv[]){

    char *server_ip = "127.0.0.1"; //Lokal adresse
    char *buffer = "hello server";
    int  port = 65430;

    int sock;
    struct sockaddr_in server_address;
    socklen_t addr_size;
    int len;


    //Lager en TCP server
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock < 0){
        perror("[-]Cannot create socket");
        exit(1);
    }

    memset(&server_address, '\0', sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port   = port; //htons sorterer nummerene som er lagret, og vil ha "most significant byte" først. Siden vi allerede er i lokal maskin s\u00e5 er verdien allerede i "network byte order" som brukes av TCP/IP protokollen
    server_address.sin_addr.s_addr = inet_addr(server_ip);
    len = sizeof(server_address);

    //connecte til server:
    if(connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        perror("Cannot connect to server");
        exit(2);
    }

    if(write(sock, buffer, strlen(buffer)) < 0){
        perror("Cannot write");
        exit(3);
    }
    
    pthread_t thread;
    int *sock_ptr = malloc(sizeof(*sock_ptr));
    *sock_ptr = sock;

    if(pthread_create(&thread, NULL, client_handler, sock_ptr) != 0) {
        perror("Cannot create client handler thread");
        exit(3);
    }

    while(1) {
        char buffer[1024];
        //printf("Enter message to send to server: ");
        fgets(buffer, 1024, stdin);

        if(write(sock, buffer, strlen(buffer)) < 0){
            perror("Cannot write");
            exit(4);
        }
    }

    pthread_join(thread, NULL);

    free(sock_ptr);
    close(sock);

    /*
    bzero(buffer, 1024);
    strcpy(buffer, "HELLO, THIS IS CLIENT.");
    printf("Client: %s\n", buffer);
    send(sock, buffer, strlen(buffer), 0);

    bzero(buffer, 1024);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);

    close(sock);
    printf("Disconnected from the server.\n\n");
    */
    return 0;
}


void *client_handler(void *sock_ptr) {

    int sock = *(int*)sock_ptr;
    char recv[1024];
    memset(recv, 0, sizeof(recv));

    while(read(sock, recv, sizeof(recv)) > 0) {
        printf("Received from server: %s\n", recv);
        memset(recv, 0, sizeof(recv));
    }
    pthread_exit(NULL);
}