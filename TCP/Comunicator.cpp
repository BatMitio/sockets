#include "Comunicator.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")



int Comunicator::init() {
	this->iResult = WSAStartup(MAKEWORD(2, 2), &this->wsaData);
	if (this->iResult != 0) return this->printError("WSAStartup failed", this->iResult);

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	this->iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (this->iResult != 0) return this->printError("getaddrinfo failed", this->iResult);

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) return this->printError("Error at socket()", WSAGetLastError());

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (this->iResult != 0) return this->printError("bind failed with error", WSAGetLastError());
	freeaddrinfo(result);

	return this->iResult;
}

void Comunicator::setHandler(int (*handler)(std::string data)) {
	this->handler = handler;
}

static void mainHandler(SOCKET client, int number) {
	char buffer[DEFAULT_BUFLEN];
	int resultCode = 0;
	do {

		resultCode = recv(client, buffer, DEFAULT_BUFLEN, 0);
		if (resultCode > 0) {
			printf("Bytes received: %d\n", resultCode);
			std::string responseBody = "{message:'This is so cool',excitement:100}";

			std::string response =
				"HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
				+ std::to_string(responseBody.length()) +
				"\r\n\r\n" + responseBody;
			std::cout << response << std::endl;
			resultCode = send(client, response.c_str(), response.length(), 0);
			if (resultCode == SOCKET_ERROR) {
				printf("send failed from thread: %d\n", number);
				closesocket(client);
				break;
			}
			printf("Bytes sent: %d\n", resultCode);
		}
		else if (resultCode == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed from thread %d\n", number);
			closesocket(client);
			break;
		}

	} while (client > 0);
	closesocket(client);
}

int Comunicator::listenS() {
	if (this->handler == NULL)
		return this->printError("Handler not specified", 1);
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) return this->printError("Listen failed with error", WSAGetLastError());
	
	while (this->ClientSocket == INVALID_SOCKET)
	{
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			this->printError("accept failed", WSAGetLastError());
			continue;
		}

		SOCKET newSocket = ClientSocket;
		std::thread newThread(&mainHandler, newSocket, this->threads.size());
		this->threads.push_back(newThread);
	}
}

void Comunicator::stopListenning() {

}

int Comunicator::printError(std::string message, int code) {
	std::cout << message << ": " << code << std::endl;
	if(result != NULL)
		freeaddrinfo(result);
	if(ListenSocket != INVALID_SOCKET)
		closesocket(ListenSocket);
	WSACleanup();
	return code;
}

