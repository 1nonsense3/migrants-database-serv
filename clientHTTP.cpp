#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "Header.h"

using namespace std;

// Определимся с портом, адресом сервера и другими константами.
// В данном случае берем произвольный порт и адрес обратной связи
// (тестируем на одной машине).
#define  SERVER_PORT     5555
#define  SERVER_NAME    "127.0.0.1"
#define  BUFLEN          4096



// Две вспомогательные функции для чтения/записи (см. ниже)
int  writeToServer  (int fd, string sms);
int  readFromServer (int fd, ofstream & outfile);


int main (int a, char* filename[])
{
    string sleeptime;
    int st;
    string askname = filename[1];
    string outname = filename[2];
    ofstream outfile;
    outfile.open(outname);
    ifstream askfile;
    //cout << "Entry the name of the required script: ";
    //cin >> askname;
    askfile.open(askname);
    getline(askfile, sleeptime);
    st = stoi(sleeptime);

    int err;
    int sock;
    struct sockaddr_in server_addr;
    struct hostent    *hostinfo;
    hostinfo = gethostbyname(SERVER_NAME);
    if (hostinfo == NULL) 
    {
        fprintf (stderr, "Unknown host %s.\n",SERVER_NAME);
        exit (EXIT_FAILURE);
    }

    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr = *(struct in_addr*) hostinfo->h_addr;

    // Создаем TCP сокет.
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) 
    {
        perror ("Client: socket was not created");
        exit (EXIT_FAILURE);
    }

    // Устанавливаем соединение с сервером
    err = connect (sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (err < 0) 
    {
        perror ("Client:  connect failure");
        exit (EXIT_FAILURE);
    }
    fprintf (stdout,"Connection is ready\n");

    // Обмениваемся данными
    string sms;
    while (getline(askfile, sms)) {
       sleep(st);
       if (writeToServer(sock, sms)<0) break;
       if (readFromServer(sock, outfile)<0) break;
   }

        /*
        if (writeToServer(sock, "a")<0) break;
        if (readFromServer(sock)<0) break;
        */
    cout << "Клиент с входным файлом " << askname << " заканчивает работу" << endl;


    // Закрываем socket
    close (sock);
    exit (EXIT_SUCCESS);
}



int  writeToServer (int fd, string sms)
{
    int   nbytes;
    char buf[BUFLEN];
    cout << "Send to server request > " << sms << endl;
    if (sms == "") 
    {
       cout << "Пустое собщение серверу ";
       return 0;
    }

    nbytes = write(fd, sms.c_str(), sms.size());
    if (nbytes < 0) 
    { 
       perror("write"); return -1; 
    }
    if (strstr(sms.c_str(),"stop")) return -1;
    return 0;
}


int  readFromServer (int fd, ofstream & outfile)
{
    int   nbytes;
    int len = 0;
    nbytes = read(fd, &len, sizeof(len));
    for (int i = 0; i < len; i++)
   {
       int size_back;
       nbytes = read(fd, &size_back, sizeof(int));
       char * buf = new char[size_back];
       nbytes = read(fd, buf, size_back);
       outfile << buf << endl;
   }
    return 0;
}

