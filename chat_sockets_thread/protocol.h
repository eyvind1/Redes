
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <string>
#include <sstream>

using namespace std;


string FirstNumberToString(int n)
{
     ostringstream ss;
     ss << n;
     return ss.str();
}


//The next two methods are a bad practice,
//but works it's necessary optimize this methods

string encode_simple_message(string message){
    char tmp = message[0];
    message.erase(0,1);
    int size_message = message.length();
    string size_str = FirstNumberToString(size_message);
    size_str = string(4-size_str.length(), '0').append(size_str);
    size_str += tmp;
    size_str += message;
    return size_str;
}

string encode_to_user_message(string message, string to_user, char custom_op='C'){
    int size_message = message.length();
    string size_str = FirstNumberToString(size_message);
    string size_str_user = FirstNumberToString(to_user.length());
    size_str = string(4-size_str.length(), '0').append(size_str);
    size_str += custom_op;
    size_str += string(2-size_str_user.length(), '0').append(size_str_user);
    size_str += to_user;
    size_str += message;
    return size_str;
}

#endif // PROTOCOL_H
