#include "Client.h"
#include <memory>
#include <iostream>
//const unsigned SLEEP_TIME = 10;
using std::shared_ptr;

int main(){
	const char* host = "";
	int port = 7497;
	int clientId = 0;

	printf( "Start of C++ Socket Client Test\n");
	Slice* slice(new Slice);
	
	slice->imply_vol_queue->addTask([]{std::this_thread::sleep_for(std::chrono::seconds(5));std::cout<<"ending seconday"<<std::endl;});

	Client client(slice);
	client.m_pSlice->contractMonth = "202506";
	client.connect(host, port, clientId);
	while(client.isConnected()){
		client.processMessages();
	}
	printf ("End of C++ Socket Client Test\n");
}

