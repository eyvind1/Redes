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


void read_from_client(int SocketFD){
    //Modify this it's necessary only read 4 bytes
    //we use 256 to allocate all type of messages
    //this function shows all the coded message
    char *message_buffer;
    char buffer[256];
    bzero(buffer,256);
    int n = read(SocketFD,buffer,256);
    do{
        if (n>0){
//            int size_message = atoi(buffer);
//            n = read(SocketFD, message_buffer, size_message);
//            cout << "Size Message: " << size_message << endl;
            cout << "Server Message Received:  " << buffer << endl;
        }
        bzero(buffer,256);
        n = read(SocketFD,buffer,256);
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
    std::thread(read_from_client, SocketFD).detach();
    cout << "Connecting..." << endl;
    string input_message;
    string to_send;
    //Please never use cin alone, use cin with get line
    while(true){
        std::getline(std::cin, input_message);
        //We have 3 cases to send
        //Sending P we will receive the users list
        if(input_message == "P"){
            to_send = "";
            to_send = encode_simple_message(string("P"));
        }
        //Writing L, we can login in the chat server
        else if(input_message == "L"){
            to_send = "";
            cout << "Please enter your nickname: ";
            std::getline(std::cin, to_send);
            to_send = encode_simple_message(string("L")+to_send);
        }
        //Writing C we can send a message to other user
        else if(input_message == "C"){
            string to_user;
            to_send = "";
            cout << "Enter the username to send: ";
            std::getline(std::cin, to_user);
            cout << "Enter the message: ";
            std::getline(std::cin, to_send);
            to_send = encode_to_user_message(to_send, to_user);
        }
        else{
            cout << "Command not recognized :(" << endl;
            continue;
        }
        n = write(SocketFD, to_send.c_str(), to_send.length());
        input_message = "";
    }

    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);

    return 0;
}
