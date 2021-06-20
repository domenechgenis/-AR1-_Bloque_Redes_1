#include "Server.h"

Server::Server()
{
	//Create memory for Pointers
	sv_socket = new TcpSocketClass();
	sv_listener = new TcpListenerClass();
	sv_socketselector = new TcpSocketSelectorClass();
	sv_status = new TcpStatusClass();
}

Server::~Server()
{
	delete[] sv_socket;
	delete[] sv_listener;
	delete[] sv_socketselector;
	delete[] sv_status;
}

void Server::Run()
{
	std::cout << "Servidor inicializado, vas a escuchar por el puerto:" << DEFAULT_PORT << std::endl;

	//Open Listener to the default port
	OpenListener();

	while (running)
	{
		// Make the selector wait for data on any socket
		if (sv_socketselector->Wait())
		{
			// Test the listener
			if (sv_socketselector->isReady(&sv_listener->GetListener()))
			{
				// The listener is ready: there is a pending connection
				sv_socket = new TcpSocketClass();

				if (sv_listener->Accept(sv_socket->GetSocket()) == sf::Socket::Done)
				{
					std::cout << "Connexion recibia del cliente: " << sv_socket->GetRemotePort() << std::endl;

					//Le enviamos la informacion a los jugadores
					SendPackets(*sv_socket->GetSocket());
					// Add the new client to the selector so that we will
					// be notified when he sends something
					sv_socketselector->Add(sv_socket->GetSocket());

					//Add to client server
					sv_clients.push_back(sv_socket);

					
				}
				else {
					delete sv_socket;
					std::cout << "Error al recibir un player\n";
					DisconnectServer();
				}
			}
		}
	}
}

void Server::OpenListener()
{
	sv_status->SetStatus(sv_listener->Listen(DEFAULT_PORT,sf::IpAddress::LocalHost));

	if (sv_status->GetStatus() != sf::Socket::Done)
	{
		std::cout << "El servidor no se ha podido vincular al servidor\n";
		DisconnectServer();
	}
	else 
	{
		std::cout << "El servidor se ha vinculado correctamente al puerto: " << DEFAULT_PORT << std::endl;
		running = true;

		//Add listener to selector
		sv_socketselector->Add(&sv_listener->GetListener());
	}
}

void Server::DisconnectServer()
{
	//Info
	std::cout << "Servidor desconectado, cerrando el servidor... " << std::endl;

	//Disconnect sockets
	for (auto const& i : sv_clients) 
	{
		i->Disconnect();
	}
	this->sv_clients.clear();

	//Stop Gameloop
	running = false;
}

void Server::SendPackets(sf::TcpSocket& socket)
{
	//Construir el paquete sin enviarme mis datos a mi mismo ya que ya los tengo, y le envio la informacion de los otros
	sf::Packet packet;

	std::string numberPlayers = std::to_string(sv_clients.size());
	packet << numberPlayers;

	std::string str_port;

	for (auto const& i : sv_clients) {
		unsigned short port = i->GetRemotePort();
		str_port = std::to_string(port);
		packet << str_port;
	}

	//When packet its ready, send it to connected user
	sv_status->SetStatus(socket.send(packet));

	if (sv_status->GetStatus() == sf::Socket::Done) 
	{
		std::cout << "El paquete se ha enviado correctamente\n";
		packet.clear();
	}
	else {
		std::cout << "El paquete no se ha podido enviar\n";
	}
}
