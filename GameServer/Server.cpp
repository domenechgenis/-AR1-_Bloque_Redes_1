#include "Server.h"

Server::Server()
{
	std::cout << "Servidor inicializado, vas a escuchar por el puerto:" << DEFAULT_PORT << std::endl;
}

void Server::Run()
{
	//Open Listener to the default port
	OpenListener();

	while (running)
	{
		// Make the selector wait for data on any socket
		if (sv_socketselector.wait())
		{
			// Test the listener
			if (sv_socketselector.isReady(sv_listener))
			{
				// The listener is ready: there is a pending connection
				sv_socket = new sf::TcpSocket();

				if (sv_listener.accept(*sv_socket) == sf::Socket::Done)
				{
					std::cout << "Connexion recibia del cliente: " << sv_socket->getRemotePort() << std::endl;

					//Le enviamos la informacion a los jugadores
					SendPackets(*sv_socket);

					// Add the new client to the selector so that we will
					// be notified when he sends something
					sv_socketselector.add(*sv_socket);

					//Add to client server
					clients.push_back(sv_socket);
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
	sv_status = sv_listener.listen(DEFAULT_PORT);

	if (sv_status != sf::Socket::Done)
	{
		std::cout << "Error al abrir listener\n";
		DisconnectServer();
	}
	else {

		running = true;

		//Add listener to selector
		sv_socketselector.add(sv_listener);
	}
}

void Server::DisconnectServer()
{
	//Info
	std::cout << "Servidor desconectado, cerrando el servidor... " << std::endl;

	//Free memory
	delete[] sv_socket;

	running = false;
}

void Server::SendPackets(sf::TcpSocket&)
{
}