#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

class Comunicator {
private:
	int iResult, iSendResult, recvbuflen = DEFAULT_BUFLEN;
	WSADATA wsaData;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	SOCKET ListenSocket = INVALID_SOCKET, ClientSocket = INVALID_SOCKET;
	char recvbuf[DEFAULT_BUFLEN];

	std::vector<std::thread> threads;

	int (*handler)(std::string data) = NULL;

	int printError(std::string message, int code);
	static void mainHandler(SOCKET client, int number);
public:
	int init();
	void setHandler(int (*handler)(std::string data));
	int listenS();
	void stopListenning();
};