 #include <iostream>
 #include <stdio.h>
 #include <errno.h>
 #include <stdlib.h>
 #include <string.h>
 #include <ctype.h>
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <sys/poll.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <netdb.h>
 #include <vector>
 #include <algorithm>
 #include "Header.h"
 
 using namespace std;
 
 // Определимся с номером порта и другими константами.
 #define PORT    5555
 #define BUFLEN  512
 
 // Две вспомогательные функции для чтения/записи (см. ниже)
 int readFromClient (int fd, vector<migrant> & forfind, vector<migrant> & spisok);
 void writeToClient (int fd, vector<migrant> & forfind, vector<migrant> & spisok);
 
 
 int openservTCP(vector<migrant> & spisok, vector<migrant> & forfind)
 {
 
     int     i, err, opt = 1;
     int     sock, new_sock;
     struct  sockaddr_in  addr;
     struct  sockaddr_in  client;
     char    buf[BUFLEN];
     socklen_t  size;
 
     // Создаем TCP сокет для приема запросов на соединение
     sock = socket (PF_INET, SOCK_STREAM, 0);
     if (sock < 0) {
         perror ("Server: cannot create socket");
         exit (EXIT_FAILURE);
     }
     // опция чтобы не ждать при повторном запуске сервера
     setsockopt(sock,SOL_SOCKET, SO_REUSEADDR, (const void*)&opt, sizeof(opt));
 
     // Заполняем адресную структуру и
     // связываем сокет с любым адресом
     addr.sin_family = AF_INET;
     addr.sin_port = htons(PORT);
     addr.sin_addr.s_addr = htonl(INADDR_ANY);
     err = bind(sock,(struct sockaddr*)&addr, sizeof(addr)); // сокет для приема запросов
     if (err < 0) 
     {
         perror ("Server: cannot bind socket");
         exit (EXIT_FAILURE);
     }
 
     // Создаем очередь на 3 входящих запроса соединения
     err = listen(sock,3);
     if (err < 0) 
     {
         perror ("Server: listen queue failure");
         exit(EXIT_FAILURE);
     }
 
     // Подготавливаем множества дескрипторов каналов ввода-вывода.
     pollfd  act_set[100];
     act_set[0].fd = sock;
     act_set[0].events = POLLIN;   // requested event канал будет проверяться на событие POLLIN соответствующее появлению данных для чтения
     act_set[0].revents = 0;       // returned event   
     int num_set = 1;
 
     while (1) {
         // Проверим, не появились ли данные в каком-либо сокете.
         // В нашем варианте ждем до фактического появления данных.
         for (int i = 0; i < num_set; i++) 
         {
            cout << act_set[i].fd << " " << act_set[i].events << endl;
         }
         int ret = poll (act_set, num_set, -1);
         if (ret < 0) 
         {
             perror("Server: poll  failure");
             exit (EXIT_FAILURE);
         }
         if (ret > 0) {
            for (i = 0; i < num_set; i++) 
            {
               if (act_set[i].revents & POLLIN) // revents — это битовая маска, которая означает какие события реально произошли на этом сокете (дескрипторе) после вызова poll().
               { 
                   cout << "//////////////////////////////////////////////////////////" << endl;
                   cout << "Пришло сообщение на " << act_set[i].fd << " сокет" << endl;
                   act_set[i].revents &= ~POLLIN; //очищает флаг, чтобы ты не отреагировал дважды на одно и то же событие. - после ответа клиенту забываем про то что он чтото писал ранее
                   if (i == 0) 
                   { //кто - то хочет подключиться
                       size = sizeof(client); 
                       new_sock = accept(act_set[i].fd,(struct sockaddr*)&client,&size);
                       cout << "подключен новый клиент с порта " << htons(client.sin_port) << endl;
                       if (num_set < 100) 
                       {
                          act_set[num_set].fd = new_sock; //устанавливаем сокет для нового клиента
                          act_set[num_set].events = POLLIN; // начинаем отслеживать чтение
                          act_set[num_set].revents = 0;
                          num_set++;
                       } 
                       else 
                       {
                          cout << "больше нет места" << endl;
                          close(new_sock); // некуда добавлять
                       }
                    } 
                    else 
                    {
                       // пришли данные в уже существующем соединени
                       err = readFromClient(act_set[i].fd, forfind, spisok); // читаем с i-го клиента
                       if (err <= 0) 
                       { // ошибка или конец данных
                           cout << "Заканчиваем работу с портом " << act_set[i].fd << endl;
                           close (act_set[i].fd);
                           // удаляем из множества сокетов 
                           if (i <= num_set-1) 
                           {
                              act_set[i] = act_set[num_set - 1];
                              num_set--;
                              i--;
                           }
                       } 
                       else
                       {
                           // данные прочитаны нормально - отвечаем
                           writeToClient(act_set[i].fd, forfind, spisok);
                       }
                   }  
               }
            }
         }
     }
 }
 
 
 
 int readFromClient(int fd, vector<migrant> & forfind, vector<migrant> & spisok)
 {
     int  nbytes;
     char buf[BUFLEN];
     nbytes = read(fd, buf, BUFLEN);
     if (nbytes < 0) 
     {
         // ошибка чтения
         perror ("Server: read failure");
         return -1;
     } 
     if (strstr(buf,"stop")) 
     {
        return -1;
     } 
     else if (nbytes == 0) 
     {
         // больше нет данных
         return 0;
     } 
     else 
     {
         // есть данные
         cout << "получил запрос: " << buf << endl;
         string line = buf;
         FindFromLine(line, spisok, forfind);
         memset(buf, 0, BUFLEN);
         return 1;
     }
 }
 
 
 void writeToClient(int fd, vector<migrant> & forfind, vector<migrant> & spisok)
 {
     int  nbytes;
 
     int len = forfind.size();
     cout << "Длина массива: " << len << endl;
     nbytes = write(fd, &len, sizeof(int));
     //cout << "НБАЙТС " << nbytes << endl;
     if(nbytes < 0) 
     { 
        cout << "Ошибка отправки" << endl;
        exit (EXIT_FAILURE);
     } 
     else 
     {
        cout << "отправляем клиенту " << nbytes * len << " байт в ответ на порт номер " << fd << endl;
        cout << "//////////////////////////////////////////////////////////" << endl;

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
        nbytes = write(fd, &size_back, sizeof(int));
        nbytes = write(fd, buf, size_back);
     }
     forfind.clear();
 }
 
 