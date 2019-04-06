/* Nome: Eduardo Moreira Freitas de Souza   */
/* RA: 166779                               */
/* Projeto 1 - MC833                        */
/* Mandar para edmundo@ic.unicamp.br        */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <time.h>

#define PORT "22222" // A porta utilizada na conexao

#define MAXDATASIZE 1024 // Numero maximo de bytes que podem ser enviados 

// Verifica se eh IPV4 ou IPV6
void *get_in_addr (struct sockaddr *sa) {

    if (sa->sa_family == AF_INET) 
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Aloca o tamanho necessario para receber a mensagem
int recv_msg (int socketfd, char *buffer, unsigned int *buffer_size) {

    unsigned int new_buffer_size;
    int bytes_recv;

    recv(socketfd, buffer, *buffer_size - 1, 0);

    sscanf(buffer, "%u", &new_buffer_size);

    if (new_buffer_size > *buffer_size) {

        *buffer_size = new_buffer_size;
        buffer = realloc(buffer, sizeof(char) * (*buffer_size));
    }

    bytes_recv = recv(socketfd, buffer, *buffer_size - 1, 0);

    return bytes_recv;
}

int main (int argc, char *argv[]) {

    int socketfd, num_bytes;
    char *buffer;
    unsigned int buffer_size = MAXDATASIZE, new_buffer_size = 0;

    struct addrinfo hints, *server_info, *pointer;
    int error;
    char ip_string[INET6_ADDRSTRLEN];

    int64_t i, num_entries;

    if (argc != 2) {

        fprintf(stderr, "uso: hostname do servidor\n");
        exit(1);
    }

    buffer = (char *)calloc(buffer_size, sizeof(char));

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((error = getaddrinfo(argv[1], PORT, &hints, &server_info)) != 0) {

        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
        return 1;
    }

    for(pointer = server_info; pointer != NULL; pointer = pointer->ai_next) {

        if((socketfd = socket(pointer->ai_family,
                              pointer->ai_socktype,
                              pointer->ai_protocol)) == -1) {

            perror("client: socket");
            continue;
    }
        if (connect(socketfd, pointer->ai_addr, pointer->ai_addrlen) == -1) {

            perror("client: connect");
            close(socketfd);
            continue;
        }

        break;
    }

    if (pointer == NULL) {

        fprintf(stderr, "cliente: falha ao conectar\n");
        return 2;
    }


    inet_ntop(pointer->ai_family,
              get_in_addr((struct sockaddr *)pointer->ai_addr),
                           ip_string,
                           sizeof(ip_string));

    printf("cliente: conectando a %s\n", ip_string);

    // Conectou, nao precisa dessas informacoes
    freeaddrinfo(server_info);

    while((num_bytes = recv_msg(socketfd, buffer, &buffer_size)) > 0) {

        buffer[num_bytes] = 0;
        printf("%s", buffer);

        fgets(buffer, MAXDATASIZE, stdin);
        buffer[strlen(buffer) - 1] = 0;
        send(socketfd, buffer, strlen(buffer), 0);

    }

    if (num_bytes == -1) {

        perror("recv");
        exit(1);
    }

    printf("cliente: conexao encerrada pelo servidor\n");

    close(socketfd);

    return 0;
}
