#pragma once
#include "Constants.h"

struct Card {
	static enum class Culture { ARABE, BANTU, CHINA, ESQUIMAL, INDIA, MEXICANA, TIROLESA };
	static enum class Types { ABUELO, ABUELA, PADRE, MADRE, HIJO, HIJA };
	Culture culture;
	Types type;
	int id;

	static const char* ToString(Types t)
	{
		switch (t)
		{
		case Card::Types::ABUELO:
			return "ABUELO";
			break;
		case Card::Types::ABUELA:
			return "ABUELA";
			break;
		case Card::Types::PADRE:
			return "PADRE";
			break;
		case Card::Types::MADRE:
			return "MADRE";
			break;
		case Card::Types::HIJO:
			return "HIJO";
			break;
		case Card::Types::HIJA:
			return "HIJA";
			break;
		default:
			return "Unknown";
			break;
		}

	}
	static const char* ToString(Culture c)
	{

		switch (c)
		{
		case Card::Culture::ARABE:
			return "ARABE";
			break;
		case Card::Culture::BANTU:
			return "BANTU";
			break;
		case Card::Culture::CHINA:
			return "CHINA";
			break;
		case Card::Culture::ESQUIMAL:
			return "ESQUIMAL";
			break;
		case Card::Culture::INDIA:
			return "INDIA";
			break;
		case Card::Culture::MEXICANA:
			return "MEXICANA";
			break;
		case Card::Culture::TIROLESA:
			return "TIROLESA";
			break;
		default:
			return "Unknown";
			break;
		}
	}

	Card(Culture cul, Types typ) {

		culture = cul;
		type = typ;
		//Hacemos este algoritmo para que todas tengan un identificador distinto

		id = (int)culture * MAX_CULTURE + (int)type;
	};
};
