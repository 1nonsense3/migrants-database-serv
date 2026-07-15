#pragma once
//#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>

using namespace std;


struct migrant
{
	int ID;
	string FIO;
	string country;
	time_t entry;
	time_t departure;
	time_t birthday;

	migrant(int id = 0, string fio = ".", time_t Birthday = 0, string Country = ".", time_t Entry = 0, time_t Departure = 9999999999999) { ID = id; FIO = fio; birthday = Birthday; country = Country; entry = Entry, departure = Departure; }
	friend bool operator<=(const migrant a, const migrant b)
	{
		if (a.birthday <= b.birthday) { return true; }
		return false;
	}

	friend bool operator<(const migrant a, const migrant b)
	{
		if (a.entry <= b.entry) { return true; }
		return false;
	}
	friend bool operator==(const migrant b, const migrant a)
	{
		if (b.FIO != ".") 
		{
			if (b.country == ".") 
			{
				if (a.FIO.find(b.FIO) != std::string::npos)
				{
					return true;
				}
				return false;
			}
			if (a.country == b.country and (a.FIO.find(b.FIO) != std::string::npos)) {
				return true;
			}
			return false;
		}
		else if (b.FIO == ".") 
		{
			if (a.country == b.country) {
				return true;
			}
			return false;
		}
		return false;
	}

	bool is_in() { if (departure == 0) { return true; } return false; }
	bool is_empty() { if (ID == 0 and FIO == ". " and country == "." and entry == 0 and departure == 9999999999999 and birthday == 0) { return true; } return false; }
};
