#include "Client.h"

Client::Client()
{
	//Create memory for Pointers
	pl_socket = new TcpSocketClass();
	pl_listener = new TcpListenerClass();
	pl_socketSelector = new TcpSocketSelectorClass();
}

void Client::Run()
{
	ConnectToBBS();
	OpenListener();
	Wait4ServerPacket();
	ShowCurrentPlayers();
	ListenToPlayers();

	std::thread socketselectorListener(&Client::SocketSelectorListener, this);
	socketselectorListener.detach();

	while (gameloop)
	{

	}
}

void Client::ConnectToBBS()
{
	//Nos conectamos al BBS por el puerto 50000
	std::cout << "\nPlayer inicializado, vas a connectarte al servidor por el puerto:" << DEFAULT_PORT << std::endl;
	pl_status = pl_socket->Connect(LOCAL_HOST, DEFAULT_PORT, sf::milliseconds(15.f));

	if (pl_status == sf::Socket::Done) 
	{
		std::cout << "Connectado al servidor correctamente... \n";
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
			std::cout << "El programa se cerrará en 10s \n";
			Sleep(microsecond);
		}

	}

}

void Client::OpenListener()
{
	//Abrimos el listener y lo añadimos al socket selector para posteriormente recibir los paquetes

	std::cout << "Me dispongo a escuchar por el puerto " << pl_socket->GetRemoteLocalPort() << std::endl;

	pl_status = pl_listener->Listen(pl_socket->GetRemoteLocalPort(), sf::IpAddress::LocalHost);

	if (pl_status == sf::Socket::Done)
	{
		std::cout << "Listener abierto correctamente\n";
		pl_socketSelector->Add(&pl_listener->GetListener());
	}
	else {
		std::cout << "Error al abrir listener del player\n";
		std::string rentry;
		std::cout << "Quieres volver a intentarlo? (y = si) \n";
		std::cin >> rentry;
		if (rentry == "y") {
			OpenListener();
		}
		else {
			std::cout << "El programa se cerrará en 10s \n";
			Sleep(microsecond);
			exit(0);
		}

	}
}

void Client::Wait4ServerPacket()
{
	sf::Packet packet;
	std::string str_port;

	// esperamos a recibir el packet

	pl_status = pl_socket->Recieve(packet);

	if (pl_status == sf::Socket::Done) 
	{


	}
	else {
		std::cout << "Nadie me envia nada" << std::endl;
	}
	packet.clear();

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
}


void Client::SocketSelectorListener()
{
	//Aqui sera donde tendrán que ir los headers !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	while (true)
	{
		for (TcpSocketClass* client : pl_clients)
		{
			if (pl_socketSelector->Wait())
			{
				if (pl_socketSelector->isReady(client->GetSocket()))
				{
					// The client has sent some data, we can receive it
					sf::Packet packet;
					pl_status = client->Recieve(packet);
					if (pl_status == sf::Socket::Done)
					{
						std::string strRec;
						packet >> strRec;

						std::cout << "\nHe recibido " << strRec << " del puerto " << client->GetRemotePort() << std::endl;

						if (std::stoi(strRec) == 4) {

							std::cout << "\nHe recibido un paquete especial, el del orden de la partida" << std::endl;
						}

						packet.clear();
					}
					else if (pl_status == sf::Socket::Disconnected)
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




