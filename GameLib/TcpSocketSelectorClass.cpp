#include "TcpSocketSelectorClass.h"

std::mutex mtx;

void TcpSocketSelectorClass::Add(sf::TcpSocket*socket)
{
    mtx.lock();
    socketSelector.add(*socket);
    mtx.unlock();
}

void TcpSocketSelectorClass::Add(sf::TcpListener* listener)
{
    mtx.lock();
    socketSelector.add(*listener);
    mtx.unlock();
}

void TcpSocketSelectorClass::Remove(sf::TcpSocket* socket)
{
    mtx.lock();
    socketSelector.remove(*socket);
    mtx.unlock();
}

void TcpSocketSelectorClass::Remove(sf::TcpListener* socket)
{
    mtx.lock();
    socketSelector.remove(*socket);
    mtx.unlock();
}

bool TcpSocketSelectorClass::Wait()
{
    return socketSelector.wait();
}

bool TcpSocketSelectorClass::isReady(sf::TcpListener* listener)
{
    return socketSelector.isReady(*listener);
}

bool TcpSocketSelectorClass::isReady(sf::TcpSocket*socket)
{
    return socketSelector.isReady(*socket);
}


