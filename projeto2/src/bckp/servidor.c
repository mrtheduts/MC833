/* Nome: Eduardo Moreira Freitas de Souza   */
/* RA: 166779                               */
/* Projeto 1 - MC833                        */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include <mongoc/mongoc.h>
#include <inttypes.h>
#include <math.h>
#include <sys/time.h>

#define PORT "22222"  // Porta escolhida para a utilizacao do servidor

#define BACKLOG 10	 // Quantos clientes vao esperar na fila para conectar

// Numero maximo de bytes que podem ser recebidos pelo cliente
#define MAXDATASIZE 1024 

size_t digit_count(int num) {
    return snprintf(NULL, 0, "%d", num);
}

void send_msg (int fd, char *msg, int size_msg,
               struct sockaddr_storage *client_addr, socklen_t sin_size) {

    char *pointer = NULL, *buffer;
    int i;

    size_msg = size_msg + digit_count(size_msg) + 2;
    buffer = (char *)calloc(digit_count(size_msg) + size_msg, sizeof(char));

    sprintf(buffer, "%u\n", size_msg);
    strcat(buffer, msg);


    for (i = size_msg, pointer = buffer;
         i > 0;
         i -= (MAXDATASIZE - 1), pointer = pointer + MAXDATASIZE - 1) {

        if (i > 1023)
            sendto(fd, pointer, MAXDATASIZE - 1, 0,
                  (struct sockaddr *)client_addr, sin_size);

        else
            sendto(fd, pointer, i, 0,
                  (struct sockaddr *)client_addr, sin_size);
    }

    free(buffer);
}

void my_strcat (char **msg, char *str, unsigned int *msg_size) {

    unsigned int str_size;

    str_size = strlen(str);


    if (*msg_size < strlen(*msg) + str_size){

        (*msg_size) += str_size;
        *msg = realloc(*msg, (*msg_size)*sizeof(char));
    }

    strcat(*msg, str);

}

void HandleClient (int socketfd, mongoc_collection_t *collection, FILE *log) {

    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    int num_bytes;
    unsigned int msg_size = MAXDATASIZE;
    char msg_recv[1024], *msg, *str;
    mongoc_cursor_t *cursor;
    bson_t *query;
    bson_t *opts;
    const bson_t *doc;
    bool found;

    struct timeval tv1, tv2;

    msg = (char *)calloc(msg_size, sizeof(char));

    while ((num_bytes = recvfrom(socketfd, msg_recv, MAXDATASIZE-1, 0,
                                 (struct sockaddr *)&client_addr, &sin_size)) > 0) {

        gettimeofday(&tv1, NULL);
        msg_recv[num_bytes] = 0;
        found = false;

        query = bson_new();
        BSON_APPEND_UTF8 (query, "Email", msg_recv);

        opts = BCON_NEW ("projection", "{",
                            "_id", BCON_BOOL(false),
                            "Nome Completo",  BCON_BOOL(true),
                         "}");

        cursor = mongoc_collection_find_with_opts(collection, query,
                                                  opts, NULL);

        msg[0] = 0;

        while (mongoc_cursor_next(cursor, &doc)) {

            found = true;
            str = bson_as_canonical_extended_json (doc, NULL);

            my_strcat(&msg, str, &msg_size);

            bson_free(str);
        }

        bson_destroy (query);
        bson_destroy (opts);
        mongoc_cursor_destroy (cursor);

        if (!found)
            strcpy(msg, "Ninguem com esse email! :(\n\0");

        send_msg(socketfd, msg, msg_size, &client_addr, sin_size);

        gettimeofday(&tv2, NULL);
        fprintf(log, "%s -> %ld\n", msg_recv, (tv2.tv_sec - tv1.tv_sec)*1000000 + (long)(tv2.tv_usec - tv1.tv_usec));
    }

    free(msg);
}

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

int main (int argc, char *argv[]) {

    // Necessario para o banco de dados
    const char *uri_string = "mongodb://localhost:27017";
    mongoc_uri_t *uri;
    mongoc_client_t *client;
    mongoc_database_t *database;
    mongoc_collection_t *collection;
    bson_error_t error;

    // Esperar conexões em sockfd e conectar em new_fd
    int socketfd;

    struct addrinfo hints, *server_info, *pointer;
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    int rv;
    int yes = 1;
    char ip_string[INET6_ADDRSTRLEN], msg_recv[1024];
    struct sigaction signal_action;

    FILE *log;

    if (argc != 2) {

        fprintf(stderr, "uso: ./servidor <nome do arquivo de log>\n");
        exit(1);
    }


    log = fopen(argv[1], "w");

    // Inicializacao do banco de dados
    printf("Iniciando mongoc...\n");
    mongoc_init();

    uri = mongoc_uri_new_with_error (uri_string, &error);
    if (!uri) {
        fprintf (stderr, "failed to parse URI: %s\nerror message:       %s\n", uri_string, error.message);
        return EXIT_FAILURE;
    }

    client = mongoc_client_new_from_uri (uri);

    if (!client)
        return EXIT_FAILURE;

    printf("Conectando ao banco de dados...\n");
    mongoc_client_set_appname (client, "servidor-mc833");
    database = mongoc_client_get_database (client, "projeto_redes");
    collection = mongoc_client_get_collection (client, "projeto_redes", "pessoas");

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &server_info)) != 0) {

        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
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

        inet_ntop(client_addr.ss_family,
                  get_in_addr((struct sockaddr *)&client_addr),
                              ip_string,
                              sizeof ip_string);

        recvfrom(socketfd, msg_recv, MAXDATASIZE, 0,  (struct sockaddr *)&client_addr, &sin_size);


        printf("Servidor: nova cliente de %s\n", ip_string);

        HandleClient(socketfd, collection, log);

        printf("Servidor: cliente de %s lidado!\n", ip_string);
    }

    close(socketfd);

    fclose(log);

    mongoc_collection_destroy (collection);
    mongoc_database_destroy (database);
    mongoc_uri_destroy (uri);
    mongoc_client_destroy (client);
    mongoc_cleanup ();

    return 0;
}
