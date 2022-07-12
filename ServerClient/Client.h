#pragma once
#include "Includes.h"
auto client_ip = INADDR_ANY;
int client_port = 31337;

SOCKET Client_ComSocket;
using namespace common;
int ClientRecieveMessage() {
	//Communication
	int size = 0;

	int result = tcp_recv_whole(Client_ComSocket, (char*)&size, 1);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		printf("DEBUG// recv is incorrect\n");
		return 0;

	}
	else
	{
		printf("DEBUG// I used the recv function\n");
		return 1;
	}

	char* buffer = new char[size];

	result = tcp_recv_whole(Client_ComSocket, (char*)buffer, size);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		printf("DEBUG// recv is incorrect\n");
		return 0;

	}
	else
	{
		printf("DEBUG// I used the recv function\n");
	}

	printf("DEBUG// I received a message from the client\n");

	printf("\n\n");
	printf(buffer);
	printf("\n\n");

	delete[] buffer;
}

int sendMessageFromClient(char* sendbuffer) {
	//Communication
	uint8_t size = 255;
	//memset(sendbuffer, 0, 255);

	int result = tcp_send_whole(Client_ComSocket, (char*)&size, 1);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		printf("DEBUG// send is incorrect\n");
		return 0;

	}
	else
	{
		printf("DEBUG// I used the send function\n");
	}



	result = tcp_send_whole(Client_ComSocket, sendbuffer, size);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		printf("DEBUG// send is incorrect\n");
		return 0;

	}
	else
	{
		printf("DEBUG// I used the send function\n");
	}
	return 1;
}

int clientShutdown() {
	shutdown(Client_ComSocket, SD_BOTH);
	closesocket(Client_ComSocket);
	return 1;
}

void ClientSetup()
{
	std::string choice;
	printf("Enter IP address\n");
	std::cin >> choice;

	client_ip = inet_addr(choice.c_str());
	printf("Enter port\n");
	std::cin >> client_port;

	WSADATA wsadata;
	WSAStartup(WINSOCK_VERSION, &wsadata);
	//Socket
	Client_ComSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Client_ComSocket == INVALID_SOCKET)
	{
		printf("DEBUG// Socket function incorrect\n");
		return;
	}
	else
	{
		printf("DEBUG// I used the socket function\n");
	}

	//Connect
	sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = client_ip;
	serverAddr.sin_port = htons(client_port);

	int result = connect(Client_ComSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR)
	{
		printf("DEBUG// Connect function incorrect\n");
		return;
	}
	else
	{
		printf("DEBUG// I used the Connect function\n");
	}

	//Communication
	char test[]  = "Test Message\0";
	sendMessageFromClient(test);
	std::cout << "Sent a test message\n";

	while (true) {
		std::string s;
		std::cin >> s;
		sendMessageFromClient(&s[0]);
		//ClientRecieveMessage();
	}
	// close sockets
	clientShutdown();

}
