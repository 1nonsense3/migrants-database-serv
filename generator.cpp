#define _CRT_SECURE_NO_WARNINGS
#include "Migrant.h"
using namespace std;


void generateFIO(vector<migrant> & spisok)
{

	srand(time(0));

	ifstream FIObase;
	FIObase.open("FIOBase.txt");
	vector <string> name, surname, patronymic;
	string curr;
	while (getline(FIObase, curr))
	{
		if (surname.size() < 150) 
		{
			surname.push_back(curr);
		}
		else if (name.size() < 150)
		{
			name.push_back(curr);
		}
		else if (patronymic.size() < 150)
		{
			patronymic.push_back(curr);
		}
	}
	for (int i = 0; i < 100000; i++) 
	{
		migrant a;
		a.ID = i + 1;
		spisok.push_back(a);
		spisok[i].FIO = surname[rand() % surname.size()] + " " + name[rand() % name.size()] + " " + patronymic[rand() % patronymic.size()];
	}
}

void generateBirthday(vector<migrant>& spisok)
{
	int r = 0;
	time_t now = time(nullptr);
	for (int i = 0; i < 100000; i++) 
	{
		r = rand() % 5000;
		spisok[i].birthday = now - (8000 + r) * 24 * 60 * 60;
	}
}

void generateCountry(vector<migrant>& spisok) 
{
	ifstream Countrybase;
	Countrybase.open("CountryBase.txt");
	vector<string> countries;
	string curr;
	while (getline(Countrybase, curr))
	{
		countries.push_back(curr);
	}
	for (int i = 0; i < 100000; i++)
	{
		spisok[i].country = countries[rand() % countries.size()];
	}
	
}

void generateEntryAndDeparture(vector<migrant>& spisok) {
	for (int i = 0; i < 100000; i++) {
		int en = rand() % 5000;
		int dep = rand() % 720;
		spisok[i].entry = spisok[i].birthday + (1+en) * 24 * 60 * 60;
		if ((rand() % 30) % 3 == 0)
		{
			spisok[i].departure = 0;
		}
		else
		{
			spisok[i].departure = spisok[i].entry + dep * 24 * 60 * 60;
		}
	}
}


