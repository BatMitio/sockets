#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#include "Comunicator.h"

int func(std::string data) {
	return 0;
}

int main() {
	Comunicator comunicator;
	comunicator.init();
	comunicator.setHandler(*func);
	comunicator.listenS();
	return 0;
}