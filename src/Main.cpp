#include "Client.h"
#include "Position.h"
#include <memory>
#include <map>
//const unsigned SLEEP_TIME = 10;
using std::shared_ptr;

int main(){
	const char* host = "";
	int port = 7497;
	int clientId = 0;

	printf( "Start of C++ Socket Client Test\n");
	shared_ptr<Slice> slice(new Slice);
	shared_ptr<std::map<long,Position>> positions = std::make_shared<std::map<long,Position>>();

	Client client(slice, positions);
	client.m_pSlice->contractMonth = "202506";
	client.connect(host, port, clientId);
	while(client.isConnected()){
		client.processMessages();
	}
	printf ("End of C++ Socket Client Test\n");
}

