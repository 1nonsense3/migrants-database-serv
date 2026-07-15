#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include "Migrant.h"

using namespace std;

void generateFIO(vector<migrant> & spisok);
void generateBirthday(vector<migrant>& spisok);
void generateCountry(vector<migrant>& spisok);
void generateEntryAndDeparture(vector<migrant>& spisok);
void addPerson(vector<migrant>& spisok);
void readFile(ifstream& file, vector<migrant>& spisok);
void Find(vector<migrant>& spisok, migrant niz, migrant verh, vector<migrant>& arr);
void FindPersons(vector<migrant>& spisok, vector<migrant>& arr);
void deletePerson(int ind, vector<migrant>& spisok);
void FindFromFile(ifstream& find, vector<migrant>& spisok, vector<migrant>& forfind);
void FindFromLine(string line, vector<migrant>& spisok, vector<migrant>& forfind);
void openservUDP(vector<migrant> & spisok, vector<migrant> & forfind);
int openservTCP(vector<migrant> & spisok, vector<migrant> & forfind);
int openservHTTP(vector<migrant> & spisok, vector<migrant> & forfind);