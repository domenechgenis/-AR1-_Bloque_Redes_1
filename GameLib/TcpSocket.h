#pragma once
#include <iostream>

#include <SFML\Network.hpp>

class TcpSocketClass
{
	sf::TcpSocket *tcpSocket;
	int id;
public:
	TcpSocketClass();
	TcpSocketClass(sf::TcpSocket*);
	~TcpSocketClass();

	sf::TcpSocket* GetSocket();
	void SetSocket(sf::TcpSocket*);

	void SetID(int);
	int GetID();
	sf::IpAddress GetRemoteIp();
	std::string GetRemoteAdress();

	unsigned short GetRemotePort();
	unsigned short GetRemoteLocalPort();

	sf::Socket::Status Connect(const std::string localhost, const unsigned short puerto, sf::Time seconds);
	void Disconnect();

	sf::Socket::Status Send(sf::Packet&);
	sf::Socket::Status Recieve(sf::Packet&);
};

