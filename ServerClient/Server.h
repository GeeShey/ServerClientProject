#pragma once
#include "Includes.h"
#include <functional>
#include <string>
auto server_ip = INADDR_ANY;
int server_port = 31337;

fd_set master,readySet;
bool serverActive = true;
int CHAT_CAPACITY = 2;
SOCKET listenSocket;
int readyFD = 0;
std::string LOG_FILENAME = "log.txt";
std::ofstream ofs;

char invalid[] = "0";//SV_FAIl

//SOCKET Server_ComSocket;
struct user {

	std::string name;
	SOCKET sock;
	bool operator==(user& that)
	{
		if (this == &that)
		{
			return true;
		}
		else {
			if (this->name == that.name && this->sock == that.sock)
				return true;
			else
				return false;
		}
	}
};

std::vector<user> users;

void Server_log(std::string s) {
	ofs.open(LOG_FILENAME, std::ios_base::app);
	ofs << s;
	ofs.close();
}

std::string getUsernameFromSocket(SOCKET socket) {
	for (int i = 0; i < users.size(); i++) {
		if ((users[i].sock) == socket) {
			return users[i].name;
		}
	}
	return "UNREGISTERED CLIENT";
}

int ServerSendBigMessage(std::string msg, SOCKET Socket, bool isEcho = false) {

	//msg.append("\0");

	std::string length = std::to_string(msg.length()+1);

	send(Socket, &length[0], length.size(), 0);//sending the size of the buffer

	//printf("sent length ");		//DEBUG STUFF
	//printf(length.c_str());		//DEBUG STUFF
	//printf("\n");				//DEBUG STUFF

	for (int i = 0; i < msg.size()+1; i++) {
		send(Socket, &msg[i], 1, 0);//sending the size of the buffer
	}

	return 1;


}
bool socketExists(SOCKET socket) {

	for (int i = 0; i < users.size(); i++) {
		if ((users[i].sock) == socket) {
			return true;
		}
	}
	return false;
}

int ServerSendMessage(char* sendbuffer, SOCKET Socket, bool isEcho = false) {
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
	std::string nama = getUsernameFromSocket(Socket);
	if (isEcho) {
		Server_log("Server -> ");
		Server_log(nama.c_str());
		Server_log(": ECHO\n");

	}
	else {

	
		Server_log("Server -> ");
		Server_log(nama.c_str());
		Server_log(": ");
		Server_log(sendbuffer);
		Server_log("\n");

	}


	return 1;
}
int getListCommand(char* msg, SOCKET s) {

	std::string message = "-------printing online members-------\n";
	for (int i = 0; i < users.size(); i++) {
		message.append(users[i].name);
		message.append("\n");
	} //todo come back here
	message.append( "-------------------------------\n");

	ServerSendBigMessage(message, s);
	return 1;
}

int getLogCommand(char* msg, SOCKET s) {

	std::ifstream ifs(LOG_FILENAME);
	std::string line = "-------printing server logs-------\n";
	if (ifs.is_open())
	{
		std::string temp = "";
		while (std::getline(ifs, temp))
		{
			line.append(temp);
			line.append("\n");

		}
		ifs.close();
		ifs.clear();
	}
	line.append("-------------------------------\n");
	ServerSendBigMessage(line, s);
	return 1;
}
int registerCommand(char* msg, SOCKET s) {

	printf("processing the following register command...\n");
	printf(msg);
	printf("\n");
	if (users.size() >= CHAT_CAPACITY) {
		printf("CHATROOM IS FULL, CANNOT ADD MORE USERS\n");
		//SV_FULL
		ServerSendMessage(invalid, s);
		//closesocket(s);
		FD_CLR(s, &master);
		return 0;
	}
	else {
		if (socketExists(s)) {
			printf("Existing user tried registering\n");
			ServerSendMessage((char*)SV_ALREADYREGISTERED, s);
			return 1;
		}
		else
		{
			user newUser;
			std::string tempString(msg);
			newUser.name = tempString.substr(10, tempString.length() - 9);
			newUser.sock = s;
			users.push_back(newUser);
			printf("added a new user: ");
			printf(newUser.name.c_str());
			printf("\n");

			ServerSendMessage((char *)SV_SUCCESS, s);
			return 1;
		}
	}

}

int exitWithoutRegister(SOCKET s) {
	FD_CLR(s, &master);
	std::string message = "2";
	ServerSendMessage(&message[0], s);

	return 1;
}

int exitCommand(char* msg, SOCKET s) {

	FD_CLR(s, &master);
	int currIndex = 0;
	if (socketExists(s)) {
		if (users.size() != 0) {
			for (int i = 0; i < users.size(); i++) {
				if (users[i].sock == s) {
					currIndex = i;
					break;
				}
			}

			printf("User ");
			printf(users[currIndex].name.c_str());
			printf(" disconnected [It was nice while it lasted :( ]\n");

			Server_log("Server disconnected ");
			Server_log(users[currIndex].name.c_str());
			Server_log(" with GRACE\n");

			users.erase(users.begin() + currIndex);


		}
		std::string message = "2";
		ServerSendMessage(&message[0], s);
		return 1;
	}
	else {
		//printf("User disconnected without registering");
		return exitWithoutRegister(s);
	}
	

}

int executeCommand(char* msg,SOCKET s) {
	// 0 SV_FULL
	// 1 SV_SUCCESS
	std::string message = std::string(msg);

	if (message.find("register") != std::string::npos)
		registerCommand(msg, s);
	else if (message.find("getlist") != std::string::npos)
		getListCommand(msg, s);
	else if (message.find("exit") != std::string::npos)
		exitCommand(msg, s);
	else if (message.find("getlog") != std::string::npos)
		getLogCommand(msg, s);
	else
		printf("UNREGISTERED COMMAND");

	return 1;
}


int ServerRecieveMessage(SOCKET Socket) {
	using namespace common;

	//Communication
	int size = 0;
	int result = tcp_recv_whole(Socket, (char*)&size, 1);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		if (error == WSAECONNRESET) {
			FD_CLR(Socket, &master);
			//graceful disconnect
			int currIndex = 0;
			for (int i = 0; i < users.size(); i++) {
				if (users[i].sock == Socket) {
					currIndex = i;
					break;
				}
			}
			printf("User ");
			printf(users[currIndex].name.c_str());
			printf(" disconnected [It was nice while it lasted :( ]\n");

			Server_log("Server disconnected ");
			Server_log(users[currIndex].name.c_str());
			Server_log(" with GRACE\n");
			users.erase(users.begin() + currIndex);


			return 0;
		}
		printf("DEBUG// recv is incorrect\n");
		return 0;

	}
	else
	{
		//printf("DEBUG// I used the recv function\n");
		//return 1;
	}

	char* buffer = new char[size];

	result = tcp_recv_whole(Socket, buffer, size);

	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		if (error == WSAECONNRESET) {
			FD_CLR(Socket, &master);

			printf("DEBUG// removed socket due to reset by user\n");

			int currIndex = 0;
			for (int i = 0; i < users.size(); i++) {
				if (users[i].sock == Socket)
					currIndex = i;
				break;
			}
			printf("User ");
			printf(users[currIndex].name.c_str());
			printf(" disconnected [It was nice while it lasted :( ]\n");

			Server_log("Server disconnected ");
			Server_log(users[currIndex].name.c_str());
			Server_log(" with GRACE\n");
			users.erase(users.begin() + currIndex);

			return 0;

		}
		printf("DEBUG// recv is incorrect\n");
		return 0;

	}
	else
	{
		//printf("DEBUG// I used the recv function\n");
		if (buffer[0] == '$') {
			std::string nama = getUsernameFromSocket(Socket);
			Server_log(nama.c_str());
			Server_log(": ");
			Server_log(std::string(buffer));
			Server_log("\n");

			printf(nama.c_str());
			printf(": ");
			printf(buffer);
			printf("\n");

			executeCommand(buffer,Socket);


		}
		else if (buffer[0] == '\0') {
			//recieved a blank message
		}
		else {//it is a public message

			//needs cleanup
			if (users.size() != 0) {//there is one user online
				std::string nama = getUsernameFromSocket(Socket);
				printf(nama.c_str());
				printf(": ");
				printf(buffer);
				printf("\n");


				nama.append(": ");
				Server_log(nama);
				Server_log(std::string(buffer));
				Server_log("\n");

				ServerSendMessage(buffer, Socket,true);
			}
			else {//nobody is online(the client of the first user is sending this message)
				printf("UNREGISTERED CLIENT");
				printf(": ");
				printf(buffer);
				printf("\n");

				Server_log("UNREGISTERED CLIENT: ");
				Server_log(std::string(buffer));
				Server_log("\n");
				ServerSendMessage(buffer, Socket, true);


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