#pragma once
#include "Cards.h"
#include <map>

struct Hand {

	int points = 0;
	int numCards = 0;
	bool inGame;
	int playerTurn;
	//Cuantas cartas de cada categoria tenemos
	std::map<int, int> categoriesAmountCards;
	//Tenemos actualmente la carta en la mano o no 
	std::map<Card, bool> hand;


	Hand() {
		for (int i = 0; i < MAX_CULTURE; i++)
		{
			categoriesAmountCards[i] = 0;
		}
	}

	//Comprobamos si 
	bool hasCard(Card _card) {
		return hand[_card];
	}

	bool cultureCompleted(Card::Culture _cult) {

		if (categoriesAmountCards[(int)_cult] >= MAX_CULTURE) {
			return true;
		}
		return false;
	}
	void removeCard(Card _card) {
		//Desactivamos la carta de la mano 
		categoriesAmountCards[(int)_card.culture]--;
		hand[_card] = false;
		numCards--;
		if (numCards <= 0)
		{
			inGame = false;
		}
	}

	void addCard(Card _card) {

		categoriesAmountCards[(int)_card.culture]++;
		hand[_card] = true;
		numCards++;

		//comprobamos si tenemos todos los componentes de una cultura
		if (cultureCompleted(_card.culture))
		{
			//Se suman los puntos y borramos estas cartas de la mano
			points++;
			for (std::map<Card, bool>::iterator it = hand.begin(); it != hand.end(); ++it) {
				if (it->second) {
					if (it->first.culture == _card.culture) {
						removeCard(it->first);
					}
				}
			}
		}
	}
};