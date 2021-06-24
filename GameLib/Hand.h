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
					std::cout << "Cultura completada" << std::endl;
					return true;
				}

			}

		}
	
		return false;
	}


	void removeCard(Card _card) {
		
		
		for (size_t i = 0; i < hand.size(); i++) {

			if (hand[i].culture == _card.culture) {
				std::cout << "La cultura es la misma\n";
				if (hand[i].type  == _card.type) {	
					std::cout << "El tipo es el\n";
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
		std::cout << "///////////////////////BORRANDO////////////////////////////"<< std::endl;
	}

	void removeCard(Card::Culture _cult) {
		int cont = 0;
			std::cout << "/////////////////////// Hands " << hand.size() << " Hands ////////////////////////////" << std::endl;
		for (size_t i = 0; i < hand.size(); i++) {

			if (hand[i].culture == _cult) {
			
					std::vector<Card>::iterator it = hand.begin() + i;
					
					hand.erase(it);
					cont++;
					numCards--;
			}
		}

		if (numCards <= 0)
		{
			inGame = false;
		}
		std::cout << "///////////////////////  "<<cont<<"  ////////////////////////////" << std::endl;
	}

	void addCard(Card _card) {

		hand.push_back(_card);
		//hand[_card] = true;
		numCards++;
		
		//comprobamos si tenemos todos los componentes de una cultura
		if (cultureCompleted(_card.culture))
		{
			//Se suman los puntos y borramos estas cartas de la mano
			points++;
			PrintHand();
			removeCard(_card.culture);

		}
		PrintHand();
	}
};