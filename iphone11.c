#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "voice.h"

int main(int argc, char **argv){
    if(argc == 2){//サーバのとき
        int ss = socket(PF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr; /* 最終的にbind に渡すアドレス情報 */
        addr.sin_family = AF_INET; /* このアドレスはIPv4 アドレスです */
        addr.sin_port = htons(atoi(argv[1])); /* ポート...で待ち受けしたいです */
        addr.sin_addr.s_addr = INADDR_ANY; /* どのIP アドレスでも待ち受けしたいです */
        bind(ss, (struct sockaddr *)&addr, sizeof(addr));
        printf("Waiting for connections on port %s...\n", argv[1]);
        listen(ss, 10);
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(struct sockaddr_in);
        int s = accept(ss, (struct sockaddr *)&client_addr, &len);
        printf("connected!\n");

        handle_voice_call(s); // 参照：voice.h
    }
    else if(argc == 3){//クライアントのとき
        int s = socket(PF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET; /* これはIPv4 のアドレスです */
        addr.sin_addr.s_addr = inet_addr(argv[1]); /* IP アドレスは...です */
        addr.sin_port = htons(atoi(argv[2])); /* ポートは...です */
        int ret = connect(s, (struct sockaddr *)&addr, sizeof(addr)); /* 遂にconnect */
        if(ret == -1){
            printf("error: connection failed\n");
            close(s);
            return -1;
        }
        handle_voice_call(s); // 参照：voice.h
    }
    else{
        printf("usage: %s [IP] [port number] or %s [port number]\n",argv[0],argv[0]);
        return -1;
    }
}