#include "TcpStatusClass.h"

TcpStatusClass::TcpStatusClass()
{
}

sf::Socket::Status TcpStatusClass::GetStatus()
{
	return status;
}

sf::Socket::Status TcpStatusClass::SetStatus(sf::Socket::Status status_)
{
	return status = status_;
}
