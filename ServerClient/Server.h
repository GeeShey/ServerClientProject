#pragma once
#include "Includes.h"
auto server_ip = INADDR_ANY;
int server_port = 31337;

fd_set master,readySet;
bool serverActive = true;

SOCKET listenSocket;
//SOCKET Server_ComSocket;



int ServerRecieveMessage(SOCKET Server_ComSocket) {
	using namespace common;

	//Communication
	int size = 0;

	int result = tcp_recv_whole(Server_ComSocket, (char*)&size, 1);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		printf("DEBUG// recv is incorrect\n");
		return 0;

	}
	else
	{
		printf("DEBUG// I used the recv function\n");
		//return 1;
	}

	char* buffer = new char[size];

	result = tcp_recv_whole(Server_ComSocket, (char*)buffer, size);
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

int sendMessageFromServer(char* sendbuffer, SOCKET Server_ComSocket) {
	//Communication
	uint8_t size = 255;
	//memset(sendbuffer, 0, 255);

	int result = tcp_send_whole(Server_ComSocket, (char*)&size, 1);
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



	result = tcp_send_whole(Server_ComSocket, sendbuffer, size);
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

int Update() {

	readySet = master;
	int readyFD = select(NULL, &readySet, NULL, NULL, NULL);

	//printf("DEBUG// I used the accept function\n");

	for (int i = 0; i < readyFD; i++) {
		if (readySet.fd_array[i] == listenSocket) {
			//if listen socket
			if (FD_ISSET(listenSocket, &readySet)) {
				SOCKET Server_ComSocket = accept(listenSocket, NULL, NULL);
				if (Server_ComSocket != INVALID_SOCKET) {
					FD_SET(Server_ComSocket, &master);
					printf("Connected to a client\n");

				}
				else {
					printf("DEBUG// Accept function incorrect\n");
					return 0;
				}
			}
		}
		else {
			int x = ServerRecieveMessage(readySet.fd_array[i]);
			if (x == 0) {
				return 0;
			}
			/*char test[] = "Test Echo\0";
			if (sendMessageFromServer(test) == 0) {
				return 0;
			}*/

		}

	}
	

}

int ServerSetup()
{//Socket

	std::string choice;
	printf("Enter IP address\n");
	std::cin >> choice;

	server_ip = inet_addr(choice.c_str());
	printf("Enter port\n");
	std::cin >> server_port;


	WSADATA wsadata;
	WSAStartup(WINSOCK_VERSION, &wsadata);
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("DEBUG// Socket function incorrect\n");
		return 0;
	}
	else
	{
		printf("DEBUG// I used the socket function\n");
	}

	//Bind
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = server_ip;
	serverAddr.sin_port = htons(server_port);

	int result = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR)
	{
		printf("DEBUG// Bind function incorrect\n");
		return 0;
	}
	else
	{
		printf("DEBUG// I used the bind function\n");
	}

	//Listen
	result = listen(listenSocket, 1);
	if (result == SOCKET_ERROR)
	{
		printf("DEBUG// Listen function incorrect\n");
		return 0;
	}
	else
	{
		printf("DEBUG// I used the listen function\n");
	}

	FD_ZERO(&master);
	FD_SET(listenSocket, &master);
	

	while (serverActive) {
		Update();
	}

	printf("Waiting...\n\n");


	


	return 1;
}



int serverShutdown() {

	// close both sockets
	shutdown(listenSocket, SD_BOTH);
	closesocket(listenSocket);

	//shutdown(Server_ComSocket, SD_BOTH);
	//closesocket(Server_ComSocket);
	return 1;
}