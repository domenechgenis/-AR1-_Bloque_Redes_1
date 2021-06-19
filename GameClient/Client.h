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

public:
    Client();
    void Run();
    void ConnectToBBS();
    void OpenListener();
    void Wait4ServerPacket();
    void ShowCurrentPlayers();
    void ListenToPlayers();
    void InitializeSocketSelectorThread();
    void SocketSelectorListener();
    void GameLoop();

    void DisconnectClient();
};
