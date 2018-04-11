#ifndef PROTOCOL_SERVER_H
#define PROTOCOL_SERVER_H

#include <iostream>
#include <map>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>


using namespace std;


string NumberToString(int n)
{
     ostringstream ss;
     ss << n;
     return ss.str();
}


class Messsage
{
public:
    int socket_dest;
    string message;
    Messsage(int socket_dest, string message) {
        this->socket_dest = socket_dest;
        this->message = message;
    }
    Messsage(){}
    Messsage& operator =(Messsage msg){
        this->socket_dest = msg.socket_dest;
        this->message = msg.message;
        return *this;
    }
    void send_message(){
        //cout << "To: " << socket_dest << "  " << message << endl;
        int n;
        n = write(socket_dest, message.c_str(), message.length());
    }
};


class Protocol
{
public:
    map<string,int> clients;
    Protocol(){}
    Messsage read_s(char operation, int size_message, int source_socket){
        int n;
        char *message_buffer;

        if(size_message == 0){
            if(operation == 'P')
                return Messsage(source_socket, prepare_simple_response(get_clients()));

            if(operation == 'E'){
                
                string message = this->get_name_client(source_socket)+" left the chat";
                this->remove_client(source_socket);
                return Messsage(source_socket, prepare_simple_response(message));
            }
        }
        
        if(operation == 'L'){
            message_buffer = new char[size_message];
            n = read(source_socket, message_buffer, size_message);
            //Validate that n and data in future
            this->add_client(string(message_buffer), source_socket);
            return Messsage(source_socket, prepare_simple_response("You are logged now"));
        }
        else if(operation == 'C'){
            //Read Two First Bytes
            message_buffer = new char[2];
            n = read(source_socket, message_buffer, 2);
            int dest_name_size = atoi(message_buffer);
            //With that read for who is the message
            message_buffer = new char[dest_name_size];
            n = read(source_socket, message_buffer, dest_name_size);
            string dest_name = string(message_buffer);
            int dest_socket = this->clients[dest_name]; //get te socket by the name
            string name = get_name_client(dest_socket);
            if((this->clients.find(name)->second) == 0)
            {
                return Messsage(source_socket, prepare_simple_response("Message not sent"));
            }
            else{
                message_buffer = new char[size_message];
            
                //now read the message
                n = read(source_socket, message_buffer, size_message);
                string message = this->get_name_client(source_socket) + " says: " + string(message_buffer);
                return Messsage(dest_socket, prepare_simple_response(message));
            }
            
             
        }
    }

    string get_clients(){
        string ret = "\n";
        for (std::map<string,int>::iterator it=this->clients.begin(); it!=this->clients.end(); ++it)
            ret += it->first + "\n";
        return ret;
    }

    string get_name_client(int socket){
        for (std::map<string,int>::iterator it=this->clients.begin(); it!=this->clients.end(); ++it)
            if(it->second == socket) return it->first;
        return "-";
    }

    string prepare_simple_response(string message){
        int size_message = message.length();
        string size_str = NumberToString(size_message);
        size_str = string(4-size_str.length(), '0').append(size_str);
        size_str += 'R';
        size_str += message;
        return size_str;
    }

    void add_client(string name, int socket){
        this->clients[name] = socket;
    }

    void remove_client(int socket){
        string name;
        name= get_name_client(socket);
        this->clients.erase(name);
    }

    bool find_client(string name)
    {
        for (std::map<string,int>::iterator it=this->clients.begin(); it!=this->clients.end(); ++it){
            if(it->first == name){
                 cout<<"find"<<endl;//return true;
             }
        
            else
                cout<<"false"<<endl;//return false;
        }
    }
};



#endif // PROTOCOL_SERVER_H
