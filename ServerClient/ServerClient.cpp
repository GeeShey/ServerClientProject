// ServerClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Client.h"
#include "Server.h"

#include <iostream>

int main()
{
    int choice;
    printf("Would you like to Create a Server or Client?\n");
    printf("1> Server\n");
    printf("2> Client\n");
    std::cin >> choice;

    if(choice==1)
    ServerSetup();
    if(choice==2)
    ClientSetup();



    //std::cout << "Hello World!\n";
}