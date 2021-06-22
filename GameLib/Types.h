#pragma once

struct PlayerInfo {
	TcpSocketClass socket;
	bool isRdy;
};

enum HEADER_MSG {
	MSG_NULL,
	MSG_OK,
	MSG_KO,
	MSG_PEERS,
	MSG_TURN,
};


