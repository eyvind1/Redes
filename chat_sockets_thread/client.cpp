#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <thread>

#include "protocol.h"

using namespace std;


void read_from_client(int &SocketFD, bool &finished){
    finished = false;
    char *message_buffer;
    char buffer[4];
    bzero(buffer,4);
    int n = read(SocketFD,buffer,4);
    do{
        if (n>0){
            int size_message = atoi(buffer);
            message_buffer = new char[size_message];
            n = read(SocketFD, message_buffer, size_message);
            cout << "Size Message: " << size_message << endl;
            cout << "Server Message Received:  " << message_buffer << endl;
        }
        bzero(buffer,4);
        n = read(SocketFD,buffer,4);
    }while(true);
}

int main(int argc, char *argv[])
{
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP );
    int n;

    if (-1 == SocketFD)
    {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);

    if (0 > Res)
    {
        perror("error: first parameter is not a valid address family");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    else if (0 == Res)
    {
        perror("char string (second parameter does not contain valid ipaddress");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    bool finished;
    std::thread(read_from_client, std::ref(SocketFD), std::ref(finished)).detach();
    cout << "Connecting..." << endl;
    string input_message = "Hi, I'm a Client";

    string to_send = encode_message(input_message);

    n = write(SocketFD,to_send.c_str(),strlen(to_send.c_str()));
    while (true) {
        std::getline(std::cin, input_message);
        to_send = encode_message(input_message);
        n = write(SocketFD,to_send.c_str(),strlen(to_send.c_str()));
        //printf("Sending:<%s>\n",input_message.c_str());
    }


    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);

    return 0;
}
