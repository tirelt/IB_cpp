#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <thread>

#include "Client.h"

const unsigned MAX_ATTEMPTS = 50;
const unsigned SLEEP_TIME = 10;

int main(){
	const char* host = "";
	int port = 7497;
	int clientId = 0;

	printf( "Start of C++ Socket Client Test\n");

	Client client;
	client.connect(host, port, clientId);
	while(client.isConnected()){
		client.processMessages();
		std::this_thread::sleep_for(std::chrono::seconds(SLEEP_TIME));
	}
	printf ("End of C++ Socket Client Test\n");
}

