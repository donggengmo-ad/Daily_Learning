#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "auction.h"

#pragma comment(lib, "ws2_32.lib")

SOCKET client_socket = INVALID_SOCKET;
int socket_enabled = 0;
char receive_buffer[8192];

int initSocket(const char* server_ip, int server_port) {
    WSADATA wsa_data;
    int result;

    result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return -1;
    }

    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET) {
        printf("socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    struct linger so_linger;
    so_linger.l_onoff = 1;
    so_linger.l_linger = 5;
    setsockopt(client_socket, SOL_SOCKET, SO_LINGER, (char*)&so_linger, sizeof(so_linger));

    int keep_alive = 1;
    setsockopt(client_socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&keep_alive, sizeof(keep_alive));

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    printf("Connecting to %s:%d...\n", server_ip, server_port);
    result = connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (result == SOCKET_ERROR) {
        printf("connect failed: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return -1;
    }

    socket_enabled = 1;
    printf("Connected to server %s:%d\n", server_ip, server_port);
    return 0;
}

int sendData(const char* data) {
    if (!socket_enabled || client_socket == INVALID_SOCKET) {
        return -1;
    }

    char message[8192];
    sprintf(message, "%s\n", data);
    int len = strlen(message);
    int total_sent = 0;

    while (total_sent < len) {
        int sent = send(client_socket, message + total_sent, len - total_sent, 0);
        if (sent == SOCKET_ERROR) {
            int err = WSAGetLastError();
            printf("send failed: %d\n", err);
            socket_enabled = 0;
            return -1;
        }
        total_sent += sent;
    }
    return total_sent;
}

char* receiveData() {
    if (!socket_enabled || client_socket == INVALID_SOCKET) {
        return NULL;
    }

    memset(receive_buffer, 0, sizeof(receive_buffer));

    u_long mode = 1;
    ioctlsocket(client_socket, FIONBIO, &mode);

    int received = recv(client_socket, receive_buffer, sizeof(receive_buffer) - 1, 0);

    mode = 0;
    ioctlsocket(client_socket, FIONBIO, &mode);

    if (received == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) {
            printf("recv failed: %d\n", err);
            socket_enabled = 0;
        }
        return NULL;
    }
    if (received == 0) {
        printf("Server disconnected\n");
        socket_enabled = 0;
        return NULL;
    }

    receive_buffer[received] = '\0';

    char* newline = strchr(receive_buffer, '\n');
    if (newline != NULL) {
        *newline = '\0';
    }

    return receive_buffer;
}

void closeSocket() {
    printf("Closing socket...\n");
    if (client_socket != INVALID_SOCKET) {
        shutdown(client_socket, SD_BOTH);
        closesocket(client_socket);
        client_socket = INVALID_SOCKET;
    }
    WSACleanup();
    socket_enabled = 0;
    printf("Socket closed\n");
}

void sendGameState() {
    if (!socket_enabled) return;

    char json[8192];
    memset(json, 0, sizeof(json));

    int offset = 0;
    offset += sprintf(json + offset, "{");
    offset += sprintf(json + offset, "\"round_no\":%d,", rounds.no);
    offset += sprintf(json + offset, "\"state\":%d,", rounds.state);
    offset += sprintf(json + offset, "\"salary\":%d,", rounds.salary);
    offset += sprintf(json + offset, "\"auc_no\":%d,", rounds.auc_no);
    offset += sprintf(json + offset, "\"winner_no\":%d,", rounds.winner_no);
    offset += sprintf(json + offset, "\"expense\":%d,", rounds.expense);
    offset += sprintf(json + offset, "\"card_remain\":%d,", rounds.card_remain);
    offset += sprintf(json + offset, "\"players\":[");

    for (int i = 0; i < player_num; i++) {
        offset += sprintf(json + offset, "{");
        offset += sprintf(json + offset, "\"name\":\"%s\",", players[i].name);
        offset += sprintf(json + offset, "\"score\":%d,", players[i].score);
        offset += sprintf(json + offset, "\"block\":%d,", players[i].block);
        offset += sprintf(json + offset, "\"identity\":%d,", players[i].identity);
        offset += sprintf(json + offset, "\"ex_auc\":%d,", players[i].ex_auc);
        offset += sprintf(json + offset, "\"dice_roll\":%d,", players[i].dice_roll);
        offset += sprintf(json + offset, "\"salary\":%d,", players[i].salary);
        offset += sprintf(json + offset, "\"AI\":%d", players[i].AI);
        offset += sprintf(json + offset, "}");
        if (i < player_num - 1) {
            offset += sprintf(json + offset, ",");
        }
    }

    offset += sprintf(json + offset, "]}");

    if (sendData(json) > 0) {
    }
}

void receiveGameState() {
    if (!socket_enabled) return;

    char* data = receiveData();
    if (data != NULL && strlen(data) > 0) {
    }
}
