#include <time.h>
#include <iostream>
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

//под "всеми вариантами" подразумевается реализация работы по протоколу TCP и UDP - две реализации (с разделением по двум файлам)

//DNS сервис - служба сопоставления доменного имени айпи адресу
//hostent - структура, отвечающая за хост
//127.0.0.1 используется для обращения к себе (localhost)
//Функция perror интерпретирует значение глобальной переменной ERRNO в строку и выводит эту строку на стандартный поток

#define  PORT_THIS      5550
#define  PORT_TO        5555    
#define  BUFLEN         512

int main()
{
	int i = 0;
	//Протокол UDP 
	sockaddr_in addr_to; // адрес получателя
	sockaddr_in addr_this; // мой адрес (отправителя)
	sockaddr_in addr_from; 
	int nbytes, sock;
	unsigned int size;
	hostent* hostinfo;
	hostinfo = gethostbyname("127.0.0.1");
	if (hostinfo == nullptr) 
	{
		perror("error with connection");	
		exit(EXIT_FAILURE);
	}
	addr_to.sin_family = hostinfo->h_addrtype; //Устанавливаем семейство адресов для получателя (обычно AF_INET для IPv4).
	addr_to.sin_port = htons(PORT_TO); //приводит значение порта в сетевой порядок байтов
	addr_to.sin_addr = *(struct in_addr *) hostinfo->h_addr; //устанавливаем наш адрес в адрес получателя

	addr_this.sin_family = AF_INET; //задаем семейство адресов для локального адреса — IPv4.
	addr_this.sin_addr.s_addr = INADDR_ANY; //указывам, к какому IP будет "привязан" сокет при вызове bind. (в нашем случае к любому)
	addr_this.sin_port = htons(PORT_THIS); //задаем локальный порт, на котором через который будем отправлять сообщения.

	sock = socket(PF_INET, SOCK_DGRAM, 0); // - создание канала ввода-вывода для сетевого общения; ProtocolFamily Internet, датаграмный протокол, 0 - для UDP (Датаграммный протокол — это протокол, передающий данные через сеть связи без предварительного установления соединения и создания виртуального канала.)
	if (sock < 0) {
		perror("Client: socket was not created");
		exit (EXIT_FAILURE);
	}

	int opt = 1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof(opt)); //разрешаем повторно использовать порт, даже если он недавно использовался.
	int err = bind(sock, (sockaddr*)&addr_this, sizeof(addr_this)); // привязать канал (сокет) к сетевому интерфейсу нашей станции (addr_this)
	if (err < 0) {
		perror ("Client: cannot bind socket");
		exit (EXIT_FAILURE);
	}
	string sms;
	cout << "Message: ";
	getline(cin, sms);
	nbytes = sendto(sock, sms.c_str(), sms.size(), 0, (sockaddr*)&addr_to, sizeof(addr_to));
	if (nbytes < 0) 
	{
		cout << "aaa" << endl;
		perror("client");
		close(sock);
		exit(EXIT_FAILURE);
	}
	else 
	{
		cout << "sending message of " << nbytes << " bytes" << endl;
	}
	socklen_t ssize = sizeof(addr_from);
	int len = 0;
	nbytes = recvfrom(sock, &len, sizeof(int), 0, (sockaddr*)&addr_from, &ssize);
	if (nbytes <= 0) 
	{
		perror("server");
		close(sock);
		exit(EXIT_FAILURE);
	}
	//vector<char*> res(len);
	cout << len << endl;
	//string message;
	for (int i = 0; i < len; i++)
	{
		int size_back;
		nbytes = recvfrom(sock, &size_back, sizeof(int), 0, (sockaddr*)&addr_from, &ssize);
		char * buf = new char[size_back];
		nbytes = recvfrom(sock, buf, size_back, 0, (sockaddr*)&addr_from, &ssize);
		cout << buf << endl;
	}
	close(sock);
	return 0;
}

