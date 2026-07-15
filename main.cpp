#define _CRT_SECURE_NO_WARNINGS
#include "Migrant.h"
#include "Header.h"


int main()
{


	//SetConsoleCP(CP_UTF8);
	//SetConsoleOutputCP(CP_UTF8);
	vector<migrant> spisok;
	ifstream Base;
	ifstream find;
	find.open("Find.txt");
	Base.open("MigrantsBase.txt");
	readFile(Base, spisok);
	vector<migrant> forfind;
	//FindFromFile(find, spisok, forfind);
	migrant niz{}, verh{};
	//niz.country = "Казахстан";
	//niz.FIO = "Гусейн-Заде";
	/*
	cout << forfind.size() << endl;
	
	for (int i = 0; i < forfind.size(); i++)
	{
		cout << forfind[i].ID << " ";
		cout << forfind[i].FIO << " ";
		cout << " " << forfind[i].country << " " << forfind[i].birthday << " " << forfind[i].entry << endl;
	}
	cout << endl;
	*/
	openservHTTP(spisok, forfind);
}