#pragma once
#include <vector>
#include <list>
#include <algorithm>
#include <random>
#include <map>
#include "iostream"
#include "Cards.h"

struct Deck {

	std::vector<Card*> deck;

	Deck() {

		//En el constructor reservamos memoria para toda la baraja y posteriormente 
		//añadimos todas las cartas de la baraja

		deck.reserve(MAX_CULTURE * MAX_TYPE);
		for (size_t i = 0; i < MAX_CULTURE; i++)
		{
			for (size_t j = 0; j < MAX_TYPE; j++)
			{
				deck.push_back(new Card((Card::Culture)i, (Card::Types)j));
			}
		}
	}

	void ShuffleDeck(int _seed) {
		//Funcion que randomiza un vector
		shuffle(deck.begin(), deck.end(),std::default_random_engine(_seed));
	}
	void PrintActualDeck() {
		std::cout << "Actual deck:" << std::endl;
		for (size_t i = 0; i < deck.size(); i++)
		{
			std::cout << Card::ToString(deck.at(i)->culture)<<"  "<< Card::ToString(deck.at(i)->type) <<"  "<< deck.at(i)->id << std::endl;
		}
		
	}
};