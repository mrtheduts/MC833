/* Nome: Eduardo Moreira Freitas de Souza   */
/* RA: 166779                               */
/* Projeto 1 - MC833                        */

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

#include <sys/time.h>

#define PORT "22222" // A porta utilizada na conexao

#define MAXDATASIZE 1024 // Numero maximo de bytes que podem ser enviados 

// Verifica se eh IPV4 ou IPV6
void *get_in_addr (struct sockaddr *sa) {

    if (sa->sa_family == AF_INET) 
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

size_t digit_count(int num) {
    return snprintf(NULL, 0, "%d", num);
}

int recv_msg (int socketfd) {

    int bytes_recv_so_far = 0,
        bytes_recv_one_msg,
        total_bytes_to_recv = -1,
        string_jmp;

    char buffer[MAXDATASIZE] = {0};
   
    do{

        if (total_bytes_to_recv == -1) {

            if ((bytes_recv_one_msg = recv(socketfd, buffer, MAXDATASIZE - 1, 0)) <= 0)
                return bytes_recv_one_msg;

            sscanf(buffer, "%d", &total_bytes_to_recv);

            string_jmp = digit_count(total_bytes_to_recv) + 1;
            buffer[bytes_recv_one_msg] = 0;

            printf("%s", buffer + string_jmp);
        }

        else {

            if ((bytes_recv_one_msg = recv(socketfd, buffer, MAXDATASIZE - 1, 0)) <= 0)
                return bytes_recv_one_msg;

            buffer[bytes_recv_one_msg] = 0;

            printf("%s", buffer);
        }


        bytes_recv_so_far += bytes_recv_one_msg;

    }while(bytes_recv_so_far < total_bytes_to_recv);


    return bytes_recv_so_far;
}

int main (int argc, char *argv[]) {

    int socketfd, num_bytes;
    char *buffer;
    unsigned int buffer_size = MAXDATASIZE;

    struct addrinfo hints, *server_info, *pointer;
    char ip_string[INET6_ADDRSTRLEN];
    int error;

    struct timeval tv1, tv2;
    FILE *log = fopen(argv[2], "w");

    if (argc != 3) {

        fprintf(stderr, "uso: ./cliente <hostname do servidor> <nome do arquivo de log>\n");
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

    while((num_bytes = recv_msg(socketfd)) > 0) {

        gettimeofday(&tv2, NULL);
        fprintf(log, "%s -> %ld\n", buffer, (tv2.tv_sec - tv1.tv_sec)*1000000 + (long)(tv2.tv_usec - tv1.tv_usec));

        if (strcmp(buffer, "sair") == 0)
            shutdown(socketfd, SHUT_RDWR);

        else {

            fgets(buffer, MAXDATASIZE, stdin);
            buffer[strlen(buffer) - 1] = 0;
            send(socketfd, buffer, strlen(buffer), 0);

            gettimeofday(&tv1, NULL);
        }
    }

    if (num_bytes == -1) {

        perror("recv");
        exit(1);
    }

    printf("cliente: conexao encerrada pelo servidor\n");

    close(socketfd);
    fclose(log);

    return 0;
}
