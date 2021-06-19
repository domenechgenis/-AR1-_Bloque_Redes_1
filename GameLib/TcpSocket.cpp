#include "TcpSocket.h"

TcpSocketClass::TcpSocketClass()
{
    tcpSocket = new sf::TcpSocket;
}

TcpSocketClass::TcpSocketClass(sf::TcpSocket*_socket)
{
    _socket = new sf::TcpSocket;
}

TcpSocketClass::~TcpSocketClass()
{
}

sf::TcpSocket* TcpSocketClass::GetSocket()
{
    return tcpSocket;
}

void TcpSocketClass::SetSocket(sf::TcpSocket*_socket)
{
    this->tcpSocket = _socket;
}

sf::IpAddress TcpSocketClass::GetRemoteIp()
{
    return tcpSocket->getRemoteAddress();
}

std::string TcpSocketClass::GetRemoteAdress()
{
    return tcpSocket->getRemoteAddress().toString();
}

unsigned short TcpSocketClass::GetRemotePort()
{
    return tcpSocket->getRemotePort();
}

unsigned short TcpSocketClass::GetRemoteLocalPort()
{
    return tcpSocket->getLocalPort();
}

sf::Socket::Status TcpSocketClass::Connect(const std::string localhost,const unsigned short puerto, sf::Time seconds)
{
    
    return tcpSocket->connect(localhost, puerto, seconds);
}

void TcpSocketClass::Disconnect()
{
    tcpSocket->disconnect();
}

sf::Socket::Status TcpSocketClass::Send(sf::Packet &packet)
{
    return tcpSocket->send(packet);
}

sf::Socket::Status TcpSocketClass::Recieve(sf::Packet &packet)
{
    return tcpSocket->receive(packet);
}
