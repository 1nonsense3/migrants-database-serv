#define _CRT_SECURE_NO_WARNINGS
#include "Header.h"


void readFile(ifstream& file, vector<migrant>& spisok)
{
	string lol;
	int len = 0;
	while (getline(file, lol))
	{
		len++;
	}
	//cout << len;
	file.clear();
	file.seekg(0);
	for (int i = 0; i < len; i++)
	{
		migrant a;
		string name, surname, patronymic;
		string input;
		getline(file, input);
		istringstream iss(input);
		iss >> a.ID >> surname >> name >> patronymic >> a.birthday >> a.country >> a.entry >> a.departure;
		a.FIO = surname + " " + name + " " + patronymic;
		/*
		cout << a.ID << " ";
		cout << a.FIO << " ";
		cout << localtime(&a.birthday)->tm_mday << " " << localtime(&a.birthday)->tm_mon + 1 << " " << 1900 + localtime(&a.birthday)->tm_year;
		cout << " " << a.country << endl;
		if (int(a.departure) == 0) {
			cout << "Entry: " << localtime(&a.entry)->tm_mday << " " << localtime(&a.entry)->tm_mon + 1 << " " << 1900 + localtime(&a.entry)->tm_year; cout << " Still in country" << endl; cout << endl;
		}
		else {
			cout << "Entry: " << localtime(&a.entry)->tm_mday << " " << localtime(&a.entry)->tm_mon + 1 << " " << 1900 + localtime(&a.entry)->tm_year;
			cout << " Deportation: " << localtime(&a.departure)->tm_mday << " " << localtime(&a.departure)->tm_mon + 1 << " " << 1900 + localtime(&a.departure)->tm_year << endl; cout << endl;
		}
		*/
		spisok.push_back(a);
	}
}

void addPerson(vector<migrant>& spisok)
{

	migrant A;
	tm birthday{ 0 }, entry{ 0 }, departure{ 0 };

	cout << "Введите данные мигранта: " << endl;

	cout << "ФИО: ";
	getline(cin, A.FIO);
	cout << "Введите страну прибывающего: ";
	getline(cin, A.country);
	cout << "Введите дату рождения в формате день, месяц, год: ";
	cin >> birthday.tm_mday >> birthday.tm_mon >> birthday.tm_year;
	birthday.tm_year -= 1900;
	birthday.tm_mon -= 1;
	cin.ignore();
	cout << "Введите дату въезда в формате день, месяц, год: ";
	cin >> entry.tm_mday >> entry.tm_mon >> entry.tm_year;
	entry.tm_year -= 1900;
	entry.tm_mon -= 1;
	cin.ignore();
	cout << "Введите дату выезда в формате день, месяц, год: ";
	cin >> departure.tm_mday >> departure.tm_mon >> departure.tm_year;
	departure.tm_year -= 1900;
	departure.tm_mon -= 1;

	A.birthday = mktime(&birthday);
	A.entry = mktime(&entry);
	A.departure = mktime(&departure);
	spisok.push_back(A);

}


void Find(vector<migrant>& spisok, migrant niz, migrant verh, vector<migrant>& arr)
{
	if (verh.is_empty()) {
		//cout << niz.FIO << " " << niz.country << " " << niz.birthday << " " << niz.entry << " " << niz.departure << endl;
		//cout << verh.FIO << " " << verh.country << " " << verh.birthday << " " << verh.entry << " " << verh.departure << endl;
		if (niz.birthday == 0 and niz.entry == 0 and niz.departure == 9999999999999) { // поиск по стране, имени и т.д.
			//cout << niz.FIO << " " << niz.country << " " << niz.birthday << " " << niz.entry << " " << niz.departure << endl;
			for (int i = 0; i < spisok.size(); i++)
			{
				if (niz == spisok[i]) { arr.push_back(spisok[i]); }
			}

		}
		else if (niz.entry == 0 and niz.FIO != ".") // поиск по др
		{
			for (int i = 0; i < spisok.size(); i++)
			{
				if (niz <= spisok[i] and niz == spisok[i]) { arr.push_back(spisok[i]); }
			}
		}
		else //поиск по всему остальному
		{
			for (int i = 0; i < spisok.size(); i++)
			{
				if (niz <= spisok[i] and niz < spisok[i] and niz == spisok[i]) { arr.push_back(spisok[i]); }
			}
		}
	}
	else 
	{
		if (verh.entry == 0)
		{
			verh.entry = 9999999999999;
		}
		if (verh.birthday == 0) 
		{
			verh.birthday = 9999999999999;
		}

		if (niz.FIO != ".") {
			if (niz.birthday == 0 and niz.entry == 0 and niz.departure == 9999999999999 and verh.birthday == 0 and verh.entry == 9999999999999 and verh.departure == 9999999999999) { // поиск по стране, имени и т.д.
				for (int i = 0; i < spisok.size(); i++)
				{
					if (niz == spisok[i]) { arr.push_back(spisok[i]); }
				}

			}
			else if (niz.entry == 0 and niz.departure == 9999999999999 and verh.entry == 9999999999999 and verh.departure == 9999999999999) // поиск по др
			{
				for (int i = 0; i < spisok.size(); i++)
				{
					if (niz <= spisok[i] and spisok[i] <= verh and niz == spisok[i]) { arr.push_back(spisok[i]); }
				}
			}
			else if (niz.entry == 0 and niz.departure == 9999999999999 and verh.entry == 9999999999999)
			{
				for (int i = 0; i < spisok.size(); i++)
				{
					if (spisok[i].departure <= verh.departure and niz == spisok[i] and niz <= spisok[i] and spisok[i] <= verh) { arr.push_back(spisok[i]); }
				}
			}
			else if (niz.entry == 0 and niz.departure == 9999999999999)
			{
				for (int i = 0; i < spisok.size(); i++)
				{
					if (spisok[i].departure <= verh.departure and spisok[i].entry <= verh.entry and niz == spisok[i] and niz <= spisok[i] and spisok[i] <= verh) { arr.push_back(spisok[i]); }
				}
			}
			else //поиск по всему остальному
			{
				for (int i = 0; i < spisok.size(); i++)
				{
					if (niz <= spisok[i] and niz < spisok[i] and spisok[i] <= verh and spisok[i] < verh and (verh == spisok[i] or niz == spisok[i])) { arr.push_back(spisok[i]); }
				}
			}
		}
		else 
		{
			if (niz.birthday == 0 and niz.entry == 0 and niz.departure == 9999999999999 and verh.birthday == 0 and verh.entry == 9999999999999 and verh.departure == 9999999999999) { // поиск по стране, имени и т.д.
				for (int i = 0; i < spisok.size(); i++)
				{
					if (niz == spisok[i]) { arr.push_back(spisok[i]); }
				}

			}
			else if (niz.entry == 0 and niz.departure == 9999999999999 and verh.entry == 9999999999999 and verh.departure == 9999999999999) // поиск по др
			{
				for (int i = 0; i < spisok.size(); i++)
				{
					if (niz <= spisok[i] and spisok[i] <= verh) { arr.push_back(spisok[i]); }
				}
			}
			else if (niz.entry == 0 and niz.departure == 9999999999999 and verh.entry == 9999999999999)
			{
				for (int i = 0; i < spisok.size(); i++)
				{
					if (spisok[i].departure <= verh.departure and niz <= spisok[i] and spisok[i] <= verh) { arr.push_back(spisok[i]); }
				}
			}
			else if (niz.entry == 0 and niz.departure == 9999999999999)
			{
				for (int i = 0; i < spisok.size(); i++)
				{
					if (spisok[i].departure <= verh.departure and spisok[i].entry <= verh.entry and niz <= spisok[i] and spisok[i] <= verh) { arr.push_back(spisok[i]); }
				}
			}
			else //поиск по всему остальному
			{
				for (int i = 0; i < spisok.size(); i++)
				{
					if (niz <= spisok[i] and niz < spisok[i] and spisok[i] <= verh and spisok[i] < verh) { arr.push_back(spisok[i]); }
				}
			}
		}

	}
	cout << "ОТПРАВЛЯЮ " << arr.size() << endl;
}

void deletePerson(int ind, vector<migrant> & spisok)
{
	for (int i = 0; i < spisok.size(); i++) 
	{
		if (spisok[i].ID == ind)
		{
			spisok.erase(spisok.begin() + i);
		}
	}
}

/*
void FindPersons(vector<migrant>& spisok, vector<migrant>& arr)
{
	//SetConsoleCP(CP_UTF8);
	//SetConsoleOutputCP(CP_UTF8);
	migrant niz{}, verh{};
	tm birthday{}, entry{}, departure{};
	string surname, name, patronymic;
	cout << "Введите нижние границы поиска: (если поле не будет использоваться в поиске - введите 0) " << endl;

	cout << "Фамилия: ";
	getline(cin, surname);
	if (surname == "0") 
	{
		surname = "";
	}
	cout << "Имя: ";
	getline(cin, name);
	if (name == "0")
	{
		name = "";
	}
	cout << "Отчество: ";
	getline(cin, patronymic);
	if (patronymic == "0")
	{
		patronymic = "";
	}
	if (patronymic == "" and name == "" and surname == "")
	{
		niz.FIO = ".";
	}
	else 
	{
		niz.FIO = surname + " " + name + " " + patronymic;
	}
	cout << "Сформированная строка FIO: " << niz.FIO << endl;
	cout << "Введите страну прибывающего: ";
	getline(cin, niz.country);
	if (niz.country == "0")
	{
		niz.country = ".";
	}
	cout << "Введите дату рождения в формате день, месяц, год: ";
	cin >> birthday.tm_mday >> birthday.tm_mon >> birthday.tm_year;
	birthday.tm_year -= 1900;
	birthday.tm_mon -= 1;
	cin.ignore();
	cout << "Введите дату въезда в формате день, месяц, год: ";
	cin >> entry.tm_mday >> entry.tm_mon >> entry.tm_year;
	entry.tm_year -= 1900;
	entry.tm_mon -= 1;
	cin.ignore();
	cout << "Введите дату выезда в формате день, месяц, год: ";
	cin >> departure.tm_mday >> departure.tm_mon >> departure.tm_year;
	departure.tm_year -= 1900;
	departure.tm_mon -= 1;

	if (birthday.tm_year == -1900 or birthday.tm_mon == -1 or birthday.tm_mday == 0) 
	{
		niz.birthday = 0;
	}
	else {
		niz.birthday = mktime(&birthday);
	}
	if (entry.tm_year == -1900 or entry.tm_mon == -1 or entry.tm_mday == 0)
	{
		niz.entry = 0;
	}
	else {
		niz.entry = mktime(&entry);
	}
	if (departure.tm_year == -1900 or departure.tm_mon == -1 or departure.tm_mday == 0)
	{
		niz.departure = 0;
	}
	else {
		niz.departure = mktime(&departure);
	}
	int temp;
	cout << "Ввести верхнюю границу?(1 - да, 0 - нет)" << endl;
	cin >> temp;
	if (temp != 0 and temp != 1) {
		cout << "Невверно введенные данные" << endl;
	}
	else if (temp == 1) {
		cout << "ok" << endl;
	}
	else if (temp == 0) {
		cout << "ok" << endl;
	}
	cout << niz.ID << " ";
	cout << niz.FIO << " ";
	cout << " " << niz.country << " " << niz.birthday << " " << niz.entry << endl;
	Find(spisok, niz, verh, arr);
}
*/

void FindFromFile(ifstream& find, vector<migrant>& spisok, vector<migrant>& forfind)
{
    migrant niz{}, verh{};
    string line;
    getline(find, line);
    cout << line << endl;

    string key, value, name, surname, patronymic, country, day, year, month;
    tm nbirthday{}, nentry{}, ndeparture{};
    tm vbirthday{}, ventry{}, vdeparture{};
    int id = 0;
    bool nbe = false, nee = false, nde = false, vbe = false, vee = false, vde = false;

    istringstream ss(line);

    // Обработка поля id
    size_t pos = line.find("id=");
    if (pos != string::npos) {
        ss.seekg(pos + 3);
        getline(ss, value, ' ');
        if (!value.empty()) id = stoi(value);
    }

    // Обработка FIO
    pos = line.find("FIO=");
    if (pos != string::npos) {
        ss.clear();
        ss.seekg(pos + 4);
        getline(ss, surname, '_');
        getline(ss, name, '_');
        getline(ss, patronymic, ' ');
        if (patronymic == "_") patronymic = "";
		//cout << name << " " << surname << " " << patronymic << endl;
    }

    pos = line.find("country=");
    if (pos != string::npos) {
        ss.clear();
        ss.seekg(pos + 8);
        getline(ss, country, ' ');
        if (country.empty()) country = ".";
    }

    pos = line.find("birth_lower");
    if (pos != string::npos) {
        ss.clear();
        ss.seekg(pos + 11);
        getline(ss, day, '.'); nbirthday.tm_mday = stoi(day); if (nbirthday.tm_mday == 0) nbe = true;
        getline(ss, month, '.'); nbirthday.tm_mon = stoi(month) - 1;
        getline(ss, year, ';'); nbirthday.tm_year = stoi(year) - 1900;
    }
	else 
	{
		nbe = true;
	}
	pos = line.find("birth_higher");
	if (pos != string::npos)
	{
		ss.clear();
        ss.seekg(pos + 12);
		getline(ss, day, '.'); vbirthday.tm_mday = stoi(day); if (vbirthday.tm_mday == 0) vbe = true;
        getline(ss, month, '.'); vbirthday.tm_mon = stoi(month) - 1;
        getline(ss, year, ']'); vbirthday.tm_year = stoi(year) - 1900;
	}
	else 
	{
		vbe = true;
	}

    pos = line.find("entry_lower");
    if (pos != string::npos) {
        ss.clear();
        ss.seekg(pos + 11);
        getline(ss, day, '.'); nentry.tm_mday = stoi(day); if (nentry.tm_mday == 0) nee = true;
        getline(ss, month, '.'); nentry.tm_mon = stoi(month) - 1;
        getline(ss, year, ';'); nentry.tm_year = stoi(year) - 1900;
    }
	else 
	{
		nee = true;
	}
	pos = line.find("entry_higher");
	if (pos != string::npos)
	{
		ss.clear();
        ss.seekg(pos + 12);
		getline(ss, day, '.'); ventry.tm_mday = stoi(day); if (ventry.tm_mday == 0) vee = true;
        getline(ss, month, '.'); ventry.tm_mon = stoi(month) - 1;
        getline(ss, year, ']'); ventry.tm_year = stoi(year) - 1900;
	}
	else 
	{
		vee = true;
	}

    pos = line.find("departure_lower");
    if (pos != string::npos) 
	{
        ss.clear();
        ss.seekg(pos + 15);
        getline(ss, day, '.'); ndeparture.tm_mday = stoi(day); if (ndeparture.tm_mday == 0) nde = true;
        getline(ss, month, '.'); ndeparture.tm_mon = stoi(month) - 1;
        getline(ss, year, ';'); ndeparture.tm_year = stoi(year) - 1900;
    }
	else 
	{
		nde = true;
	}
	pos = line.find("departure_higher");
	if (pos != string::npos) 
	{
		ss.clear();
        ss.seekg(pos + 16);
		getline(ss, day, '.'); vdeparture.tm_mday = stoi(day); if (vdeparture.tm_mday == 0) vde = true;
        getline(ss, month, '.'); vdeparture.tm_mon = stoi(month) - 1;
        getline(ss, year, ']'); vdeparture.tm_year = stoi(year) - 1900;
	}
	else 
	{
		vde = true;
	}

    // Присвоение значений
    niz.ID = id;
	if ((surname.empty() and name.empty() and patronymic.empty())) {
		niz.FIO = ".";
	}
	else {
    	niz.FIO = surname + name + patronymic;
	}
	if (country.empty()) {
		niz.country = ".";
	} else {
		niz.country = country;
	}
	if (nbe) {
		niz.birthday = 0;
	} else {
		niz.birthday = mktime(&nbirthday);
	}
	
	if (nee) {
		niz.entry = 0;
	} else {
		niz.entry = mktime(&nentry);
	}
	
	if (nde) {
		niz.departure = 0;
	} else {
		niz.departure = mktime(&ndeparture);
	}
	
	if (vbe) {
		verh.birthday = 0;
	} else {
		verh.birthday = mktime(&vbirthday);
	}
	
	if (vee) {
		verh.entry = 0;
	} else {
		verh.entry = mktime(&ventry);
	}
	
	if (vde) {
		verh.departure = 999999999999;
	} else {
		verh.departure = mktime(&vdeparture);
	}
	

    if (verh.FIO == "." && verh.ID == 0 && verh.birthday == 0 && verh.country == "." && verh.departure == 0) {
        migrant em{};
        verh = em;
    }
	//cout << niz.ID << " " << niz.FIO << " " << niz.country << " " << niz.birthday;
    Find(spisok, niz, verh, forfind);
}

void FindFromLine(string line, vector<migrant>& spisok, vector<migrant>& forfind)
{
    migrant niz{}, verh{};

    string key, value, name, surname, patronymic, country, day, year, month;
    tm nbirthday{}, nentry{}, ndeparture{};
    tm vbirthday{}, ventry{}, vdeparture{};
    int id = 0;
    bool nbe = false, nee = false, nde = false, vbe = false, vee = false, vde = false;

    istringstream ss(line);
/*
	size_t pos = line.find("ID=");
    if (pos != string::npos) {
        ss.seekg(pos + 3);
        getline(ss, value, ' ');
        if (!value.empty()) id = stoi(value);
    }
*/
    // Обработка FIO
	cout << "На обработку: " << line << endl;
	size_t pos = line.find("FIO=");
    if (pos != string::npos) {
        ss.clear();
        ss.seekg(pos + 4);
        getline(ss, surname, '_');
        getline(ss, name, '_');
        getline(ss, patronymic, '_');
        if (patronymic == "_") patronymic = "";
		cout << "ИМЯ:" << name << " Фамилия " << surname << " Отчество " << patronymic << endl;
    }

    pos = line.find("country=");
    if (pos != string::npos) {
        ss.clear();
        ss.seekg(pos + 8);
        getline(ss, country, '&');
        if (country == "") 
		{
			country = ".";
		}
    }

    pos = line.find("birth_lower=");
    if (pos != string::npos) {
        ss.clear();
        ss.seekg(pos + 12);
		getline(ss, year, '&');
		if (year == "") 
		{
			nbe = true;
		}
		else 
		{
			ss.seekg(pos + 12);
			getline(ss, year, '-'); nbirthday.tm_year = stoi(year) - 1900;
       	 	getline(ss, month, '-'); nbirthday.tm_mon = stoi(month) - 1;
        	getline(ss, day, '&'); nbirthday.tm_mday = stoi(day);	
			if (nbirthday.tm_mday == 0) nbe = true;
		}
    }
	else 
	{
		nbe = true;
	}
	pos = line.find("birth_higher=");
	if (pos != string::npos)
	{
		ss.clear();
        ss.seekg(pos + 13);
		getline(ss, year, '&');
		if (year == "") 
		{
			vbe = true;
		}
		else 
		{
			ss.seekg(pos + 13);
			getline(ss, year, '-'); vbirthday.tm_year = stoi(year) - 1900;
       	 	getline(ss, month, '-'); vbirthday.tm_mon = stoi(month) - 1;
        	getline(ss, day, '&'); vbirthday.tm_mday = stoi(day);	
			if (vbirthday.tm_mday == 0) vbe = true;
		}
	}
	else 
	{
		vbe = true;
	}

    pos = line.find("entry_lower=");
    if (pos != string::npos) {
        ss.clear();
        ss.seekg(pos + 12);
		getline(ss, year, '&');
		if (year == "") 
		{
			nee = true;
		}
		else 
		{
			ss.seekg(pos + 12);
        	getline(ss, year, '-'); nentry.tm_year = stoi(year) - 1900;
        	getline(ss, month, '-'); nentry.tm_mon = stoi(month) - 1;
        	getline(ss, day, '&'); nentry.tm_mday = stoi(day);
			if (nentry.tm_mday == 0) nee = true;
		}
    }
	else 
	{
		nee = true;
	}

	pos = line.find("entry_higher=");
	if (pos != string::npos)
	{
		ss.clear();
        ss.seekg(pos + 13);
		getline(ss, year, '&');
		cout << year << endl;
		if (year == "") 
		{
			vee = true;
		}
		else 
		{
			ss.seekg(pos + 13);
			getline(ss, year, '-'); ventry.tm_year = stoi(year) - 1900; 
       	 	getline(ss, month, '-'); ventry.tm_mon = stoi(month) - 1;
        	getline(ss, day, '&'); ventry.tm_mday = stoi(day);
			if (ventry.tm_mday == 0) vee = true;
		}
	}
	else 
	{
		vee = true;
	}

    pos = line.find("departure_lower=");
    if (pos != string::npos) 
	{
        ss.clear();
        ss.seekg(pos + 16);
		getline(ss, year, '&');
		if (year == "")  
		{
			nde = true;
		}
		else 
		{
			ss.seekg(pos + 16);
       	 	getline(ss, year, '-'); ndeparture.tm_year = stoi(year) - 1900;
       	 	getline(ss, month, '-'); ndeparture.tm_mon = stoi(month) - 1;
        	getline(ss, day, '&'); ndeparture.tm_mday = stoi(day);
			if (ndeparture.tm_mday == 0) nde = true;
		}
    }
	else 
	{
		nde = true;
	}
	pos = line.find("departure_higher=");
	if (pos != string::npos) 
	{
		ss.clear();
        ss.seekg(pos + 17);
		getline(ss, year, '&');
		if (year == "") 
		{
			vde = true;
		}
		else
		{
			ss.seekg(pos + 17);
			getline(ss, year, '.'); vdeparture.tm_year = stoi(year) - 1900;
        	getline(ss, month, '.'); vdeparture.tm_mon = stoi(month) - 1;
        	getline(ss, day, '&'); vdeparture.tm_mday = stoi(day);
			if (vdeparture.tm_mday == 0) vde = true;
		}
	}
	else 
	{
		vde = true;
	}

    niz.ID = id;
	if ((surname.empty() and name.empty() and patronymic.empty())) {
		niz.FIO = ".";
	}
	else if (name.empty() and patronymic.empty()) 
	{
		niz.FIO = surname + " ";
	}
	else if (patronymic.empty()) 
	{
		niz.FIO = surname + " " + name + " ";
	}
	else if (surname.empty() and patronymic.empty()) 
	{
		niz.FIO = name;
	}
	else {
    	niz.FIO = surname + " " + name + " " + patronymic;
	}


	niz.country = country;
	
	if (nbe) {
		niz.birthday = 0;
	} else {
		niz.birthday = mktime(&nbirthday);
	}
	
	if (nee) {
		niz.entry = 0;
	} else {
		niz.entry = mktime(&nentry);
	}
	 
	if (nde) {
		niz.departure = 999999999999;
	} else {
		niz.departure = mktime(&ndeparture);
	}
	
	if (vbe) {
		verh.birthday = 999999999999;
	} else {
		verh.birthday = mktime(&vbirthday);
	}
	
	if (vee) {
		verh.entry = 0;
	} else {
		verh.entry = mktime(&ventry);
	}
	
	if (vde) {
		verh.departure = 999999999999;
	} else {
		verh.departure = mktime(&vdeparture);
	}
	

    if (verh.FIO == "." && verh.ID == 0 && verh.birthday == 0 && verh.country == "." && verh.departure == 0) {
        migrant em{};
        verh = em;
    }
	cout << niz.ID << " " << niz.FIO << " " << niz.country << " Дршка " << niz.birthday << " " << " " << niz.entry << " " << niz.departure << endl;
    Find(spisok, niz, verh, forfind);
}