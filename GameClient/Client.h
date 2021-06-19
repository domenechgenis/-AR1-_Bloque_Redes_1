#pragma once

#include <iostream>
#include <stdlib.h>
#include <thread>
#include <queue>
#include <list>
#include <windows.h>

#include "TcpSocket.h"
#include "TcpListener.h"
#include "TcpStatusClass.h"
#include "TcpSocketSelectorClass.h"

#include "Constants.h"
#include "Types.h"

class Client
{
    std::list<TcpSocketClass*> pl_clients;
    std::mutex clientsSemaphore;

    //Abstracted SFML
    TcpSocketClass* pl_socket;
    TcpListenerClass* pl_listener;
    TcpSocketSelectorClass* pl_socketSelector;

    sf::Socket::Status pl_status;

    bool gameloop = false;
    Header header;

    void MatchMaker();

public:
    Client();
    void Run();
    void ConnectToBBS();
    void OpenListener();
    void ShowCurrentPlayers();
    void ListenToPlayers();
    void SocketSelectorListener();

    void DisconnectClient();
    void Wait4ServerPacket();
};
