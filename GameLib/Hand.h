#pragma once
#include "Cards.h"
#include <map>

struct Hand {

	int points = 0;
	int numCards = 0;
	bool inGame;
	int playerTurn;
	//Cuantas cartas de cada categoria tenemos

	//Tenemos actualmente la carta en la mano o no 
	std::vector<Card> hand; // SINO CAMBIAR POR VECTOR ES LO MAS VIBLE SI NO FUNCIONA 


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
					std::cout << "Cultura " << Card::ToString(i.culture) <<" completada " << std::endl;
					return true;
				}

			}

		}
	
		return false;
	}


	void removeCard(Card _card) {
		
		
		for (size_t i = 0; i < hand.size(); i++) {

			if (hand[i].culture == _card.culture) {
			
				if (hand[i].type  == _card.type) {	
				
					std::vector<Card>::iterator it = hand.begin() + i;
				
					hand.erase(it);
				}
				

			}
		}
		numCards--;
		if (numCards <= 0)
		{
			inGame = false;
		}
		
	}


	void addCard(Card _card) {

		hand.push_back(_card);
		//hand[_card] = true;
		numCards++;
		
		//comprobamos si tenemos todos los componentes de una cultura
		if (cultureCompleted(_card.culture))
		{
			points++;
			int aux = MAX_TYPE;
			while (aux != 0) {

				for (auto i : hand) {

					if (i.culture == _card.culture) {
						removeCard(i);
						aux--;
						break;
					}

				}
			}

		}
		
	}
};