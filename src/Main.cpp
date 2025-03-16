#include "Client.h"
#include <memory>
//const unsigned SLEEP_TIME = 10;
using std::shared_ptr;

int main(){
	const char* host = "";
	int port = 7497;
	int clientId = 0;

	printf( "Start of C++ Socket Client Test\n");
	std::shared_ptr<Slice> slice(new Slice);
	
	Client client(slice);
	client.m_pSlice->contractMonth = "202506";
	client.connect(host, port, clientId);
	while(client.isConnected()){
		client.processMessages();
	}
	printf ("End of C++ Socket Client Test\n");
}

