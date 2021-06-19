#include "TcpListener.h"

TcpListenerClass::TcpListenerClass()
{
}

TcpListenerClass::~TcpListenerClass()
{
}

sf::TcpListener& TcpListenerClass::GetListener()
{
    return tcplistener;
}

sf::Socket::Status TcpListenerClass::Listen(unsigned int port, const sf::IpAddress& address = sf::IpAddress::Any)
{

    return tcplistener.listen(port, address);
}

sf::Socket::Status TcpListenerClass::Accept(sf::TcpSocket *socket)
{
    return tcplistener.accept(*socket);
}

void TcpListenerClass::Disconnect()
{
    tcplistener.close();
}
