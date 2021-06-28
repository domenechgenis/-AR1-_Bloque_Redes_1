#pragma once

struct PlayerInfo
{
	TcpSocketClass socket;
	bool isRdy;
};

struct RoomInfo 
{
	std::string room,password;
	int npassowrd;
};

enum HEADER_MSG
{
	MSG_RDY,
	MSG_TURN,
	MSG_PASSTURN,
	MSG_CHEAT,
	MSG_CHAT,
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


