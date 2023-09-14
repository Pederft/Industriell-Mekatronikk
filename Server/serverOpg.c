#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10 // maksimalt antall klienter som serveren kan håndtere
#define BUFFER_SIZE 1024 // størrelse på buffer for meldinger

// struktur for klientinformasjon
typedef struct {
    int socket_fd;
    struct sockaddr_in address;
    pthread_t thread;
} client_info_t;

// funksjon som håndterer klientkommunikasjon
void *handle_client(void *arg) {
    client_info_t *client_info = (client_info_t *)arg;
    int socket_fd = client_info->socket_fd;
    char buffer[BUFFER_SIZE];
    int num_bytes;

    // håndter klientkommunikasjon
    while ((num_bytes = recv(socket_fd, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[num_bytes] = '\0';
        printf("Melding mottatt fra klient %d: %s\n", socket_fd, buffer);

        // send melding tilbake til klienten
        send(socket_fd, buffer, strlen(buffer), 0);
    }

    // klienten har koblet fra, lukk socket
    close(socket_fd);
    printf("Klient %d koblet fra.\n", socket_fd);

    // frigjør minne for klientinfo
    free(client_info);

    return NULL;
}

int main(int argc, char **argv) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    pthread_t threads[MAX_CLIENTS];
    client_info_t *client_info;
    int i = 0;

    // opprett socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Feil ved oppretting av socket");
        exit(EXIT_FAILURE);
    }

    // konfigurer serveradresse
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8888);

    // bind socket til serveradresse
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Feil ved binding av socket");
        exit(EXIT_FAILURE);
    }

    // start serveren og vent på klientforbindelser
    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("Feil ved start av server");
        exit(EXIT_FAILURE);
    }

    printf("Serveren er klar til å motta klientforbindelser.\n");

    // aksepter klientforbindelser og opprett tråder for hver klient
    while (1) {
        // aksepter klientforbindelse
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size)) == -1) {
            perror("Feil ved akseptering av klientforbindelse");
            continue;
        }

        printf("Ny klient koblet til. Socket FD: %d, IP-adresse: %s, port: %d\n", 
            client_fd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // opprett klientinfo
         // opprett klientinfo
        client_info = (client_info_t *)malloc(sizeof(client_info_t));
        client_info->socket_fd = client_fd;
        client_info->address = client_addr;

        // opprett tråd for klienten og håndter klientkommunikasjon i tråden
        if (pthread_create(&(client_info->thread), NULL, &handle_client, (void *)client_info) != 0) {
            perror("Feil ved opprettelse av klienttråd");
            free(client_info);
        }

        // lagre klientinfo og tråd i array for senere håndtering
        threads[i++] = client_info->thread;

        // reset i ved maks antall klienter
        i %= MAX_CLIENTS;
    }

    // vent på trådene
    for (int i = 0; i < MAX_CLIENTS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}