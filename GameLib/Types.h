#pragma once

struct Card {
	Culture culture;  //Arabe, Bant�, China,
	Types type; //El abuelo, La abuela, El padre...
};

struct PlayersInfo {
	std::string nickname;
	std::vector<Card> card;
};

