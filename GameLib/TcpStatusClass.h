 #pragma once

#include <SFML\Network.hpp>

class TcpStatusClass
{
public:
	TcpStatusClass();

	sf::Socket::Status GetStatus();
	sf::Socket::Status SetStatus(sf::Socket::Status);

private:
	sf::Socket::Status status;
};

