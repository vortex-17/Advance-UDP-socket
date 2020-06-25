
/* Advance UDP Server  */
 
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <cpuid.h>
#include <syslog.h>
#include <sys/uio.h>
#include <sys/event.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <pthread.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/if_dl.h>
#include <net/route.h>

using namespace std;

struct hdr{
    int seq;
    struct sockaddr_in sa;
};

int retransmit(struct sockaddr_in sa,int sockfd, struct hdr h){

    int n;
    struct msghdr msg = {};
    struct iovec iov[1];
    memset(&h.sa,0, sizeof(h.sa));

    iov[0].iov_base = &h;
    iov[0].iov_len = sizeof(struct hdr);

    msg.msg_name = &sa;
    msg.msg_namelen = sizeof(sa);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_control = 0;
    msg.msg_controllen = 0;
    msg.msg_flags = 0;

    n = sendmsg(sockfd,&msg,0);
    if(n == -1){
        perror("server - sendmsg");
        return 0;
    }

    cout << "Message Retransmitted" << endl;

    return 0;
}

int recv_msg(int sockfd){

    int n;
    // struct msghdr msg = {};
    struct iovec iov[2];
    char recv_buf[100];
    struct hdr h;

    iov[0].iov_base = &h;
    iov[0].iov_len = sizeof(h);
    iov[1].iov_base = recv_buf;
    iov[1].iov_len = sizeof(recv_buf);

    struct msghdr msg = {NULL,0,iov,2,0,0,0};

    cout << "Listening for messages...." << endl;

    n = recvmsg(sockfd,&msg,0);
    if(n == -1){
        perror("server - recvmsg");
        return(1);
    }

    cout << "Message : " << recv_buf << endl;

    cout << "Retransmitting the seq no" << endl;
    retransmit(h.sa,sockfd,h);


}

int create_server(){
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    int n;
    int port = 3456;

    memset(&servaddr,0,sizeof(servaddr));

    sockfd = socket(AF_INET,SOCK_DGRAM,0);

    servaddr.sin_addr.s_addr =inet_addr("<your_ip_address>");
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);


    if(bind(sockfd,(struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        perror("bind");
    }

    recv_msg(sockfd);


    return 0;
}

int main(){

    int x = create_server();
    return 0;
}