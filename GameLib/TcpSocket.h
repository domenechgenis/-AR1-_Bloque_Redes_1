#pragma once
#include <iostream>

#include <SFML\Network.hpp>

class TcpSocketClass
{
	sf::TcpSocket *tcpSocket;

	int id;
	bool rdy;

public:
	TcpSocketClass();
	TcpSocketClass(sf::TcpSocket*);
	~TcpSocketClass();

	sf::TcpSocket* GetSocket();
	void SetSocket(sf::TcpSocket*);

	void SetId(int);
	int GetId();

	void SetRdy(bool);
	bool GetRdy();

	sf::IpAddress GetRemoteIp();
	std::string GetRemoteAdress();

	unsigned short GetRemotePort();
	unsigned short GetRemoteLocalPort();

	sf::Socket::Status Connect(const std::string localhost, const unsigned short puerto, sf::Time seconds);
	void Disconnect();

	sf::Socket::Status Send(sf::Packet&);
	sf::Socket::Status Recieve(sf::Packet&);
};

