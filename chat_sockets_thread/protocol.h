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


string NumberToString(int n)
{
     ostringstream ss;
     ss << n;
     return ss.str();
}

string encode_message(string message){
    int size_message = message.length();
    string size_str = NumberToString(size_message);
    size_str = string(4-size_str.length(), '0').append(size_str);
    size_str += message;
    return size_str;
}

#endif // PROTOCOL_H
