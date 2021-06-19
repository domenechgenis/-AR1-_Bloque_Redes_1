#pragma once

#include "TcpListener.h"

#include <SFML\Network.hpp>
#include <mutex>

class TcpSocketSelectorClass
{
private:
	sf::SocketSelector sv_socketSelector;
public:
	//Overloaded functions, accept sockets or listeners from socket selector
	void Add(sf::TcpSocket*);
	void Add(sf::TcpListener*);

	//Overloaded functions, remove sockets or listeners from socket selector
	void Remove(sf::TcpSocket*);
	void Remove(sf::TcpListener*);

	//Wait for connections
	bool Wait();

	//Wait for connections
	bool isReady(sf::TcpListener*);
	bool isReady(sf::TcpSocket*);
};

