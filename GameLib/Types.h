#pragma once

struct PlayerInfo {
	TcpSocketClass socket;
	bool isRdy;
};

enum HEADER_MSG
{
	MSG_NULL,
	MSG_OK,
	MSG_KO,
	MSG_PEERS,
	MSG_RDY,
	MSG_TURN,
};

enum Culture 
{ 
	ARABE, 
	BANTU, 
	CHINA, 
	ESQUIMAL, 
	INDIA, 
	MEXICANA, 
	TIROLESA 
};

enum Family 
{ 
	ABUELO,
	ABUELA, 
	PADRE,
	MADRE, 
	HIJO, 
	HIJA 
};


