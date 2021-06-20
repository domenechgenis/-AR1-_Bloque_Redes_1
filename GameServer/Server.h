#pragma once

#include "TcpSocket.h"
#include "TcpSocketSelectorClass.h"
#include "TcpListener.h"
#include "TcpStatusClass.h"

#include "Constants.h"
#include "Types.h"

#include <list>

class Server
{
private:
	//Data structure
	std::list<TcpSocketClass*> sv_clients;

	//Utils
	TcpSocketClass* sv_socket;
	TcpListenerClass* sv_listener;
	TcpSocketSelectorClass* sv_socketselector;
	TcpStatusClass* sv_status;

	bool running = false;
	Header header;

	void OpenListener();
	void DisconnectServer();
	void SendPackets(sf::TcpSocket&);

public:
	Server();
	~Server();

	void Run();
};

