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
		if (sv_socketselector.Wait())
		{
			// Test the listener
			if (sv_socketselector.isReady(&sv_listener.GetListener()))
			{
				// The listener is ready: there is a pending connection
				sv_socket = new TcpSocketClass();

				if (sv_listener.Accept(sv_socket->GetSocket()) == sf::Socket::Done)
				{
					std::cout << "Connexion recibia del cliente: " << sv_socket->GetRemotePort() << std::endl;

					//Le enviamos la informacion a los jugadores
					SendPackets(*sv_socket->GetSocket());

					// Add the new client to the selector so that we will
					// be notified when he sends something
					sv_socketselector.Add(sv_socket->GetSocket());

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
	status.SetStatus(sv_listener.Listen(DEFAULT_PORT,sf::IpAddress::LocalHost));

	if (status.GetStatus() != sf::Socket::Done)
	{
		std::cout << "Error al abrir listener\n";
		DisconnectServer();
	}
	else 
	{
		running = true;

		//Add listener to selector
		sv_socketselector.Add(&sv_listener.GetListener());
	}
}

void Server::DisconnectServer()
{
	//Info
	std::cout << "Servidor desconectado, cerrando el servidor... " << std::endl;

	//Free memory
	delete[] sv_socket;

	//Stop Gameloop
	running = false;
}

void Server::SendPackets(sf::TcpSocket& socket)
{
	//Construir el paquete sin enviarme mis datos a mi mismo ya que ya los tengo, y le envio la informacion de los otros
	sf::Packet packet;

	std::string numberPlayers = std::to_string(clients.size());
	packet << numberPlayers;

	std::string str_port;

	for (auto const& i : clients) {
		unsigned short port = i->GetRemotePort();
		str_port = std::to_string(port);
		packet << str_port;
	}

	//When packet its ready, send it to connected user
	status.SetStatus(socket.send(packet));

	if (status.GetStatus() == sf::Socket::Done) 
	{
		std::cout << "El paquete se ha enviado correctamente\n";
		packet.clear();
	}
	else {
		std::cout << "El paquete no se ha podido enviar\n";
	}
}
