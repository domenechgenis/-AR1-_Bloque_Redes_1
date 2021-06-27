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
	delete[] timer;
}

void Client::Run()
{
	//Connect player to Port 50.000 and Open Listener
	if (!ConnectToBBS() || !OpenListener()) 
	{
		std::cout << "El cliente no ha podido conectarse a ningun servidor, cerrando el programa... \n";
		Sleep(DEFAULT_SLEEP);
		return;
	}

	//Create Room or Join it
	CreateOrJoinRoom();

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

	//Turn timer check
	std::thread turnTimerCheck(&Client::TurnTimerChecker, this);
	turnTimerCheck.detach();
	

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
					player->SetId(pl_clients.size()+1);
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
	case MSG_PASSTURN:
		HandlePassTurnReciever(packet, client);
		break;
	case MSG_CHEAT:
		CheckGameHands(packet, client);
		break;
	case MSG_CHAT:
		ReceiveChat(packet, client);
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
	int id_recieved, player_recieved, family_recieved, culture_recieved;

	packet >> id_recieved;

	packet >> player_recieved;

	packet >> culture_recieved;
	
	packet >> family_recieved;

	
	//Entra aqui por la cara por esto esta estwe "fix" hecho

	if (id_recieved >= 0 && id_recieved <= 3 ) {

		UpdateHandsTakeCardFromPlayer(id_recieved, player_recieved, (Card::Culture)culture_recieved, (Card::Types)family_recieved);

	}


}
void Client::HandlePassTurnReciever(sf::Packet& packet, TcpSocketClass* client)
{
	int id_recieved;

	packet >> id_recieved;


	//Entra aqui por la cara por esto esta estwe "fix" hecho

	if (id_recieved >= 0 && id_recieved <= 3) {

		PasarTurno(id_recieved);

	}


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

			//Iniciamos el chat
			std::thread chat(&Client::ChatSoloUno, this);
			chat.detach();


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
		if (hands[id]->inGame) {
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

			//Comprobamos si ha acertado con la carta
			if (CheckCard(player, (Card::Culture)culture, (Card::Types)family)) {

				UpdateHandsTakeCardFromPlayer(id, player, (Card::Culture)culture, (Card::Types)family);

				//Enviamos al resto de peers la informacion de las cartas
				HandlePlayerGoodDecision();

				std::cout << "Has acertado la carta por lo que puedes coger otra" << std::endl;

				Sleep(2000);
				HandlePlayerTurn();
				FinishGame();
			}
			else {

				std::cout << "Has fallado , pierdes el turno" << std::endl;
				

				//enviar paquete de cambio de turno
				HandlePlayerBadDecision();
				//Actualizamos en local el turno 
				PasarTurno(id);
				FinishGame();
				Sleep(5000);

			}

		}
		else {
			HandlePlayerBadDecision();
			PasarTurno(id);
			Sleep(5000);
		}
		
	}
	else
	{

		if (!hands[id]->inGame) {

			system("cls");
			std::cout << "Soy el jugador: " << id <<"  y estoy en modo espectador\n" <<std::endl;

			std::cout << "Puntuaciones: \n" << std::endl;
			for (auto i : hands) {
				std::cout << "Jugador" << i.first << ":" << i.second->points << std::endl;
			}
		}
		else {
			system("cls");
			std::cout << "Soy el jugador: " << id << std::endl;
			std::cout << "Esta es tu mano actual:" << std::endl;
			std::cout << "--------------------------------------------------" << std::endl;

			hands[id]->PrintHand();
			std::cout << "NO es tu turno, esperando a que te toque\n" << std::endl;
			std::cout << "Puntuaciones: \n" << std::endl;
			for (auto i : hands) {
				std::cout << "Jugador" << i.first << ":" << i.second->points << std::endl;
			}
		}
		

		Sleep(2000);
	}
}

void Client::CreateOrJoinRoom()
{
	int msg;
	do 
	{
		std::cout << "-------------------------------------------------------------------------------" << std::endl;
		std::cout << "Quieres crear una partida - 0, o unirte a una partida - 1" << std::endl;
		std::cout << "Tecla: ";
		std::cin >> msg;

		if(msg == 0) // Create Room
		{
			CreateRoom();
		}
		else if(msg == 1) // Join Room
		{
			JoinRoom();
		}
		else if (msg == 2) // Error
		{
			std::cout << "Tecla no valida" << std::endl;
		}
	} while (msg > 1);
}

void Client::CreateRoom()
{
	std::string room, password;
	int npassword;

	std::cout << "-------------------------------------------------------------------------------" << std::endl;

	std::cout << "Introduce el nombre de la sala:" << std::endl;
	std::cout << "Nombre: ";
	std::cin >> room;

	do 
	{
		std::cout << "Quieres introducir contraseña a la partida 0 - Si , 1 - No" << std::endl;
		std::cout << "Contraseña?: ";
		std::cin >> npassword;

		if(npassword == 0) // User want password
		{
			std::cout << "Introduce la contraseña de la partida: " << std::endl;
			std::cout << "Contraseña: ";
			std::cin >> password;
		}
		else if (npassword > 1) // Invalid Key
		{
			std::cout << "Tecla no valida" << std::endl;
		}

	} while (npassword > 1);
}

void Client::JoinRoom()
{
	std::cout << "-------------------------------------------------------------------------------" << std::endl;
	std::cout << "Salas Disponibles: " << std::endl;
}

void Client::CheckGameHands(sf::Packet& packet, TcpSocketClass* client)
{
	//Comprobar que todas las manos estén igual 
	int aux=0;
	int _id_receive, numcards,card_id;
	

	packet >> _id_receive;
	packet >> numcards;

	if (numcards != hands[_id_receive]->numCards) {
		//Eres un chiter
	}
	else {

		for (size_t i = 0; i < numcards; i++) {
			packet >> card_id;
			for (size_t j = 0; j < numcards; j++) {

				if (card_id == hands[_id_receive]->hand[j].id) {
					aux++;
					break;
				}

			}

		}

		if (aux==numcards) {

			std::cout<<"EL JUGADOR "<<_id_receive << " TIENE TODO OK \n";

		}
		else {

			std::cout <<"EL JUGADOR "<< _id_receive << "...ERES UN CHEATER \n";
		}

	}



}

void Client::SendMyHand()
{
	sf::Packet packet;

	for (auto const& i : pl_clients)
	{
		packet << HEADER_MSG::MSG_CHEAT << id<< hands[id]->numCards;

	

		for (size_t i = 0;i < hands[id]->numCards;i++) {
			packet << hands[id]->hand[i].id;
		}
		//Se actualiza hands en el jugador actual;

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

void Client::HandlePlayerGoodDecision() 
{


	sf::Packet packet;

	for (auto const& i : pl_clients) 
	{
		packet << HEADER_MSG::MSG_TURN << id << player << culture << family;

		//Se actualiza hands en el jugador actual;
	
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
void Client::HandlePlayerBadDecision()
{

	sf::Packet packet;

	for (auto const& i : pl_clients)
	{
		packet << HEADER_MSG::MSG_PASSTURN << id ;

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
		system("cls");
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

void Client::TurnTimerChecker()
{
	while (gameloop)
	{
		std::cout << "Mi turno esta durando " << timer->GetDuration() << "/" << LOSE_TURN_TIME << std::endl;
		if (timer->GetDuration() > LOSE_TURN_TIME)
		{
			std::cout << "Has perdido el turno por tardar demasiado" << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}


void Client::ExtractPlayer()
{
	int j = 0;
	std::cout << "Es tu turno, a quien le quieres pedir carta?" << std::endl;
	for (const auto & i : pl_clients)
	{
		if(hands[i->GetId()]->inGame)
		std::cout << "Jugador " <<i->GetId() << " - " << i->GetRemotePort() << std::endl;
	}

	//do
	//{
		std::cout << "A que jugador de los anteriores quieres coger una carta?  ";
	
		std::cin >> player;

		//Esto cambiarlo sino la logica no funciona
		/*if (player > 2 || player <0)
		{
			std::cout << "\nJugador no disponible" << std::endl;
		}
		*/

	//} while (player > 2 || player < 0);

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

void Client::UpdateHandsTakeCardFromPlayer(int _id, int _player, Card::Culture _culture, Card::Types _type)
{
	Card *_newCard = new Card(_culture, _type);

	//Añadir std cout

	hands[_id]->addCard(*_newCard);


	hands[_player]->removeCard(*_newCard);
	
	
}

bool Client::CheckCard(int _playerID, Card::Culture _culture, Card::Types _type)
{
	Card* checkcard = new Card(_culture, _type);
	return hands[_playerID]->hasCard(*checkcard);

}

void Client::PasarTurno(int _id)
{
	for (auto i : hands ) {

		

		if (_id == 3) {

			i.second->playerTurn = 0;
			//Al acabar la ronda se envia la mano al resto de jugadores para comprobar que no se está cheateando
			SendMyHand();
		}
		else {
			i.second->playerTurn = _id + 1;
		}
		

	}

}

void Client::PasarTurnoLocal(int _id)
{
	for (auto i : hands) {



		if (_id == 3) {

			i.second->playerTurn = 0;

		}
		else {
			i.second->playerTurn = _id + 1;
		}


	}
}

void Client::FinishGame() {

	//Comprobamos cuantos jugadores hay inGame y los puntos de los jugadores

	int auxIngame=0;
	int sumFamCompleted = 0;

	for (auto i : hands) {
		if (i.second->inGame) {
			auxIngame++;
		}
		sumFamCompleted += i.second->points;
	}

	if (auxIngame<=2){

		std::cout << "\nSe ha terminado la partida porque solo quedan 2 jugadores\n" << std::endl;

		std::cout << "\nEl ganador es el jugador : " <<Winner()<< std::endl;
		Sleep(100000);

	}
	else if (sumFamCompleted == MAX_CULTURE){
		std::cout << "\nSe ha terminado la partida porque todas las familias se han completado\n" << std::endl;

		std::cout << "\nEl ganador es el jugador : " << Winner() << std::endl;

		Sleep(100000);
	}


}

int Client::Winner()
{
	int _id_winner;
	int points = 0;

	for (auto i : hands) {
		if (i.second->points > points) {
			points = i.second->points;
			_id_winner = i.first;
		}
	}
	return _id_winner;

}


void Client::ChatSoloUno()
{
	int mesageDest = 4;
	int mesageOption = 0;
	sf::Vector2i screenDimensions(300, 300);
	
	sf::RenderWindow window;
	
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Chat");

	sf::Font font;
	if (!font.loadFromFile("courbd.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	sf::String mensaje = SwitchChat(mesageDest, mesageOption);

	sf::Text chattingText(mensaje, font, 14);
	chattingText.setFillColor(sf::Color(0, 160, 0));
	chattingText.setStyle(sf::Text::Bold);


	sf::Text text(mensaje, font, 14);
	text.setFillColor(sf::Color(0, 160, 0));
	text.setStyle(sf::Text::Bold);
	text.setPosition(0, 270);

	sf::RectangleShape separator(sf::Vector2f(800, 5));
	separator.setFillColor(sf::Color(200, 200, 200, 255));
	separator.setPosition(0,250);

	aMensajes.push_back("Chat del Jugador: " + std::to_string(id)); 
	aMensajes.push_back("TAB->Cambiar Destinatario");
	aMensajes.push_back("LControl -> Cambiar Mensaje");
	aMensajes.push_back("Enter -> Enviar");
	aMensajes.push_back("---------------");

	while (window.isOpen())
	{
		sf::Event evento;
		while (window.pollEvent(evento))
		{
			switch (evento.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (evento.key.code == sf::Keyboard::Escape)
					window.close();
				else if (evento.key.code == sf::Keyboard::Return)
				{
					aMensajes.push_back("(local)" + mensaje);

					SendChat(mesageDest, mesageOption);

					if (aMensajes.size() > 25)
					{
						aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
					}
					mensaje = SwitchChat(mesageDest, mesageOption);
				}
				else if (evento.key.code == sf::Keyboard::Tab) {

					do {
						if (mesageDest == 4) {
							mesageDest = 0;
						}
						else {

							mesageDest++;
						}
					} while (mesageDest == id);

					mensaje = SwitchChat(mesageDest, mesageOption);
					
				}
				else if (evento.key.code == sf::Keyboard::LControl) {
					
						if (mesageOption == 2) {
							mesageOption = 0;
						}
						else {

							mesageOption++;
						}

						mensaje = SwitchChat(mesageDest, mesageOption);
				}
				break;
			case sf::Event::TextEntered:
				if (evento.text.unicode >= 32 && evento.text.unicode <= 126)
					mensaje += (char)evento.text.unicode;
				else if (evento.text.unicode == 8 && mensaje.getSize() > 0)
					mensaje.erase(mensaje.getSize() - 1, mensaje.getSize());
				break;
			}
		}
		window.draw(separator);
		for (size_t i = 0; i < aMensajes.size(); i++)
		{
			std::string chatting = aMensajes[i];
			chattingText.setPosition(sf::Vector2f(0, 20 * i));
			chattingText.setString(chatting);
			window.draw(chattingText);
		}
		std::string mensaje_ = mensaje ;
		text.setString(mensaje_);
		window.draw(text);


		window.display();
		window.clear();
	}



}

void Client::SendChat(int mDest, int option)
{

	sf::Packet packet;

	if(mDest == 4) {//Todos

		for (auto const& i : pl_clients)
		{
			packet << HEADER_MSG::MSG_CHAT << 4 << option;

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
	else {
		packet << HEADER_MSG::MSG_CHAT << id << option;

		for (auto const& i : pl_clients)
		{
			if (i->GetId() == mDest) {
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

				break;
			}
		}
	}


	

}

std::string Client::SwitchChat(int mesDes, int mesOpt)
{
	std::string mensaje;
	switch (mesDes)
	{
	case 0:
		switch (mesOpt)
		{
		case 0:
			mensaje = "> Jugador 0 : HOLA ";
			break;
		case 1:
			mensaje = "> Jugador 0 : BUENA JUGADA";
			break;
		case 2:
			mensaje = "> Jugador 0 : QUE SUERTE!";
			break;
		default:
			break;
		}


		break;
	case 1:
		switch (mesOpt)
		{
		case 0:
			mensaje = "> Jugador 1 : HOLA ";
			break;
		case 1:
			mensaje = "> Jugador 1 : BUENA JUGADA";
			break;
		case 2:
			mensaje = "> Jugador 1 : QUE SUERTE!";
			break;
		default:
			break;
		}
		break;
	case 2:
		switch (mesOpt)
		{
		case 0:
			mensaje = "> Jugador 2 : HOLA ";
			break;
		case 1:
			mensaje = "> Jugador 2 : BUENA JUGADA";
			break;
		case 2:
			mensaje = "> Jugador 2 : QUE SUERTE!";
			break;
		default:
			break;
		}
		break;
	case 3:
		switch (mesOpt)
		{
		case 0:
			mensaje = "> Jugador 3 : HOLA ";
			break;
		case 1:
			mensaje = "> Jugador 3 : BUENA JUGADA";
			break;
		case 2:
			mensaje = "> Jugador 3 : QUE SUERTE!";
			break;
		default:
			break;
		}
		break;

	case 4:
		switch (mesOpt)
		{
		case 0:
			mensaje = "> Todos : HOLA ";
			break;
		case 1:
			mensaje = "> Todos : BUENA JUGADA";
			break;
		case 2:
			mensaje = "> Todos : QUE SUERTE!";
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	
	return mensaje;
}
void Client::ReceiveChat(sf::Packet& packet, TcpSocketClass* client)
{

	int _id_receive, option;


	packet >> _id_receive;
	packet >> option;

	aMensajes.push_back(SwitchChat(_id_receive, option));

}







