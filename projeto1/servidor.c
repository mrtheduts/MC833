/* Nome: Eduardo Moreira Freitas de Souza   */
/* RA: 166779                               */
/* Projeto 1 - MC833                        */
/* Mandar para edmundo@ic.unicamp.br        */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "22222"  // Porta escolhida para a utilizacao do servidor

#define BACKLOG 10	 // Quantos clientes vao esperar na fila para conectar

void HandleClient(int fd);

void SignalChildHandler (int s) {
    
    (void)s;

    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

// Verifica se eh IPV4 ou IPV6
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main () {


    // Esperar conexões em sockfd e conectar em new_fd
    int socketfd, new_fd;

    struct addrinfo hints, *server_info, *pointer;
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    int error;
    int yes = 1;
    char ip_string[INET6_ADDRSTRLEN];
    struct sigaction signal_action;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((error= getaddrinfo(NULL, PORT, &hints, &server_info)) != 0) {

        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
        return 1;
    }


    // Verifica se algum ponteiro sem erro para a preparacao da socket e o binding
    for (pointer = server_info; pointer != NULL; pointer = pointer->ai_next) {

        if ((socketfd = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol)) == -1) {

            perror("server: socket");
            continue;
        }

        if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {

            perror("setsockopt");
            exit(1);
        }

        if (bind(socketfd, pointer->ai_addr, pointer->ai_addrlen) == -1) {

            close(socketfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(server_info);

    // Nao foi possivel encontrar um socket valido
    if (pointer == NULL) {

        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    // Nao foi possivel configurar o listen()
    if (listen(socketfd, BACKLOG) == -1) {

        perror("listen");
        exit(1);
    }

    signal_action.sa_handler = SignalChildHandler;
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &signal_action, NULL) == -1) {

        perror("sigaction");
        exit(1);
    }

    printf("Servidor: esperando por conexoes...\n");

    // Laco principal
    while(1) {

        sin_size = sizeof(client_addr);
        new_fd = accept(socketfd, (struct sockaddr *)&client_addr, &sin_size);

        if (new_fd == -1) {

            perror("accept");
            continue;
        }

        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), ip_string, sizeof ip_string);
        printf("Servidor: nova conexao de %s\n", ip_string);

        if (!fork()) {

            close(socketfd); // filho nao precisa escutar a socket

            HandleClient(new_fd);

            close(new_fd);
            exit(0);
        }

        close(new_fd); // pai nao precisa da nova porta
    }

    return 0;
}
