#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include "../include/common.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

static ChatClient clients[MAX_CLIENTS];
static pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
static int server_fd = -1;

/* ── Broadcast to all except sender ── */
static void broadcast(const char *msg, int sender_fd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].active && clients[i].fd != sender_fd)
            send(clients[i].fd, msg, strlen(msg), MSG_NOSIGNAL);
    pthread_mutex_unlock(&clients_mutex);
}

/* ── Private message ── */
static int send_private(const char *to, const char *msg) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].username, to) == 0) {
            send(clients[i].fd, msg, strlen(msg), MSG_NOSIGNAL);
            pthread_mutex_unlock(&clients_mutex);
            return 1;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return 0;
}

/* ── List online users ── */
static void list_users(int fd) {
    char buf[512] = "Online: ";
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].active) {
            strcat(buf, clients[i].username);
            strcat(buf, " ");
        }
    pthread_mutex_unlock(&clients_mutex);
    strcat(buf, "\n");
    send(fd, buf, strlen(buf), MSG_NOSIGNAL);
}

/* ── Remove a client slot ── */
static void remove_client(int fd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].fd == fd) {
            clients[i].active = 0;
            clients[i].fd     = -1;
            memset(clients[i].username, 0, MAX_NAME_LEN);
            break;
        }
    pthread_mutex_unlock(&clients_mutex);
}

/* ── Per-client handler thread ── */
typedef struct { int fd; struct sockaddr_in addr; } ClientArg;

static void *handle_client(void *arg) {
    ClientArg *ca = (ClientArg *)arg;
    int fd = ca->fd;
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ca->addr.sin_addr, ip, sizeof(ip));
    free(ca);

    /* Login */
    char username[MAX_NAME_LEN] = {0};
    send(fd, "Username: ", 10, 0);
    int n = recv(fd, username, MAX_NAME_LEN - 1, 0);
    if (n <= 0) { close(fd); return NULL; }
    username[strcspn(username, "\r\n")] = '\0';

    /* Register */
    pthread_mutex_lock(&clients_mutex);
    int slot = -1;
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (!clients[i].active) { slot = i; break; }
    if (slot == -1) {
        pthread_mutex_unlock(&clients_mutex);
        send(fd, "Server full.\n", 13, 0);
        close(fd); return NULL;
    }
    clients[slot].fd     = fd;
    clients[slot].active = 1;
    strncpy(clients[slot].username, username, MAX_NAME_LEN - 1);
    pthread_mutex_unlock(&clients_mutex);

    printf("  [Server] %s (%s) connected\n", username, ip);
    char welcome[MAX_MSG_LEN];
    snprintf(welcome, sizeof(welcome),
             "Welcome %s! Commands: /list /msg <user> <text> /quit\n", username);
    send(fd, welcome, strlen(welcome), 0);

    char join_msg[MAX_MSG_LEN];
    snprintf(join_msg, sizeof(join_msg), "*** %s joined ***\n", username);
    broadcast(join_msg, fd);

    /* Message loop */
    char buf[MAX_MSG_LEN];
    while (1) {
        memset(buf, 0, sizeof(buf));
        n = recv(fd, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;
        buf[strcspn(buf, "\r\n")] = '\0';
        if (!*buf) continue;

        if (strcmp(buf, "/quit") == 0) break;

        if (strcmp(buf, "/list") == 0) {
            list_users(fd);
            continue;
        }

        if (strncmp(buf, "/msg ", 5) == 0) {
            /* /msg <user> <message> */
            char *rest = buf + 5;
            char *sp   = strchr(rest, ' ');
            if (sp) {
                *sp = '\0';
                char pm[MAX_MSG_LEN];
                snprintf(pm, sizeof(pm), "[PM from %s]: %s\n", username, sp + 1);
                if (!send_private(rest, pm))
                    send(fd, "User not found.\n", 16, 0);
            }
            continue;
        }

        /* Broadcast */
        char out[MAX_MSG_LEN];
        snprintf(out, sizeof(out), "[%s]: %s\n", username, buf);
        printf("  %s", out);
        broadcast(out, fd);
    }

    char left[MAX_MSG_LEN];
    snprintf(left, sizeof(left), "*** %s left ***\n", username);
    broadcast(left, -1);
    printf("  [Server] %s disconnected\n", username);
    remove_client(fd);
    close(fd);
    return NULL;
}

/* ── Start server ── */
static void run_chat_server(void) {
    print_header("Multithreaded Chat Server");
    printf("Starting on port %d ... (Ctrl-C to stop)\n\n", CHAT_PORT);

    signal(SIGPIPE, SIG_IGN);

    memset(clients, 0, sizeof(clients));
    for (int i = 0; i < MAX_CLIENTS; i++) clients[i].fd = -1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return; }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family      = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port        = htons(CHAT_PORT)
    };
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(server_fd); return;
    }
    listen(server_fd, 5);
    printf("  Listening on 0.0.0.0:%d\n", CHAT_PORT);
    printf("  Test with:  telnet 127.0.0.1 %d\n\n", CHAT_PORT);

    while (1) {
        struct sockaddr_in cli_addr;
        socklen_t len = sizeof(cli_addr);
        int cli_fd = accept(server_fd, (struct sockaddr *)&cli_addr, &len);
        if (cli_fd < 0) {
            if (errno == EINTR) break;
            perror("accept"); continue;
        }
        ClientArg *ca = malloc(sizeof(ClientArg));
        ca->fd   = cli_fd;
        ca->addr = cli_addr;
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, ca);
        pthread_detach(tid);
    }
    close(server_fd);
}

#endif /* CHAT_SERVER_H */
