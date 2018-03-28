#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
            cout << "Message Received:  " << message_buffer << endl;
        }
        bzero(buffer,4);
        n = read(SocketFD,buffer,4);
    }while(true);
}

int main()
{
    struct sockaddr_in stSockAddr;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(-1 == SocketFD)
    {
        perror("can not create socket");
        exit(EXIT_FAILURE);
    }
    int n;
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
        perror("error bind failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if(-1 == listen(SocketFD, 10))
    {
        perror("error listen failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    string input;
    for(;;)
    {
        int ConnectFD = accept(SocketFD, NULL, NULL);

        if(0 > ConnectFD)
        {
            perror("error accept failed");
            close(SocketFD);
            exit(EXIT_FAILURE);
        }
        bool finished = false;
        std::thread(read_from_client, std::ref(ConnectFD), std::ref(finished)).detach();
        input = "Bienvenido";
        string to_send = encode_message(input);
        n = write(ConnectFD,to_send.c_str(),strlen(to_send.c_str()));
        while(true){
            std::getline(std::cin, input);
            to_send = encode_message(input);
            n = write(ConnectFD,to_send.c_str(),strlen(to_send.c_str()));
        }

        shutdown(ConnectFD, SHUT_RDWR);
        close(ConnectFD);
        cout << "Disconnected..." << endl;

    }
    return 0;
}
