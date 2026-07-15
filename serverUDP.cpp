#include <iostream>
#include <vector>
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
#include "Header.h"
using namespace std;

#define  PORT          5555
#define  BUFLEN         512
string buf;

void openservUDP(vector<migrant> & spisok, vector<migrant> & forfind)
{
    vector<string> buffer;
    int sock;
    int nbytes, i, err;
    unsigned int size;
    sockaddr_in addr_from; // адрес клиента
    sockaddr_in addr_this; // наш адрес

    hostent * hostinfo;
    hostinfo = gethostbyname("127.0.0.1");
    if (hostinfo == nullptr) 
    {
        perror("connection errors");
        exit(EXIT_FAILURE);
    }
    addr_this.sin_family = AF_INET; //семейство адрес - IPV4
    addr_this.sin_addr.s_addr = INADDR_ANY; //указывам, к какому IP будет "привязан" сокет при вызове bind. (в нашем случае к любому)
    addr_this.sin_port = htons(PORT);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("Client: socket was not created");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof(opt)); //Даем использовать привязанный к сокету порт повторно (не включается этот дилэй)

    err = bind(sock, (sockaddr*)&addr_this, sizeof(addr_this)); //привязываем сокет к нашему адресу (к порту и айпишнику)
    if (err < 0) {
        perror ("Client: cannot bind socket");
        exit (EXIT_FAILURE);
    }
    char buf[512];
    size = sizeof(addr_from); // size будет содержать размер структуры, в которую recvfrom запишет адрес отправителя.
    nbytes = recvfrom(sock, buf, sizeof(buf) + 1, 0, (sockaddr*)&addr_from, &size); //ждем сообщение и принимаем в buf (ждём входящий UDP-пакет на сокете sock -> записываем данные в буфер buf -> сохраняет адрес клиента в addr_from -> Возвращает количество полученных байт
    cout << buf << endl;   
    string line(buf);
    FindFromLine(line, spisok, forfind);
    int len = forfind.size();
    nbytes = sendto(sock, &len, sizeof(int), 0, (sockaddr*)&addr_from, sizeof(addr_from));
    if(nbytes < 0) 
    {
        perror("Client");
        close(sock);
        exit (EXIT_FAILURE);
    } else {
        cout << "отправляем клиенту " << nbytes * len << " байт в обратку" << endl;
    }
    for (int i = 0; i < len; i++) 
    {
        string message = to_string(forfind[i].ID) + " " + forfind[i].FIO + " Birthday: " + to_string(localtime(&forfind[i].birthday)->tm_mday) + "." + to_string(localtime(&forfind[i].birthday)->tm_mon + 1) + "." + to_string(1900 + localtime(&forfind[i].birthday)->tm_year) + " ";
        message += "Country: " + forfind[i].country + " ";
        message += "Entry: " + to_string(localtime(&forfind[i].entry)->tm_mday) + "." + to_string(localtime(&forfind[i].entry)->tm_mon + 1) + "." + to_string(1900 + localtime(&forfind[i].entry)->tm_year) + " ";
        message += "Deportation: " + to_string(localtime(&forfind[i].departure)->tm_mday) + "." + to_string(localtime(&forfind[i].departure)->tm_mon + 1) + "." + to_string(1900 + localtime(&forfind[i].departure)->tm_year);
        //cout << message << endl;
        char* buf = new char[message.length() + 1];
        int size_back = message.length() + 1;
        strcpy(buf, message.c_str());
        nbytes = sendto(sock, &size_back, sizeof(int), 0, (sockaddr*)&addr_from, sizeof(addr_from));
        nbytes = sendto(sock, buf, size_back, 0, (sockaddr*)&addr_from, sizeof(addr_from));
    }
}
