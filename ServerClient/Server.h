#pragma once
#include "Includes.h"
#include <fstream>
auto server_ip = INADDR_ANY;
int server_port = 31337;

fd_set master,readySet;
bool serverActive = true;
int CHAT_CAPACITY = 2;
SOCKET listenSocket;
int readyFD = 0;
std::ofstream ofs;
std::string LOG_FILENAME = "log.txt";
//SOCKET Server_ComSocket;
struct user {

	std::string name;
	SOCKET sock;
};

enum messages {
	SV_FULL, SV_SUCCESS
};

std::vector<user> users;

void Server_log(std::string s) {
	ofs.open(LOG_FILENAME, std::ios_base::app);
	ofs << s;
	ofs.close();
}

int ServerSendMessage(char* sendbuffer, SOCKET Socket,bool isServerMessage = true) {
	//Communication
	uint8_t size = 255;
	//memset(sendbuffer, 0, 255);

	int result = tcp_send_whole(Socket, (char*)&size, 1);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		printf("DEBUG// send is incorrect\n");
		return 0;

	}
	else
	{
		//printf("DEBUG// I used the send function\n");
	}



	result = tcp_send_whole(Socket, sendbuffer, size);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		printf("DEBUG// send is incorrect\n");
		return 0;

	}
	else
	{
		//printf("DEBUG// I used the send function\n");
	}
	return 1;
}

int executeCommand(char* msg,SOCKET s) {
	// 0 SV_FULL
	// 1 SV_SUCCESS
	if (msg[0] == '$') {
		printf("processing the following command...\n");
		printf(msg);
		printf("\n");
		if (users.size() >= CHAT_CAPACITY) {
			printf("CHATROOM IS FULL, CANNOT ADD MORE USERS\n");
			//SV_FULL
			char invalid[] = "0";
			ServerSendMessage(invalid, s);
			return 0;
		}
		else {
			user newUser;
			std::string tempString(msg);
			newUser.name = tempString.substr(10, tempString.length() - 9);
			newUser.sock = s;
			users.push_back(newUser);
			printf("added a new user: ");
			printf(newUser.name.c_str());
			printf("\n");

			char valid[] = "1";
			ServerSendMessage(valid, s);
			return 1;
		}
	}
}

std::string getUsernameFromSocket(SOCKET socket) {
	for (int i = 0; i < users.size(); i++) {
		if ((users[i].sock) == socket) {
			return users[i].name;
		}
	}
	return "NEW CLIENT";
}
int ServerRecieveMessage(SOCKET Socket) {
	using namespace common;

	//Communication
	int size = 0;
	int result = tcp_recv_whole(Socket, (char*)&size, 1);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		printf("DEBUG// recv is incorrect\n");
		return 0;

	}
	else
	{
		//printf("DEBUG// I used the recv function\n");
		//return 1;
	}

	char* buffer = new char[size];

	result = tcp_recv_whole(Socket, (char*)buffer, size);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		printf("DEBUG// recv is incorrect\n");
		return 0;

	}
	else
	{
		//printf("DEBUG// I used the recv function\n");
		if (buffer[0] == '$') {
			executeCommand(buffer,Socket);
			Server_log("COMMAND: ");
			Server_log(std::string(buffer));
			Server_log("\n");


		}
		else if (buffer[0] == '\0') {
			//recieved a blank message
		}
		else {
			if (users.size() != 0) {
				std::string nama = getUsernameFromSocket(Socket);
				printf(nama.c_str());
				printf(": ");
				printf(buffer);
				printf("\n");

				nama.append(": ");
				Server_log(nama);
				Server_log(std::string(buffer));
				Server_log("\n");

			}
			else {
				printf("CLIENT");
				printf(": ");
				printf(buffer);
				printf("\n");

				Server_log("CLIENT: ");
				Server_log(std::string(buffer));
				Server_log("\n");

			}

		}
	}
	//printf("\nDEBUG// I received a message from the client\n");

	delete[] buffer;
}



int Update() {

	readySet = master;
	timeval timeout{};
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	readyFD = select(NULL, &readySet, NULL, NULL, &timeout);


	for (int i = 0; i < readyFD; i++) {
		if (readySet.fd_array[i] == listenSocket) {
			//if listen socket
			if (FD_ISSET(listenSocket, &readySet)) {
				SOCKET Server_ComSocket = accept(listenSocket, NULL, NULL);
				if (Server_ComSocket != INVALID_SOCKET) {
					FD_SET(Server_ComSocket, &master);
					Server_log("Connected to a new client");
					Server_log("\n");

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
			else {
				//echo
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
	//clearing the file
	ofs.open(LOG_FILENAME, std::ofstream::out | std::ofstream::trunc);
	ofs.close();

	//std::string choice;
	//printf("Enter IP address\n");
	//std::cin >> choice;
	//
	//server_ip = inet_addr(choice.c_str());
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
		//printf("DEBUG// I used the socket function\n");
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
		//printf("DEBUG// I used the bind function\n");
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
		//printf("DEBUG// I used the listen function\n");
	}

	FD_ZERO(&master);
	FD_SET(listenSocket, &master);
	
	printf("Great Success | Waiting...\n\n");
	while (serverActive) {
		Update();
	}



	


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