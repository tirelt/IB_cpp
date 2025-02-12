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

	unsigned attempt = 0;
	printf( "Start of C++ Socket Client Test %u\n", attempt);

	for (;;) {
		++attempt;
		printf( "Attempt %u of %u\n", attempt, MAX_ATTEMPTS);

		Client client;

		client.connect( host, port, clientId);
		
		while( client.isConnected()) {
			client.processMessages();
		}
		if( attempt >= MAX_ATTEMPTS) {
			break;
		}

		printf( "Sleeping %u seconds before next attempt\n", SLEEP_TIME);
		std::this_thread::sleep_for(std::chrono::seconds(SLEEP_TIME));
	}

	printf ( "End of C++ Socket Client Test\n");
}

