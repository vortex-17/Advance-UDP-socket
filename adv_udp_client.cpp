
/* Advance UDP Client  */
 
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

int send_msg(int sockfd, struct sockaddr_in cliaddr){

    int n;
    struct msghdr msg = {};
    struct iovec iov[2];
    struct hdr h, h1;
    h.seq = 1;
    h.sa = cliaddr;
    struct sockaddr_in sa;
    char buf[100] = "This is the test message";

    char *hostname =  "<your_ip_address>";
    struct hostent *host;
    host = gethostbyname(hostname);
    if(host == NULL){
        perror("hostent");
        exit(1);
    }
    sa.sin_addr = *(struct in_addr*)host->h_addr;
    sa.sin_family = AF_INET;
    int port = 3456;
    sa.sin_port = htons(port);

    iov[0].iov_base = &h;
    iov[0].iov_len = sizeof(h);
    iov[1].iov_base = buf;
    iov[1].iov_len = sizeof(buf);

    msg.msg_name = &sa;
    msg.msg_namelen = sizeof(sa);
    msg.msg_iov = iov;
    msg.msg_iovlen = 2;
    msg.msg_control = 0;
    msg.msg_controllen = 0;
    msg.msg_flags = 0;

    // struct msghdr recv_msg = {};
    struct iovec recv_iov[1];
    recv_iov[0].iov_base = &h1;
    recv_iov[0].iov_len = sizeof(h1);
    msghdr recv_msg = {NULL,0,recv_iov,1,0,0,0};

    sendagain:
        n = sendmsg(sockfd,&msg,0);
        if(n == -1){
            perror("client - sendmsg");
            return 0;
        }
        cout << "Message has been sent" << endl;
        cout << "Waiting for ACK..." << endl;
        n = recvmsg(sockfd,&recv_msg,0); // *** This is blocking. Use fcntl to create non-blocking socket or other method
        if(n == -1){
            perror("client - recvmsg");
            return 0;
        }
        if(h.seq == h1.seq){
            cout << "ACK received";
            return 0;
        }
        else{
            cout << "Wrong ACK" << endl;
            goto sendagain;
        }


    return 0;

    
}

int create_client(){

    int sockfd;
    struct sockaddr_in cliaddr;
    int port = 3456;

    sockfd = socket(AF_INET,SOCK_DGRAM,0);

    memset(&cliaddr, 0, sizeof(cliaddr));

    cliaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    cout << cliaddr.sin_addr.s_addr << endl;
    cliaddr.sin_port = htons(port);
    cliaddr.sin_family = AF_INET;

    if(bind(sockfd,(struct sockaddr*)&cliaddr,sizeof(cliaddr)) == -1){
        perror("bind");
    }

    send_msg(sockfd,cliaddr);

    return 0;
}

int main(){

    int x = create_client();
    return 0;
}
