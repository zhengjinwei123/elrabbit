#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        printf("eg: a.out port \n");
        exit(1);
    }

    int port = atoi(argv[1]);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr.s_addr);
    connect(fd, (struct sockaddr*)&serv, sizeof(serv));

    while(1) {
        char buf[1024];

        printf("please input \n");
        fgets(buf, sizeof(buf), stdin);

        write(fd, buf, strlen(buf));

        int len  = read(fd, buf, sizeof(buf));

        if (len == -1) {
            perror("read error");
            exit(1);
        } else if (len == 0) {
            printf("server closed \n");
            break;
        } else {
            printf("recv %s \n", buf);
        }
    }

    close(fd);
    return 0;
}