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
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include <mongoc/mongoc.h>
#include <inttypes.h>

#define PORT "22222"  // Porta escolhida para a utilizacao do servidor

#define BACKLOG 10	 // Quantos clientes vao esperar na fila para conectar

// Numero maximo de bytes que podem ser recebidos pelo cliente
#define MAXDATASIZE 1024 

// Estados possiveis para o servidor
#define DISCONNECTED    0
#define INVALID_EMAIL   -1
#define INVALID_PASSWD  -2
#define FAILED          -3
#define PASSWD_REQ      1
#define VERIFY_PASSWD   2
#define LOGGED          3

#define OPT_1           11
#define OPT_2           12
#define OPT_3           13
#define OPT_4           14
#define OPT_5           15
#define OPT_6           16
#define OPT_7           17


void send_msg(int fd, char *msg){

    unsigned int size_msg;
    char msg_with_size[10] = {0};

    size_msg = strlen(msg);
    sprintf(msg_with_size, "%u", size_msg);
    printf("msg_w_size: %u\n", size_msg);
    printf("Msg: |%s|\n", msg);

    if (send(fd, msg_with_size, strlen(msg_with_size), 0) == -1)
            perror("send");

    if (send(fd, msg, size_msg, 0) == -1)
            perror("send");

    printf("Tudo enviado!\n");
}

char *set_initial_msg(int *state){

    *state = LOGGED;

    return "\n\nAs seguintes operacoes sao possiveis:\n"
          "(1) Listar todas as pessoas formadas em um determinado curso;\n"
          "(2) Listar as habilidades dos perfis que moram em uma determinada cidade;\n"
          "(3) Acrescentar uma nova experiência em um perfil;\n"
          "(4) Dado o email do perfil, retornar sua experiência;\n"
          "(5) Listar todas as informações de todos os perfis;\n"
          "(6) Dado o email de um perfil, retornar suas informações.\n"
          "(7) Sair\n\n"
          "Obs: Escrever apenas o numero correspondente e enviar com enter.\n\n"
          "Sua opcao: \0";
}

void my_strcat(char *msg, char *str, unsigned int *msg_size){

    unsigned int str_size;

    str_size = strlen(str);

    printf("msg_size: %d, strlen(msg): %lu, str_size: %d\n", *msg_size, strlen(msg), str_size);

    if (*msg_size < strlen(msg) + str_size){

        (*msg_size) += str_size;
        msg = realloc(msg, (*msg_size)*sizeof(char));
        printf("Nova mensagem: %s\n", msg);
    }

    strcat(msg, str);
}

void HandleClient (int socketfd, mongoc_collection_t *collection) {

    int num_bytes, state = DISCONNECTED;
    unsigned int msg_size = MAXDATASIZE;
    char *msg, buffer[MAXDATASIZE], *email, *str;
    mongoc_cursor_t *cursor;
    bson_t *query;
    bson_t *opts;
    const bson_t *doc;
    bool found;
    int count;

    msg = (char *)calloc(MAXDATASIZE, sizeof(char));
    email = (char *)calloc(MAXDATASIZE, sizeof(char));

    strcpy(msg, "\nBem-vind* ao RedesBook! Faca seu login para continuar:\n\nemail: \0");

    send_msg(socketfd, msg);

    while ((num_bytes = recv(socketfd, buffer, MAXDATASIZE-1, 0)) > 0) {

        buffer[num_bytes] = 0;
        printf("Estado: %d\n", state);
        printf("Recebi isso: |%s|\n", buffer);

        found = false;

        if (state == DISCONNECTED) {

            strcpy(email, buffer);
            strcpy(msg, "senha: \0");
            state = VERIFY_PASSWD;

            send_msg(socketfd, msg);
        }

        else if (state == VERIFY_PASSWD) {

            printf("Esse eh o email: |%s|\n", email);
            printf("Essa eh a senha: |%s|\n", buffer);
            query = bson_new();
            BSON_APPEND_UTF8 (query, "Email", email);
            BSON_APPEND_UTF8 (query, "senha", buffer);

            opts = BCON_NEW ("projection", "{", "_id", BCON_BOOL(false), "Nome Completo",  BCON_BOOL(true), "}");

            cursor = mongoc_collection_find_with_opts(collection, query, opts, NULL);

            while (mongoc_cursor_next(cursor, &doc)) {

                found = true;
                str = bson_as_canonical_extended_json (doc, NULL);
                printf("Combinacao de %s\n", str);

                bson_free(str);
            }

            bson_destroy (query);
            mongoc_cursor_destroy (cursor);

            if (found) {

                strcpy(msg, "\n\nBem-vind*!\0");
                my_strcat(msg, set_initial_msg(&state), &msg_size);
            }

            else {

                strcpy(msg, "email e/ou senha incorretos.\nemail: \0");
                state = DISCONNECTED;
            }

            send_msg(socketfd, msg);

        }

        else if (state == LOGGED){

            if (buffer[0] == '1') { 

                printf("Opcao escolhida: 1\n");
                strcpy(msg, "Escreva o curso que deseja saber: \0");
                state = OPT_1;
                send_msg(socketfd, msg);
            }
            else if (buffer[0] == '2') { 

                printf("Opcao escolhida: 2\n");
                strcpy(msg, "Escreva a cidade: \0");
                state = OPT_2;
                send_msg(socketfd, msg);
            }
            else if (buffer[0] == '3') { 

                printf("Opcao escolhida: 3\n");
                strcpy(msg, "Escreva o local: \0");
                state = OPT_3;
                send_msg(socketfd, msg);
            }
            else if (buffer[0] == '4') { 

                printf("Opcao escolhida: 4\n");
                strcpy(msg, "Escreva o email: \0");
                state = OPT_4;
                send_msg(socketfd, msg);
            }
            else if (buffer[0] == '5') { 

                printf("Opcao escolhida: 5\n");
                state = OPT_5;
            }
            else if (buffer[0] == '6') { 

                printf("Opcao escolhida: 6\n");
                strcpy(msg, "Escreva o email da pessoa: \0");
                state = OPT_6;
                send_msg(socketfd, msg);
            }
            else if (buffer[0] == '7') { 

                printf("Opcao escolhida: 7\n");
                strcpy(msg, "Ate mais! :)\n\0");
                state = OPT_7;
                send_msg(socketfd, msg);
            }
            else {

                printf("Opcao invalida!\n");
                strcpy(msg, "Opcao invalida!\n\0");
                my_strcat(msg, set_initial_msg(&state), &msg_size);
                send_msg(socketfd, msg);
            }

        }

        else if (state ==OPT_1) {

            query = bson_new();
            BSON_APPEND_UTF8 (query, "Formacao Academica", buffer);

            count = (int)mongoc_collection_count_documents(collection, query, NULL, NULL, NULL, NULL);

            printf("Contei %d pessoas\n", count);

            opts = BCON_NEW ("projection", "{", "_id", BCON_BOOL(false), "Nome Completo",  BCON_BOOL(true), "}");

            cursor = mongoc_collection_find_with_opts(collection, query, opts, NULL);

            msg[0] = 0;

            while (mongoc_cursor_next(cursor, &doc)) {

                found = true;
                str = bson_as_canonical_extended_json (doc, NULL);
                printf("Achei: %s\n", str);

                my_strcat(msg, str, &msg_size);

                bson_free(str);
            }

            bson_destroy (query);
            mongoc_cursor_destroy (cursor);

            if (!found)
                strcpy(msg, "Ninguem se formou nesse curso! :(\n\0");

            my_strcat(msg, set_initial_msg(&state), &msg_size);

            send_msg(socketfd, msg);

        }
        else if (state ==OPT_2) {

            query = bson_new();
            BSON_APPEND_UTF8 (query, "Residencia", buffer);

            count = (int)mongoc_collection_count_documents(collection, query, NULL, NULL, NULL, NULL);

            printf("Contei %d pessoas\n", count);

            opts = BCON_NEW ("projection", "{", "_id", BCON_BOOL(false), "Experiencia",  BCON_BOOL(true), "}");

            cursor = mongoc_collection_find_with_opts(collection, query, opts, NULL);

            msg[0] = 0;

            while (mongoc_cursor_next(cursor, &doc)) {

                found = true;
                str = bson_as_canonical_extended_json (doc, NULL);
                printf("Achei: %s\n", str);

                my_strcat(msg, str, &msg_size);

                bson_free(str);
            }

            bson_destroy (query);
            mongoc_cursor_destroy (cursor);

            if (!found)
                strcpy(msg, "Ninguem eh dessa cidade! :(\n\0");

            my_strcat(msg, set_initial_msg(&state), &msg_size);

            send_msg(socketfd, msg);

        }
        else if (state ==OPT_3) {

        }
        else if (state ==OPT_4) {

            query = bson_new();
            BSON_APPEND_UTF8 (query, "Email", buffer);

            count = (int)mongoc_collection_count_documents(collection, query, NULL, NULL, NULL, NULL);

            printf("Contei %d pessoas\n", count);

            opts = BCON_NEW ("projection", "{", "_id", BCON_BOOL(false), "Experiencia",  BCON_BOOL(true), "}");

            cursor = mongoc_collection_find_with_opts(collection, query, opts, NULL);

            msg[0] = 0;

            while (mongoc_cursor_next(cursor, &doc)) {

                found = true;
                str = bson_as_canonical_extended_json (doc, NULL);
                printf("Achei: %s\n", str);

                my_strcat(msg, str, &msg_size);

                bson_free(str);
            }

            bson_destroy (query);
            mongoc_cursor_destroy (cursor);

            if (!found)
                strcpy(msg, "Ninguem com esse email! :(\n\0");

            my_strcat(msg, set_initial_msg(&state), &msg_size);

            send_msg(socketfd, msg);

        }
        if (state ==OPT_5) {

            query = bson_new();
            count = (int)mongoc_collection_count_documents(collection, query, NULL, NULL, NULL, NULL);

            printf("Contei %d pessoas\n", count);

            opts = BCON_NEW ("projection", "{", "_id", BCON_BOOL(false), "senha",  BCON_BOOL(false), "}");

            cursor = mongoc_collection_find_with_opts(collection, query, opts, NULL);

            msg[0] = 0;

            while (mongoc_cursor_next(cursor, &doc)) {

                found = true;
                str = bson_as_canonical_extended_json (doc, NULL);

                my_strcat(msg, str, &msg_size);
                printf("msg (%d): %s\n", msg_size,msg);

                bson_free(str);
            }

            bson_destroy (query);
            mongoc_cursor_destroy (cursor);

            my_strcat(msg, set_initial_msg(&state), &msg_size);

            printf("msg (%d): %s\n", msg_size,msg);
            send_msg(socketfd, msg);

        }
        else if (state ==OPT_6) {

            query = bson_new();
            BSON_APPEND_UTF8 (query, "Email", buffer);

            count = (int)mongoc_collection_count_documents(collection, query, NULL, NULL, NULL, NULL);

            printf("Contei %d pessoas\n", count);

            opts = BCON_NEW ("projection", "{", "_id", BCON_BOOL(false), "senha",  BCON_BOOL(false), "}");

            cursor = mongoc_collection_find_with_opts(collection, query, opts, NULL);

            msg[0] = 0;

            while (mongoc_cursor_next(cursor, &doc)) {

                found = true;
                str = bson_as_canonical_extended_json (doc, NULL);
                printf("Achei: %s\n", str);

                my_strcat(msg, str, &msg_size);

                bson_free(str);
            }

            bson_destroy (query);
            mongoc_cursor_destroy (cursor);

            if (!found)
                strcpy(msg, "Ninguem com esse email! :(\n\0");

            my_strcat(msg, set_initial_msg(&state), &msg_size);

            send_msg(socketfd, msg);
        }
        else if (state == OPT_7)
            break;

    }

    close(socketfd);
    exit(0);

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

int main () {

    // Necessario para o banco de dados
    const char *uri_string = "mongodb://localhost:27017";
    mongoc_uri_t *uri;
    mongoc_client_t *client;
    mongoc_database_t *database;
    mongoc_collection_t *collection;
    bson_t *command, reply, *insert;
    bson_error_t error;
    char *str;
    bool retval;
    bson_oid_t oid;

    mongoc_cursor_t *cursor;
    bson_t *query;
    const bson_t *doc;

    // Esperar conexões em sockfd e conectar em new_fd
    int socketfd, new_fd;

    struct addrinfo hints, *server_info, *pointer;
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    int rv, count;
    int yes = 1;
    char ip_string[INET6_ADDRSTRLEN];
    struct sigaction signal_action;

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

        printf("Servidor: nova conexao de %s\n", ip_string);

        if (!fork()) {

            close(socketfd); // filho nao precisa escutar a socket

            HandleClient(new_fd, collection);

            close(new_fd);
            exit(0);
        }

        close(new_fd); // pai nao precisa da nova porta
    }

    close(socketfd);

    mongoc_collection_destroy (collection);
    mongoc_database_destroy (database);
    mongoc_uri_destroy (uri);
    mongoc_client_destroy (client);
    mongoc_cleanup ();

    return 0;
}
