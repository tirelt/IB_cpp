#include "Client.h"
#include "Position.h"
#include "Orders.h"
#include <memory>
#include <map>
//const unsigned SLEEP_TIME = 10;
using std::shared_ptr;
using std::make_shared;
using std::map;
using std::pair;

int main(){
	const char* host = "";
	int port = 7497;
	int clientId = 0;

	printf( "Start of C++ Socket Client\n");
	shared_ptr<Slice> slice(new Slice);
	shared_ptr<map<long,Position>> positions = make_shared<map<long,Position>>();	
	shared_ptr<map<long,LiveOrder>> orders = make_shared<map<long,LiveOrder>>();	
	Client client(slice, positions, orders);
	slice->contractMonth = "202506";
	client.connect(host, port, clientId);
	while(client.isConnected()){
		client.processMessages();
	}
	printf ("End of C++ Socket Client\n");
}

