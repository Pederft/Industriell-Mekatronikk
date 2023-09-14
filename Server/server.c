#include <stdio.h>     // perror, printf
#include <stdlib.h>    // exit, atoi
#include <unistd.h>    // read, write, close
#include <arpa/inet.h> // sockaddr_in, AF_INET, SOCK_STREAM, INADDR_ANY, socket etc...
#include <string.h>    // memset
#include <stdbool.h>   // true, false
#include <pthread.h>

void *handle_client(void *arg);


int main(int argc, char const *argv[]) {

    int port = 65430;
    int server_sock, client_sock;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len;
    pthread_t tid;

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("[-]Cannot create socket");
        exit(1);
    }

    // Bind socket to address and port
    memset(&server_address, '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = port;
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("[-]Bind failed");
        exit(2);
    }

    // Listen for incoming connections
    if (listen(server_sock, 5) < 0) {
        perror("[-]Listen failed");
        exit(3);
    }
    printf("[+]Server is listening on port %d\n", port);

    while (1) {
        // Accept incoming connection
        client_address_len = sizeof(client_address);
        client_sock = accept(server_sock, (struct sockaddr *) &client_address, &client_address_len);
        if (client_sock < 0) {
            perror("[-]Accept failed");
            exit(4);
        }
        printf("[+]Client connected from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        // Spawn a new thread to handle the client
        if (pthread_create(&tid, NULL, handle_client, (void *) &client_sock) < 0) {
            perror("[-]Thread creation failed");
            exit(5);
        }
        printf("[+]Thread created for client %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    }

    return 0;
}


void *handle_client(void *arg) {
    int client_sock = *(int *) arg;
    char buffer[1024];
    int len;

    while ((len = recv(client_sock, buffer, sizeof(buffer), 0)) > 0) {
        if (send(client_sock, buffer, len, MSG_NOSIGNAL) < 0) {
            perror("[-]Send failed");
            break;
        }
    }

    printf("[-]Connection closed by client\n");
    close(client_sock);

    return NULL;
}