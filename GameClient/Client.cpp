#include "Client.h"

Client::Client()
{
	//Create memory for Pointers
	pl_socket = new TcpSocketClass();
	pl_listener = new TcpListenerClass();
	pl_socketSelector = new TcpSocketSelectorClass();
	pl_status = new TcpStatusClass();
}

Client::~Client()
{
	delete[] pl_socket;
	delete[] pl_listener;
	delete[] pl_socketSelector;
	delete[] pl_status;
}

void Client::Run()
{
	//Connect player to Port 50.000
	if (!ConnectToBBS() || !OpenListener()) 
	{
		std::cout << "El cliente no ha podido conectarse a ningun servidor, cerrando el programa... \n";
		Sleep(DEFAULT_SLEEP);
		return;
	}

	//Wait4ServerPackets
	Wait4ServerPacket();

	ShowCurrentPlayers();

	//Listen to players connected
	ListenToPlayers();

	//Assign deck to player
	system("cls");
	AssignDeck();

	AsignHandsAndTurn();
	DealCards();

	//Listener thread
	std::thread socketselectorListener(&Client::SocketSelectorListener, this);
	socketselectorListener.detach();

	std::cout << "Mi mano es" << id << std::endl;
	std::cout << "Cantidad de cartas : "<< hands[id]->numCards<< std::endl;
	hands[id]->PrintHand();
	std::cout <<"--------------------------------------------------"<< std::endl;

	std::cout << "Manos de los otros : "<< std::endl;
	for (int i = 0; i < 4; i++)
	{
		if (i != id)
		{
			std::cout << "Cantidad de cartas : " << hands[i]->numCards << std::endl;
			std::cout << "La mano del jugador  " << i << " es:" << std::endl;
			hands[i]->PrintHand();
		}
	}

	system("cls");
	//Match Start
	std::cout << "TODOS LOS JUGADORES HAN SIDO CONECTADOS ESPERANDO A QUE ESTEN LISTOS" << std::endl;

	while (gameloop)
	{
		Wait4Rdy();

		while (gamestarded)
		{
			HandlePlayerTurn();
		}
	}
}

bool Client::ConnectToBBS()
{
	//Nos conectamos al BBS por el puerto 50000
	std::cout << "\nPlayer inicializado, vas a connectarte al servidor por el puerto:" << DEFAULT_PORT << std::endl;
	pl_status->SetStatus(pl_socket->Connect(LOCAL_HOST, DEFAULT_PORT, sf::milliseconds(15.f)));

	if (pl_status->GetStatus() == sf::Socket::Done) 
	{
		std::cout << "Connectado al servidor correctamente... \n";
		return true;
	}
	else 
	{
		std::cout << "Error conectandote al Servidor \n";
		std::string rentry;
		std::cout << "Quieres volver a intentarlo? (y = si) \n";
		std::cin >> rentry;
		if (rentry == "y") 
		{
			ConnectToBBS();
		}
		else 
		{
			std::cout << "No hay ningun servidor abierto actualmente... \n";
			return false;
		}
	}
}

bool Client::OpenListener()
{
	//Abrimos el listener y lo añadimos al socket selector para posteriormente recibir los paquetes
	std::cout << "Me dispongo a escuchar por el puerto " << pl_socket->GetRemoteLocalPort() << std::endl;
	pl_status->SetStatus(pl_listener->Listen(pl_socket->GetRemoteLocalPort(), sf::IpAddress::LocalHost));

	if (pl_status->GetStatus() == sf::Socket::Done)
	{
		std::cout << "Listener abierto correctamente\n";
		pl_socketSelector->Add(&pl_listener->GetListener());
		return true;
	}
	else {
		std::cout << "Error al abrir el listener\n";
		return false;
	}
}

void Client::ShowCurrentPlayers()
{
	std::cout << "Clientes connectados actualmente: " << pl_clients.size() << std::endl;

	for (auto const& i : pl_clients) 
	{
		std::cout << "Cliente: " << i + 1;
		std::cout << " Puerto ---> " << i->GetRemotePort() << std::endl;
	}
}

void Client::ListenToPlayers()
{
	//Mientras no haya 3 jugadores el jugador espera escuchando alguna conexion de los otros players
	while (pl_clients.size() < 3) 
	{
		if (pl_socketSelector->Wait())
		{
			if (pl_socketSelector->isReady(&pl_listener->GetListener()))
			{
				TcpSocketClass* player = new TcpSocketClass();

				if (pl_listener->Accept(player->GetSocket()) == sf::Socket::Done)
				{
					std::cout << "Connexion recibia del cliente: " << player->GetRemotePort() << std::endl;

					pl_socketSelector->Add(player->GetSocket());

					this->clientsSemaphore.lock();
					pl_clients.push_back(player);
					this->clientsSemaphore.unlock();

					std::cout << "Clientes connectados actualmente: " << pl_clients.size() << std::endl;
				}
				else {
					delete player;
					std::cout << "Error al recibir un player\n";
					Sleep(DEFAULT_SLEEP);
					exit(0);
				}
			}
		}
		else 
		{
			std::cout << "Error al abrir listener\n";
			Sleep(DEFAULT_SLEEP);
			exit(0);
		}
	}

	gameloop = true;
}

void Client::HandlePacketReciever(sf::Packet& packet, TcpSocketClass* client)
{
	this->clientsSemaphore.lock();

	std::cout << "\nEstoy recibiendo un paquete del Cliente: " << client->GetRemotePort() << std::endl;
	int header_int = 0;

	packet >> header_int;
	header = HEADER_MSG(header_int);

	std::cout << "\nEstoy recibiendo la cabecera: " << header_int << std::endl;

	switch (header)
	{
	case MSG_NULL:
		break;
	case MSG_OK:
		break;
	case MSG_KO:
		break;
	case MSG_PEERS:
		break;
	case MSG_RDY:
		HandleRdyReciever(packet,client);
		break;
	case MSG_TURN:
		HandleTurnReciever(packet, client);
		break;
	}

	packet.clear();

	this->clientsSemaphore.unlock();
}

void Client::HandleRdyReciever(sf::Packet& packet, TcpSocketClass* client)
{
	for (auto const& i : pl_clients)
	{
		if (client->GetRemotePort() == i->GetRemotePort())
		{
			i->SetRdy(true);
		}
	}
}

void Client::HandleTurnReciever(sf::Packet& packet, TcpSocketClass* client)
{
	std::cout << "Holaaa" << std::endl;
}

void Client::Wait4ServerPacket()
{
	sf::Packet packet;
	std::string str_port;
	std::string numberPlayers;
	int nPlayers;

	pl_status->SetStatus(pl_socket->Recieve(packet));

	if (pl_status->GetStatus() == sf::Socket::Done) 
	{
		packet >> numberPlayers;
		nPlayers = std::stoi(numberPlayers);

		//Recibo cuantos players hay incluido yo
		for (size_t i = 0; i < nPlayers; i++)
		{
			packet >> str_port;
			int puerto = std::stoi(str_port);

			//Connecto el puerto que me lleva a un socket;
			TcpSocketClass* player = new TcpSocketClass();
			pl_status->SetStatus(player->Connect(pl_socket->GetRemoteAdress(), puerto, sf::milliseconds(15.f)));
				
			if (pl_status->GetStatus() == sf::Socket::Done) {

				//Add this client to the list because connection its done
				std::cout << "Me connecto correctamente a " << player->GetRemotePort() << std::endl;
				player->SetId(i);
				pl_clients.push_back(player);
				pl_socketSelector->Add(player->GetSocket());

			}
			else {
				std::cout << "Error al connectar el jugador a la partida";
				delete player;
				Sleep(DEFAULT_SLEEP);
				exit(0);
			}
		}
		id = nPlayers;
	}
	else 
	{
		std::cout << "Nadie me envia nada" << std::endl;
	}
	packet.clear();

}

void Client::AssignDeck()
{
	deck = new Deck();

	if (id == 0) 
	{
		seed = pl_socket->GetRemoteLocalPort();
		deck->ShuffleDeck(seed);
	}
	else
	{
		for (auto const& i : pl_clients) 
		{
			if (i->GetId() == 0)
			{
				seed = i->GetRemotePort();
				deck->ShuffleDeck(seed);
				break;
			}
		}
	}
}

void Client::AsignHandsAndTurn() 
{
	for (size_t i = 0; i < 4; i++)
	{
		hands[i] = new Hand();
		hands[i]->inGame = true;

		//Asignamos aqui el turno correctamente 
		hands[i]->playerTurn = 0;
	}
}

void Client::Wait4Rdy()
{
	if (!this->rdy) 
	{
		std::string msg;
		std::cout << "\nINTRODUCE (r) cuando estes listo para empezar la partida: ";
		std::cin >> msg;

		if (msg == "r")
		{
			std::cout << "\nESTAS RDY PARA EMPEZAR LA PARTIDA, MIRANDO SI LOS OTROS LO ESTAN" << std::endl;
			this->rdy = true;

			//Check if other players are rdy
			std::thread checkPlayersRdyListener(&Client::CheckPlayersRdy, this);
			checkPlayersRdyListener.detach();

			//Send Rdy
			sf::Packet packet;
			for (auto const& i : pl_clients)
			{
				//When packet its ready, send it to connected user
				packet << HEADER_MSG::MSG_RDY << true;
				pl_status->SetStatus(i->Send(packet));

				if (pl_status->GetStatus() == sf::Socket::Done)
				{
					std::cout << "El paquete se ha enviado correctamente\n";
					packet.clear();
				}
				else {
					std::cout << "El paquete no se ha podido enviar\n";
				}
			}
		}
	}
}

void Client::DealCards()
{
	for (int i = 0; i < deck->deck.size(); i++)
	{
		if (i <= 10) 
		{
			hands[0]->addCard(*deck->deck[i]);	
		}
		else if (i>10 && i<=20)
		{
			hands[1]->addCard(*deck->deck[i]);
		}
		else if (i > 20 && i <= 30) 
		{
			hands[2]->addCard(*deck->deck[i]);
		}
		else 
		{
			hands[3]->addCard(*deck->deck[i]);
		}
	}
}

/// <summary>
/// Threads functions

void Client::SocketSelectorListener()
{
	while (gameloop)
	{
		for (TcpSocketClass* client : pl_clients)
		{
			if (pl_socketSelector->Wait())
			{
				if (pl_socketSelector->isReady(client->GetSocket()))
				{
					// The client has sent some data, we can receive it
					sf::Packet packet;
					pl_status->SetStatus(client->Recieve(packet));

					if (pl_status->GetStatus() == sf::Socket::Done)
					{
						HandlePacketReciever(packet, client);
					}
					else if (pl_status->GetStatus() == sf::Socket::Disconnected)
					{
						pl_socketSelector->Remove(client->GetSocket());
						pl_clients.remove(client);
						std::cout << "Elimino el socket que se ha desconectado\n";
					}
					else
					{
						std::cout << "Error al recibir de " << client->GetRemoteLocalPort() << std::endl;
					}
				}
			}
		}
	}
}

/// <summary>
/// In game Functions

void Client::HandlePlayerTurn()
{
	if (id == hands[0]->playerTurn)
	{
		system("cls");

		std::cout << "Esta es tu mano actual:" << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;

		hands[id]->PrintHand();
		std::cout << "--------------------------------------------------" << std::endl;

		//Extract turn
		std::string confirm;
		do 
		{
			ExtractPlayer();
			ExtractCulture();
			ExtractFamily();

			std::cout << "Has escogido quitarle " << castSwitchToStringCulture(culture) << " " << castSwitchToStringType(family) << " al jugador " << player << std::endl;
			std::cout << "Quieres hacer esto (s) o hacer algun cambio (c): ";
			std::cin >> confirm;

			if (confirm == "c") 
			{
				std::cout << "Re-haciendo la decision... " << std::endl;
			}

		} while (confirm != "s");

		HandlePlayerDecision();
	}
	else
	{
		system("cls");

		std::cout << "Esta es tu mano actual:" << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;

		hands[id]->PrintHand();
		std::cout << "NO es tu turno, esperando a que te toque" << std::endl;

		Sleep(1000);
	}
}

void Client::HandlePlayerDecision() 
{
	sf::Packet packet;

	for (auto const& i : pl_clients) 
	{
		packet << HEADER_MSG::MSG_TURN << id << culture << family;
		pl_status->SetStatus(i->Send(packet));

		if (pl_status->GetStatus() == sf::Socket::Done)
		{
			std::cout << "El paquete se ha enviado correctamente\n";
			packet.clear();
		}
		else
		{
			std::cout << "El paquete no se ha podido enviar\n";
		}

	}
}

void Client::CheckPlayersRdy()
{
	while (!this->gamestarded)
	{
		std::cout << "----------------------------------------------------------" << std::endl;
		int j = 0;

		if (this->rdy)
		{
			std::cout << "El jugador " << this->pl_socket->GetRemoteLocalPort() << " ESTA rdy" << std::endl;
			j++;
		}
		else
		{
			std::cout << "El jugador " << this->pl_socket->GetRemoteLocalPort() << " NO ESTA rdy aun" << std::endl;
		}

		for (auto const& i : pl_clients)
		{
			if (i->GetRdy())
			{
				std::cout << "El jugador " << i->GetRemoteLocalPort() << " ESTA rdy" << std::endl;
				j++;
			}
			else
			{
				std::cout << "El jugador " << i->GetRemoteLocalPort() << " NO ESTA rdy aun" << std::endl;
			}
		}

		if (j == 4)
		{
			std::cout << "----------------------------------------------------------" << std::endl;
			std::cout << "TODOS LOS JUGADORES ESTAN PREPARADOS PARA EMPEZAR LA PARTIDA" << std::endl;
			std::cout << "----------------------------------------------------------" << std::endl;

			gamestarded = true;

			//Send Respective turns to each player
			if (id == hands[0]->playerTurn)
			{
				sf::Packet packet;
				for (auto const& i : pl_clients)
				{
					//When packet its ready, send it to connected user
					packet << HEADER_MSG::MSG_TURN;
					pl_status->SetStatus(i->Send(packet));

					if (pl_status->GetStatus() == sf::Socket::Done)
					{
						std::cout << "El paquete se ha enviado correctamente\n";
						packet.clear();
					}
					else
					{
						std::cout << "El paquete no se ha podido enviar\n";
					}
				}
			}
			else
			{
				std::cout << "NO ES MI TURNO" << std::endl;
			}
		}

		std::cout << "----------------------------------------------------------" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_SLEEP));
	}
}

void Client::ExtractPlayer()
{
	int j = 0;
	std::cout << "Es tu turno, a quien le quieres pedir carta?" << std::endl;
	for (const auto& i : pl_clients)
	{
		std::cout << "Jugador " << j++ << " - " << i->GetRemotePort() << std::endl;
	}

	do
	{
		std::cout << "Escoge coger una carta al jugador (0-2): ";
		std::cin >> player;

		if (player > 2)
		{
			std::cout << "\nJugador no disponible" << std::endl;
		}
	} while (player > 2);

	std::cout << "--------------------------------------------------" << std::endl;
}

void Client::ExtractCulture()
{
	int int_culture = 0;

	do
	{
		std::cout << "Elige la cultura:  (0) - 'ARABE' | (1) - 'BANTU' | (2) - 'CHINA' | (3) - 'ESQUIMAL' | (4) - 'INDIA' | (5) - 'MEXICANA' | (6) - 'TIROLESA' " << std::endl;
		std::cout << "Cultura: ";

		std::cin >> int_culture;
		culture = (Culture)int_culture;

		if (culture > 6)
		{
			std::cout << "\nCultura no disponible" << std::endl;
		}
	} while (culture > 6);

	std::cout << "--------------------------------------------------" << std::endl;
}

void Client::ExtractFamily()
{
	int int_familia = 0;

	do
	{
		std::cout << "Elige la familia: (0) - 'ABUELO' | (1) - 'ABUELA' | (2) - 'PADRE' | (3) - 'MADRE' | (4) - 'HIJO' | (5) - 'HIJA' " << std::endl;
		std::cout << "Familia: ";

		std::cin >> int_familia;
		family = (Family)int_familia;

		if (int_familia > 5)
		{
			std::cout << "\nFamilia no disponible" << std::endl;
		}
	} while (int_familia > 5);

	std::cout << "--------------------------------------------------" << std::endl;
}

/// <summary>
/// Cast Functions

std::string Client::HeaderToString(HEADER_MSG header)
{
	switch (header)
	{
	case MSG_NULL:
		return "MSG_NULL";
	case MSG_OK:
		return "MSG_OK";
	case MSG_KO:
		return "MSG_KO";
	case MSG_PEERS:
		return "MSG_PEERS";
	case MSG_TURN:
		return "MSG_TURN";
	}
}

std::string Client::castSwitchToStringCulture(Culture culture)
{
	switch (culture)
	{
	case Culture::ARABE:
		return "ARABE";
	case Culture::BANTU:
		return "BANTU";
	case Culture::CHINA:
		return "CHINA";
	case Culture::ESQUIMAL:
		return "ESQUIMAL";
	case Culture::INDIA:
		return "INDIA";
	case Culture::MEXICANA:
		return "MEXICANA";
	case Culture::TIROLESA:
		return "TIROLESA";
	}
}

std::string Client::castSwitchToStringType(Family types)
{
	switch (types)
	{
	case Family::ABUELO:
		return "ABUELO";
	case Family::ABUELA:
		return "ABUELA";
	case Family::PADRE:
		return "PADRE";
	case Family::MADRE:
		return "MADRE";
	case Family::HIJO:
		return "HIJO";
	case Family::HIJA:
		return "HIJA";
	}
}

int Client::castStringToIntCulture(std::string culture) {

	if (culture == "ARABE") {
		return 0;
	}
	else if (culture == "BANTU") {
		return 1;
	}
	else if (culture == "CHINA") {
		return 2;
	}
	else if (culture == "ESQUIMAL") {
		return 3;
	}
	else if (culture == "INDIA") {
		return 4;
	}
	else if (culture == "MEXICANA") {
		return 5;
	}
	else if (culture == "TIROLESA") {
		return 6;
	}
}

int Client::castStringToIntType(std::string types) {

	if (types == "ABUELO") {
		return 0;
	}
	else if (types == "ABUELA") {
		return 1;
	}
	else if (types == "PADRE") {
		return 2;
	}
	else if (types == "MADRE") {
		return 3;
	}
	else if (types == "HIJO") {
		return 4;
	}
	else if (types == "HIJA") {
		return 5;
	}
}







