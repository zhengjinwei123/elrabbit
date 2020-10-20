#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>

struct SockInfo {
    int fd;
    struct sockaddr_in addr;
    pthread_t id;
};


void* worker(void *arg) {
    char ip[64];
    char buffer[1024];

    SockInfo* info = (SockInfo*)arg;

    while (1) {
        printf("client Ip: %s, port: %d\n",
            inet_ntop(AF_INET, &info->addr.sin_addr.s_addr, ip, sizeof(ip)),
            ntohs(info->addr.sin_port));
        
        int len = read(info->fd, buffer, sizeof(buffer));
        if (len == -1) {
            perror("read error");
            pthread_exit(NULL);
        } else if (len == 0) {
            printf("client off \n");
            close(info->fd);
            info->fd = -1;
            break;
        } else {
            printf("recv buf: %s \n", buffer);
            write(info->fd, buffer, sizeof(buffer));
        }
    }

    return NULL;
}


int main(int argc, const char* argv[])
{
    if (argc < 2) {
        printf("eg: /a.out port \n");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    socklen_t serv_len = sizeof(serv_addr);
    int port = atoi(argv[1]);

    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0 , serv_len);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    bind(lfd, (struct sockaddr*)&serv_addr, serv_len);

    listen(lfd, 36);
    printf("start accept ...\n");


    SockInfo info[256];

    for (uint32_t i = 0 ; i < sizeof(info) / sizeof(info[0]); ++i) {
        info[i].fd = -1;
    }

    socklen_t cli_len = sizeof(struct sockaddr_in);

    while(1) {

        int j = -1;
        for (uint32_t i = 0 ; i < sizeof(info) / sizeof(info[0]); ++i) {
            if (info[i].fd == -1) {
                j = i;
                break;
            }
        }

        if (j == -1) {
            break;
        }

        info[j].fd = accept(lfd, (struct sockaddr*)&info[j].addr, &cli_len);

        // create child thread
        pthread_create(&info[j].id, NULL, worker, &info[j]);
        // 设置线程分离
        pthread_detach(info[j].id);
    }

    close(lfd);

    // 只退出主线程
    pthread_exit(NULL);
    return 0;
}
