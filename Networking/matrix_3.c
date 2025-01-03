#include <limits.h>
#include <math.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include "matrix.h"

//Included in "question_client_udp.c" from class, but not in the matrix file:
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

matrix_t *matrix_download_udp(const char *host, const char *port, const char *matrix_name) {
    char *internet_id = "oneil853";
    int matrix_info[1024];
    unsigned rows;
    unsigned cols;

    struct addrinfo hints;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    struct addrinfo *server;

    int ret_val = getaddrinfo(host,port,&hints,&server);
    if (ret_val != 0) {
        printf("getaddrinfo failed: %s\n", gai_strerror(ret_val));
        return NULL;
    }

    int sock_fd = socket(server->ai_family,server->ai_socktype,server->ai_protocol);
    if (sock_fd == -1) {
        perror("socket");
        freeaddrinfo(server);
        return NULL;
    }


    if (sendto(sock_fd,internet_id,strlen(internet_id),0,server->ai_addr,server->ai_addrlen) == -1) {
        perror("sendto");
        close(sock_fd);
        freeaddrinfo(server);
        return NULL;
    }
    
    
    if (sendto(sock_fd,matrix_name,strlen(matrix_name),0,server->ai_addr,server->ai_addrlen) == -1) {
        perror("sendto");
        close(sock_fd);
        freeaddrinfo(server);
        return NULL;
    }
	
	
    if (recvfrom(sock_fd,matrix_info,sizeof(matrix_info),0,NULL,NULL) == -1) {
        perror("recvfrom");
        close(sock_fd);
        freeaddrinfo(server);
        return NULL;
    }
    
    if (ntohl(matrix_info[0]) == 1) {
        //Need to convert bytes to correct Endianness
        close(sock_fd);
        freeaddrinfo(server);
        return NULL;
    }

    //Need to convert bytes to correct Endianness
    rows = ntohl(matrix_info[1]);
    cols = ntohl(matrix_info[2]);
    matrix_t * udp_matrix = matrix_init(rows,cols);

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            udp_matrix->data[row][col] = ntohl(matrix_info[(row * cols)+(col)+3]);
            //ntohl, because we need to convert four incoming bytes to little_endian.
            //For each row that's passed, every column was counted.
            //Plus 3 ignores the following: success value, # of rows, and # of cols.
        }
    }

    freeaddrinfo(server);

    if (close(sock_fd) == -1) {
        perror("close");
        return NULL;
    }

    return udp_matrix;
}

matrix_t *matrix_download_tcp(const char *host, const char *port, const char *matrix_name) {

    char *internet_id = "oneil853";
    unsigned rows;
    unsigned cols;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo *server;
    

    int ret_val = getaddrinfo(host, port, &hints, &server);
    if (ret_val == -1) {
        printf("getaddrinfo failed: %s\n", gai_strerror(ret_val));
        return NULL;
    }

    int sock_fd = socket(server->ai_family,server->ai_socktype,server->ai_protocol);
    if (sock_fd == -1) {
        perror("socket");
        freeaddrinfo(server);
        return NULL;
    }

    if (connect(sock_fd,server->ai_addr,server->ai_addrlen) == -1) {
        perror("connect");
        close(sock_fd);
        freeaddrinfo(server);
        return NULL;
    }

    if (write(sock_fd,internet_id,strlen(internet_id)) == -1) {
        perror("write");
        close(sock_fd);
        freeaddrinfo(server);
        return NULL;
    }

    if (write(sock_fd,matrix_name,strlen(matrix_name)) == -1) {
        //Subtracting "sizeof(char)" to disinclude '\0' from request
        perror("write");
        close(sock_fd);
        freeaddrinfo(server);
        return NULL;
    }

    int success;
    if (read(sock_fd,&success,sizeof(success)) == -1) {
        perror("read");
        close(sock_fd);
        freeaddrinfo(server);
        return NULL;
    }

    if (ntohl(success) == 1) {
        //Need to convert bytes to correct Endianness
        close(sock_fd);
        freeaddrinfo(server);
        return NULL;
    }

    if (read(sock_fd,&rows,sizeof(rows)) == -1) {
        perror("read");
        close(sock_fd);
        freeaddrinfo(server);
        return NULL;
    }


    if (read(sock_fd,&cols,sizeof(cols)) == -1) {
        perror("read");
        close(sock_fd);
        freeaddrinfo(server);
        return NULL;
    }

    //Need to convert bytes to correct Endianness
    rows = ntohl(rows);
    cols = ntohl(cols);

    matrix_t *tcp_matrix = matrix_init(rows,cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int entry;
            if (read(sock_fd,&entry,sizeof(int)) == -1) {
                perror("read");
                close(sock_fd);
                freeaddrinfo(server);
                return NULL;
            }    
            //Need to convert bytes to correct Endianness
            entry = ntohl(entry);
            matrix_put(tcp_matrix, i, j, entry);

        }
    }
    
    freeaddrinfo(server);
    if (close(sock_fd) == -1) {
        perror("close");
        return NULL;
    }

    return tcp_matrix;
    
}

