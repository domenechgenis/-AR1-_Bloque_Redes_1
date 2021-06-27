#pragma once
#include <SFML\Network.hpp>

class TcpListenerClass
{
private:
	sf::TcpListener tcplistener;
public:
	TcpListenerClass();
	~TcpListenerClass();

	sf::TcpListener &GetListener();
	sf::Socket::Status Listen(unsigned int,const sf::IpAddress&);
	sf::Socket::Status Accept(sf::TcpSocket*);
	void Disconnect();
};

