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
	void PrintHand() {
		int cont=0;
		for (auto i : hand) {
			
			std::cout << Card::ToString(i.first.culture)<<"------"<< Card::ToString(i.first.type)<< std::endl;
			cont++;
		}
		std::cout << "Tamaño de la mano :"<<hand.size()<<std::endl;

	}

	bool cultureCompleted(Card::Culture _cult) {

		int auxCount = 0;
		for (std::map<Card, bool>::iterator it = hand.begin(); it != hand.end(); ++it) {
			if (it->second) {
				if (it->first.culture == _cult) {
					auxCount++;
					if (auxCount == MAX_TYPE) {
						return true;
					}
				}
			}
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
		std::cout << "/////////////////////////////////////////////////////////////////////////"<< std::endl;
	}

	void addCard(Card _card) {

		hand.insert(hand.begin(),std::pair<Card, bool>(_card, true));
		std::cout << hand.size();
		categoriesAmountCards[(int)_card.culture]++;
		//hand[_card] = true;
		numCards++;
		std::cout << Card::ToString(_card.culture)<<"********" <<Card::ToString(_card.type)<< std::endl;
		
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