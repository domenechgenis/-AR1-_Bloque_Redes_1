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
	std::vector<Card> hand; // SINO CAMBIAR POR VECTOR ES LO MAS VIBLE SI NO FUNCIONA 


	Hand() {
		for (int i = 0; i < MAX_CULTURE; i++)
		{
			categoriesAmountCards[i] = 0;
		}	
	}

	//Comprobamos si 
	
	bool hasCard(Card _card) {
		for (auto i : hand) {

			if (i == _card) {
				return true;
			}
		}
		return false;
	}
	void PrintHand() {
		int cont=0;
		for (auto i : hand) {
			
			std::cout << Card::ToString(i.culture)<<"------"<< Card::ToString(i.type)<< std::endl;
			cont++;
		}

	}


	bool cultureCompleted(Card::Culture _cult) {

		int auxCount = 0;
		for (auto i : hand) {

			if (i.culture == _cult) {
				auxCount++;
				if (auxCount == MAX_TYPE) {
					std::cout << "Cultura completada" << std::endl;
					return true;
				}

			}

		}
	
		return false;
	}


	void removeCard(Card _card) {
		//Desactivamos la carta de la mano 

		categoriesAmountCards[(int)_card.culture]--;
		for (size_t i = 0; i < hand.size(); i++) {

			if (hand[i].culture == _card.culture) {
				std::cout << "La cultura es la misma\n";
				if (hand[i].type  == _card.type) {	
					std::cout << "El tipo es el\n";
					std::vector<Card>::iterator it = hand.begin() + i;
					//std::cout << "CARTA BORRADA:" << hand.erase(it) << "\n\n"; ;
					hand.erase(it);
				}
				

			}
		}
		numCards--;
		if (numCards <= 0)
		{
			inGame = false;
		}
		std::cout << "///////////////////////BORRACION////////////////////////////"<< std::endl;
	}

	void addCard(Card _card) {

		
		hand.push_back(_card);
	
		categoriesAmountCards[(int)_card.culture]++;
		//hand[_card] = true;
		numCards++;
		
		//comprobamos si tenemos todos los componentes de una cultura
		if (cultureCompleted(_card.culture))
		{
			
			//Se suman los puntos y borramos estas cartas de la mano
			points++;
			for (auto i : hand) {

				if (i.culture == _card.culture) {
					PrintHand();
					removeCard(_card);
					PrintHand();
				}
			}
			
		}
	}
};