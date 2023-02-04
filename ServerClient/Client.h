#pragma once
#include "Includes.h"
auto client_ip = INADDR_ANY;
int client_port = 31337;
bool CONNECTION_SUCCESFUL = false;
SOCKET Client_ComSocket,client_udp;
std::string CLIENT_LOG_FILENAME = "client_log.txt";
std::ofstream cofs;


using namespace common;
int ClientRecieveMessage(std::string& out, bool displayDataToUser = true) {
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
		//printf("DEBUG// I used the recv function\n");
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
		//printf("DEBUG// I used the recv function\n");
	}

	if (displayDataToUser) {
		//printf("\nDEBUG// I received a message from the server\n");

		printf("Server: ");
		printf(buffer);
		printf("\n");
	}

	out = std::string(buffer);
	delete[] buffer;
	return 1;
}

int ClientRecieveBigMessage(std::string& out, bool displayDataToUser = false) {
	//Communication

	std::string length;
	recv(Client_ComSocket, &length[0], 255, 0);
	//printf("recieved length ");
	//printf(length.c_str());
	//printf("\n");
	int length_int = stoi(length);//CONVERTS STRING TO INT

	char* buffer = new char[length_int];

	for (int i = 0; i < length_int; i++) {
		recv(Client_ComSocket, &buffer[i], 1, 0);//sending the size of the buffer
	}
	out.append(std::string(buffer));
	delete[] buffer;

	return 1;
}

int sendMessageFromClient(char* sendbuffer) {
	//Communication
	uint8_t size = 255;
	//memset(sendbuffer, 0, 255);
	
	int result = tcp_send_whole(Client_ComSocket, (char*)&size, 1);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		if (error == WSAECONNRESET) {

			printf("DEBUG// server disconnected due to reset by user\n");

			CONNECTION_SUCCESFUL = false;

			return 0;

		}
		printf("DEBUG// send is incorrect\n");
		return 0;

	}
	else
	{
		//printf("DEBUG// I used the send function\n");
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
		//printf("DEBUG// I used the send function\n");
	}
	return 1;
}

int clientShutdown() {
	shutdown(Client_ComSocket, SD_BOTH);
	closesocket(Client_ComSocket);
	return 1;
}

void Client_log(std::string s) {
	cofs.open(CLIENT_LOG_FILENAME, std::ios_base::app);
	cofs << s;
	cofs.close();
}

void UDP_client_init() {
	int val = 1;

	client_udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int result = setsockopt(client_udp, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val));
	if (result < 0)
	{
		int err = WSAGetLastError();
		printf("Error: ");
		printf(std::to_string(err).c_str());
		printf("\n");

		return;
	}
	else
	{
		//printf("DEBUG// I used the Connect function\n");
	}

	sockaddr_in bcAddr;
	bcAddr.sin_family = AF_INET;
	bcAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bcAddr.sin_port = htons(9009);

	result = bind(client_udp, (sockaddr *) &bcAddr, sizeof(sockaddr_in));
	if (result <0)
	{
		int err = WSAGetLastError();
		printf("Error: ");
		printf(std::to_string(err).c_str());
		printf("\n");

		return;
	}
	else
	{
		//printf("DEBUG// I used the Connect function\n");
	}

}

void client_recv_UDP(char* buffer, int length) {
	sockaddr_in bcAddr;
	int len = sizeof(bcAddr);

	int result = recvfrom(client_udp, buffer, length, 0, (sockaddr*)&bcAddr,&len);
	if (result <0)
	{
		int err = WSAGetLastError();
		printf("Error: ");
		printf(std::to_string(err).c_str());
		printf("\n");

		return;
	}
	else
	{
		//printf("DEBUG// I used the Connect function\n");
	}

}

void ClientSetup()
{

	char* msgIN = new char[15];

	WSADATA wsadata;
	WSAStartup(WINSOCK_VERSION, &wsadata);

	UDP_client_init();

	client_recv_UDP(msgIN, 15);

	std::string choice;
	//printf("Enter IP address\n");
	//std::cin >> choice;

	//printf("Enter port\n");
//	std::cin >> client_port;


	std::string msg(msgIN);

	choice = msg.substr(0, msg.find_first_of(':'));
	client_ip = inet_addr(choice.c_str());
	client_port = std::stoi(msg.substr(msg.find_first_of(':') + 1, msg.length()-1));
	printf("Recieved UDP Broadcast Succesfully\n");
	printf(msg.c_str());
	printf("\n");



	//Socket
	Client_ComSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Client_ComSocket == INVALID_SOCKET)
	{
		printf("DEBUG// Socket function incorrect\n");
		return;
	}
	else
	{
		//printf("DEBUG// I used the socket function\n");
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
		//printf("DEBUG// I used the Connect function\n");
	}

	//Communication
	/*char test[]  = "Test Message\0";
	sendMessageFromClient(test);
	std::cout << "[Sent a test message]\n";
	std::string TestRecieve;

	ClientRecieveMessage(TestRecieve);
	if (TestRecieve._Equal("Test Message"))
		printf("Server echo successful\n");
	else
		printf("There is a transmisiion error(echo from the server was incorrect\n");*/
	std::string username;
	printf("Enter username for registration(Usernames cannot be separated by spaces) or type $exit to exit\n");
	std::cin >> username;
	std::string COMMAND_RESULT;

	if (username.find("$exit") != std::string::npos) {
		sendMessageFromClient(&username[0]);
		if (ClientRecieveMessage(COMMAND_RESULT) == 1) {
			printf(COMMAND_RESULT.c_str());
			printf("\n");
			if (COMMAND_RESULT[0] == '2') {
				//server said that you need to disconnect
				CONNECTION_SUCCESFUL = false;
				printf("CONNECTION TERMINATED!\n");
			}
		}
	}
	else {
		std::string command = "$register ";
		command.append(username);
		sendMessageFromClient(&command[0]);
		std::string USER_REGISTRATION_RESULT;
		if (ClientRecieveMessage(USER_REGISTRATION_RESULT) == 1) {
			if (USER_REGISTRATION_RESULT._Equal(SV_SUCCESS)) {
				//printf("user registration successfull\n");
				printf("----------------WELCOME TO THE CHATROOM----------------\n");
				CONNECTION_SUCCESFUL = true;
			}
			else{
				printf("user registration unsuccessfull, chat-room is full\n");
				CONNECTION_SUCCESFUL = false;

			}
		}
	}

	while (CONNECTION_SUCCESFUL) {
		//ClientRecieveMessage();
		std::string s;
		std::getline(std::cin, s);
		bool isCommand = false;
		if (s[0] == '$') {
			isCommand = true;
		}
		if (isCommand) {
			sendMessageFromClient(&s[0]);
			if (s.find("getlog") != std::string::npos) {
				if (ClientRecieveBigMessage(COMMAND_RESULT) == 1) {
					printf(COMMAND_RESULT.c_str());
					printf("\n");
					printf("Created a file called ");


					cofs.open(CLIENT_LOG_FILENAME, std::ofstream::out | std::ofstream::trunc);
					cofs.close();//clearing the file
					Client_log(COMMAND_RESULT);

					printf(CLIENT_LOG_FILENAME.c_str());
					printf("\n");



				}
				else {
					printf("ERROR WITH COMMAND");
				}
			}
			else if (s.find("getlist") != std::string::npos) {
				if (ClientRecieveBigMessage(COMMAND_RESULT) == 1) {
					printf(COMMAND_RESULT.c_str());
					printf("\n");
				}
				else {
					printf("ERROR WITH COMMAND");
				}
			}
			//wait for response
			//comeback to this pls
			else if (s.find("exit") != std::string::npos) {
				if (ClientRecieveMessage(COMMAND_RESULT) == 1) {
					printf(COMMAND_RESULT.c_str());
					printf("\n");
					if (COMMAND_RESULT[0] == '2') {
						//server said that you need to disconnect
						CONNECTION_SUCCESFUL = false;
						printf("CONNECTION TERMINATED!\n");
					}
				}
			}
			else if (s.find("register") != std::string::npos) {
				if (ClientRecieveMessage(COMMAND_RESULT) == 1) {
					printf("ALREADY REGISTERED!\n");
				}
			}
			else {
				printf("ERROR WITH COMMAND");
			}
		}
		else {
			if (s[0] != ' ' && s[0] != '\0') {
				sendMessageFromClient(&s[0]);
				std::string RESULT;
				ClientRecieveMessage(RESULT);//echo
				//printf("/Debug ^this is the echo\n");
			}
		}

	}
	// close sockets
	clientShutdown();

}
