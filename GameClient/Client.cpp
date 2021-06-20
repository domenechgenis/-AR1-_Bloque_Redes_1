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
	if (!ConnectToBBS() || !OpenListener()) {
		std::cout << "El cliente no ha podido conectarse a ningun servidor, cerrando el programa... \n";
		Sleep(microsecond);
		return;
	}

	//Wait4ServerPackets
	Wait4ServerPacket();

	//Listener thread
	std::thread socketselectorListener(&Client::SocketSelectorListener, this);
	socketselectorListener.detach();

	ShowCurrentPlayers();

	ListenToPlayers();

	Deck deck;

	deck = Deck();
	

	//deck.ShuffleDeck();
	while (gameloop)
	{
		//Match Start
		system("cls");
		std::cout << "TODOS LOS JUGADORES HAN SIDO CONECTADOS";

		//


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

	for (auto const& i : pl_clients) {
		std::cout << "Cliente: " << i + 1;
		std::cout << " Puerto ---> " << i->GetRemotePort() << std::endl;
	}

}

void Client::ListenToPlayers()
{
	//Mientras no haya 3 jugadores el jugador espera escuchando alguna conexion de los otros players
	while (pl_clients.size() < 3) {
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
					Sleep(microsecond);
					exit(0);
				}
			}
		}
		else {

			std::cout << "Error al abrir listener\n";
			Sleep(microsecond);
			exit(0);
		}
	}


	gameloop = true;
}

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
						std::string strRec;
						packet >> strRec;
						std::cout << "\nHe recibido " << strRec << " del puerto " << client->GetRemotePort() << std::endl;
						packet.clear();
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

void Client::Wait4ServerPacket()
{
	sf::Packet packet;
	std::string str_port;
	std::string numberPlayers;
	int nPlayers;

	pl_status->SetStatus(pl_socket->Recieve(packet));

	if (pl_status->GetStatus() == sf::Socket::Done) {

		packet >> numberPlayers;
		nPlayers = std::stoi(numberPlayers);

		//Recibo cuantos players hay antes que yo
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
				pl_clients.push_back(player);
				pl_socketSelector->Add(player->GetSocket());

			}
			else {
				std::cout << "Error al connectar el jugador a la partida";
				delete player;
				Sleep(microsecond);
				exit(0);

			}
		}

	}
	else {
		std::cout << "Nadie me envia nada" << std::endl;
	}
	packet.clear();

}





