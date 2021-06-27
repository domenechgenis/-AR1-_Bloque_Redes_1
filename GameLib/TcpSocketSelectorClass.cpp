#include "TcpSocketSelectorClass.h"

std::mutex mtx;

void TcpSocketSelectorClass::Add(sf::TcpSocket*socket)
{
    mtx.lock();
    sv_socketSelector.add(*socket);
    mtx.unlock();
}

void TcpSocketSelectorClass::Add(sf::TcpListener* listener)
{
    mtx.lock();
    sv_socketSelector.add(*listener);
    mtx.unlock();
}

void TcpSocketSelectorClass::Remove(sf::TcpSocket* socket)
{
    mtx.lock();
    sv_socketSelector.remove(*socket);
    mtx.unlock();
}

void TcpSocketSelectorClass::Remove(sf::TcpListener* socket)
{
    mtx.lock();
    sv_socketSelector.remove(*socket);
    mtx.unlock();
}

bool TcpSocketSelectorClass::Wait()
{
    return sv_socketSelector.wait();
}

bool TcpSocketSelectorClass::isReady(sf::TcpListener* listener)
{
    return sv_socketSelector.isReady(*listener);
}

bool TcpSocketSelectorClass::isReady(sf::TcpSocket*socket)
{
    return sv_socketSelector.isReady(*socket);
}


