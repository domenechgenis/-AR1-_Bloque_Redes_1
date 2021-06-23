#pragma once

#include <iostream>
#include <stdlib.h>
#include <thread>
#include <queue>
#include <list>
#include <map>
#include <windows.h>
#include <algorithm>

#include "TcpSocket.h"
#include "TcpListener.h"
#include "TcpStatusClass.h"
#include "TcpSocketSelectorClass.h"

#include "Constants.h"
#include "Types.h"
#include "Deck.h"
#include "Hand.h"

class Client
{
    std::list<TcpSocketClass*> pl_clients;
    std::mutex clientsSemaphore;

    //Abstracted SFML
    TcpSocketClass* pl_socket;
    TcpListenerClass* pl_listener;
    TcpSocketSelectorClass* pl_socketSelector;
    TcpStatusClass * pl_status;
    Deck *deck;

    std::map<int, Hand*> hands;

    bool gameloop,gamestarded,rdy;
    int seed,id;

    HEADER_MSG header;

    //In game variables
    int player;
    Culture culture;
    Family family;

public:
    Client();
    ~Client();

    void Run();

    bool ConnectToBBS();
    bool OpenListener();

    void ShowCurrentPlayers();
    void ListenToPlayers();

    void Wait4ServerPacket();
    void AssignDeck();
    void DealCards();
    void AsignHandsAndTurn();
    void Wait4Rdy();
    void CreateOrJoinRoom();

    //Handle Main Reciever
    void HandlePacketReciever(sf::Packet&, TcpSocketClass*);
    void HandleRdyReciever(sf::Packet&, TcpSocketClass*);
    void HandleTurnReciever(sf::Packet&, TcpSocketClass*);

    void HandlePlayerTurn();
    void HandlePlayerDecision();

    //Utils
    std::string HeaderToString(HEADER_MSG);
    std::string castSwitchToStringCulture(Culture);
    std::string castSwitchToStringType(Family);
    int castStringToIntCulture(std::string culture);
    int castStringToIntType(std::string types);

    //In Game Extracted functions
    void ExtractPlayer();
    void ExtractCulture();
    void ExtractFamily();
    void CreateRoom();
    void JoinRoom();

    //Threads
    void SocketSelectorListener();
    void CheckPlayersRdy();
};
