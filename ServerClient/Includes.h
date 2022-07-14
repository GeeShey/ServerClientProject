// ServerInit.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma once

#define _CRT_SECURE_NO_WARNINGS                 // turns of deprecated warnings
#define _WINSOCK_DEPRECATED_NO_WARNINGS         // turns of deprecated warnings for winsock
constexpr auto SV_FULL = "2";
constexpr auto SV_SUCCESS = "1";
constexpr auto SV_ALREADYREGISTERED = "3";


#include <winsock2.h>
//#include <ws2tcpip.h>                         // only need if you use inet_pton
#pragma comment(lib,"Ws2_32.lib")
#include <iostream>
#include <string>
#include<vector>
#include <fstream>

// loop send func
namespace common {


	int tcp_recv_whole(SOCKET s, char* buf, int len)
	{
		int total = 0;

		do
		{
			int ret = recv(s, buf + total, len - total, 0);
			if (ret < 1)
				return ret;
			else
				total += ret;

		} while (total < len);

		return total;
	}

	int tcp_send_whole(SOCKET skSocket, const char* data, uint16_t length)
	{
		int result;
		int bytesSent = 0;

		while (bytesSent < length)
		{
			result = send(skSocket, (const char*)data + bytesSent, length - bytesSent, 0);

			if (result <= 0)
				return result;

			bytesSent += result;
		}

		return bytesSent;
	}

}

// loop recv func

