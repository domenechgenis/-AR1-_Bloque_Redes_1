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
#include "Deck.h"

class Client
{
    std::list<TcpSocketClass*> pl_clients;

    int id;
    std::mutex clientsSemaphore;

    //Abstracted SFML
    TcpSocketClass* pl_socket;
    TcpListenerClass* pl_listener;
    TcpSocketSelectorClass* pl_socketSelector;
    TcpStatusClass * pl_status;

    bool gameloop = false;
    Header header;

public:
    Client();
    ~Client();

    void Run();

    bool ConnectToBBS();
    bool OpenListener();

    void ShowCurrentPlayers();
    void ListenToPlayers();
    void SocketSelectorListener();

    void DisconnectClient();
    void Wait4ServerPacket();
};
