#pragma once

#include "TcpSocket.h"
#include "TcpListener.h"
#include "Constants.h"
#include "Types.h"

#include <list>

class Server
{
private:
	//Data structure
	std::list<sf::TcpSocket*> clients;

	//Utils
	sf::TcpSocket* sv_socket;
	sf::TcpListener sv_listener;
	sf::SocketSelector sv_socketselector;
	sf::Socket::Status sv_status;

	bool running = false;
	Header header;

	void OpenListener();
	void DisconnectServer();
	void SendPackets(sf::TcpSocket&);


public:
	Server();
	void Run();
};

