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
#define BUFLEN  4096

// Две вспомогательные функции для чтения/записи (см. ниже)
int readFromClientHTTP (int fd, vector<migrant> & forfind, vector<migrant> & spisok, char * buf);
void writeToClientHTTP (int fd, vector<migrant> & forfind, vector<migrant> & spisok, char * buf);
void ProcessHTML(stringstream &html, string & request);
string Decode(string current);


int openservHTTP(vector<migrant> & spisok, vector<migrant> & forfind)
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
                      err = readFromClientHTTP(act_set[i].fd, forfind, spisok, buf); // читаем с i-го клиента
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
                          writeToClientHTTP(act_set[i].fd, forfind, spisok, buf);
                      }
                  }  
              }
           }
        }
    }
}

void ProcessHTML(stringstream &html, string & request)
{
    /*
    char tbuf[64];   // время, когда обрабатываем этот запрос
    time_t ttt;      // можем потом поставить в заголовок ответа сервера
    time(&ttt);
    ctime_s(tbuf, 256, &ttt);
    printf("%s\n",tbuf);
    */

    //  генерируем выходной html
    // здесь request не влияет на выводимый html, но это нетрудно поменять
 
html << "<!DOCTYPE html>\r\n";
html << "<html>\r\n";
html << "<head>\r\n";
html << "<meta charset=\"UTF-8\">\r\n";
html << "<title>ФМС Скотляндии</title>\r\n";
html << "</head>\r\n";
html << "<body style=\"background-color:grey;\">\r\n";
html << "<h1 style=\"font-size:45px;\">Федеральная Миграционная служба страны Скотляндии</h1>\r\n";
html << "<p><strong>Введите поисковой запрос. Мы с радостью предоставляем всевозможные данные о мигрантах.</strong></p>\r\n";
html << "<form name=\"myform\" action=\"http://localhost:5555\" method=\"get\">\r\n";
html << "<strong>ФИО в формате Ф_И_О:</strong><br>\r\n";
html << "<input name=\"FIO\" type=\"text\" width=\"20\" value=\"\"><br>\r\n";
html << "<table>\r\n";
html << "<tr>\r\n";
html << "<th>Страна происхождения:</th>\r\n";
html << "</tr>\r\n";
html << "tr>\r\n";
html << "<td>\r\n";
html << "<select name=\"country\">\r\n";
html << "<option value=\"\"> —выберите страну— </option>\r\n";
html << "<option value=\"Казахстан\">Казахстан</option>\r\n";
html << "<option value=\"Армения\">Армения</option>\r\n";
html << "<option value=\"США\">США</option>\r\n";
html << "<option value=\"Китай\">Китай</option>\r\n";
html << "<option value=\"Узбекистан\">Узбекистан</option>\r\n";
html << "<option value=\"Украина\">Украина</option>\r\n";
html << "<option value=\"Таджикистан\">Таджикистан</option>\r\n";
html << "<option value=\"Кыргызстан\">Кыргызстан</option>\r\n";
html << "<option value=\"Молдова\">Молдова</option>\r\n";
html << "<option value=\"Беларусь\">Беларусь</option>\r\n";
html << "<option value=\"Грузия\">Грузия</option>\r\n";
html << "<option value=\"Германия\">Германия</option>\r\n";
html << "<option value=\"Литва\">Литва</option>\r\n";
html << "<option value=\"Латвия\">Латвия</option>\r\n";
html << "</select>\r\n";
html << "</td>\r\n";
html << "</tr>\r\n";
html << "</table>\r\n";
html << "<strong>Нижняя и верхняя границы даты рождения</strong><br>\r\n";
html << "Нижняя граница: <input name=\"birth_lower\" type=\"date\" value=\"\"><br>\r\n";
html << "Верхняя граница: <input name=\"birth_higher\" type=\"date\" value=\"\"><br>\r\n";
html << "<strong>Нижняя и верхняя даты въезда</strong><br>\r\n";
html << "Нижняя граница: <input name=\"entry_lower\" type=\"date\" value=\"\"><br>\r\n";
html << "Верхняя граница: <input name=\"entry_higher\" type=\"date\" value=\"\"><br>\r\n";
html << "<strong>Нижняя и верхняя границы даты выезда</strong><br>\r\n";
html << "Нижняя граница: <input name=\"departure_lower\" type=\"date\" value=\"\"> <br>\r\n";
html << "Верхняя граница: <input name=\"departure_higher\" type=\"date\" value=\"\"><br>\r\n";
html << "<input name=\"sub\" type=\"submit\" value=\"Send\">\r\n";
html << "</form>\r\n";
html << "</body>\r\n";
html << "</html>\r\n";
    
}

int readFromClientHTTP(int fd, vector<migrant> & forfind, vector<migrant> & spisok, char *buf)
{
    int  nbytes;

    nbytes = recv(fd, buf, BUFLEN, 0);
    cout << "Считываю " << nbytes << " байт с " << fd << " сокета" << endl;
    if (nbytes < 0) 
    {
        // ошибка чтения
        perror("Server: read failure");
        return -1;
    }
    else if (nbytes == 0) 
    {
        return 0;
    } 
    else 
    {
        // есть данные
        cout << "Сервер получил " << nbytes << " байт от клиента" << endl;
        buf[min(nbytes, BUFLEN - 1)] = 0;
        printf("%s\n", buf);
        return nbytes;
    }
}


void writeToClientHTTP(int fd, vector<migrant> & forfind, vector<migrant> & spisok, char *buf)
{
    int  nbytes;
    stringstream head; 
    stringstream html;
    string request = "";   // строка параметров запроса
    string FIO = "", country = "", birth_lower = "", birth_higher = "", entry_lower = "", entry_higher = "", departure_lower = "", departure_higher = "", a = "";

    if (strstr(buf, "GET")) 
    {  // здесь параметры в первой строке после /
        char *p1 = strchr(buf, '/');
        char *p2 = strchr(p1, ' ');
        *p2 = 0;
        request = p1;
        cout << "Получил запрос: " << request.c_str() << endl;
        string a = request.c_str();
        a = Decode(a);
        istringstream ss(a);
        cout << "Декодировал: " << a << endl;
        FindFromLine(a, spisok, forfind);
        *p2 = ' ';


        size_t pos = a.find("FIO=");
        if (pos != string::npos) 
        {
            ss.clear();
            ss.seekg(pos + 4);
            getline(ss, FIO, '&');
        }

        pos = a.find("country=");
        if (pos != string::npos) 
        {
            ss.clear();
            ss.seekg(pos + 8);
            getline(ss, country, '&');
        }

        pos = a.find("birth_lower=");
        if (pos != string::npos)   
        {
            ss.clear();
            ss.seekg(pos + 12);
		    getline(ss, birth_lower, '&');
        }
	    pos = a.find("birth_higher=");
	    if (pos != string::npos)
	    {
		    ss.clear();
            ss.seekg(pos + 13);
            getline(ss, birth_higher, '&');
	    }

        pos = a.find("entry_lower=");
        if (pos != string::npos) 
        {
            ss.clear();
            ss.seekg(pos + 12);
            getline(ss, entry_lower, '&'); 
        }
	    pos = a.find("entry_higher=");
	    if (pos != string::npos)
	    {
		    ss.clear();
            ss.seekg(pos + 13);
            getline(ss, entry_higher, '&');
	    }

        pos = a.find("departure_lower=");
        if (pos != string::npos) 
	    {
            ss.clear();
            ss.seekg(pos + 16);
		    getline(ss, departure_lower, '&');
        }
	    pos = a.find("departure_higher=");
	    if (pos != string::npos) 
	    {
		    ss.clear();
            ss.seekg(pos + 17);
		    getline(ss, departure_higher, '&');
	    }
    }
    if (strstr(buf, "POST")) 
    {  // здесь параметры после всего заголовка после пустой строки
        char *p1 = strstr(buf, "\r\n\r\n");
        request = (p1 + 4);
        printf("post request: %s\n", request.c_str());
    }
    
    if (request != "/") 
    {
        html << "<!DOCTYPE html>\r\n";
        html << "<html>\r\n";   
        html << "<head>\r\n";
        html << "<meta charset=\"UTF-8\">\r\n";
        html << "<title>ФМС Скотляндии</title>\r\n";
        html << "</head>\r\n";
        html << "<body style=\"background-color:grey;\">\r\n";
        html << "<h1 style=\"font-size:45px;\">Федеральная Миграционная служба страны Скотляндии</h1>\r\n";
        html << "<p><strong>Введите поисковой запрос. Мы с радостью предоставляем всевозможные данные о мигрантах.</strong></p>\r\n";
        html << "<form name=\"myform\" action=\"http://localhost:5555\" method=\"get\">\r\n";
        html << "<strong>ФИО в формате Ф_И_О:</strong><br>\r\n";
        html << "<input name=\"FIO\" type=\"text\" width=\"20\" value=\"" << FIO << "\"><br>\r\n";
        html << "<table>\r\n";
        html << "<tr>\r\n";
        html << "<th>Страна происхождения:</th>\r\n";
        html << "</tr>\r\n";
        html << "<tr>\r\n";
        html << "<td>\r\n";
        html << "<select name=\"country\">\r\n";
        html << "<option value=\"\">" << country <<  "</option>\r\n";
        html << "<option value=\"Казахстан\">Казахстан</option>\r\n";
        html << "<option value=\"Армения\">Армения</option>\r\n";
        html << "<option value=\"США\">США</option>\r\n";
        html << "<option value=\"Китай\">Китай</option>\r\n";
        html << "<option value=\"Узбекистан\">Узбекистан</option>\r\n";
        html << "<option value=\"Украина\">Украина</option>\r\n";
        html << "<option value=\"Таджикистан\">Таджикистан</option>\r\n";
        html << "<option value=\"Кыргызстан\">Кыргызстан</option>\r\n"; 
        html << "<option value=\"Молдова\">Молдова</option>\r\n";
        html << "<option value=\"Беларусь\">Беларусь</option>\r\n"; 
        html << "<option value=\"Грузия\">Грузия</option>\r\n";
        html << "<option value=\"Германия\">Германия</option>\r\n";
        html << "<option value=\"Литва\">Литва</option>\r\n";
        html << "<option value=\"Латвия\">Латвия</option>\r\n";
        html << "</select>\r\n";
        html << "</td>\r\n";
        html << "</tr>\r\n";
        html << "</table>\r\n";
        html << "<strong>Нижняя и верхняя границы даты рождения</strong><br>\r\n";
        html << "Нижняя граница: <input name=\"birth_lower\" type=\"date\" value=\""<< birth_lower << "\"><br>\r\n";
        html << "Верхняя граница: <input name=\"birth_higher\" type=\"date\" value=\""<< birth_higher << "\"><br>\r\n";
        html << "<strong>Нижняя и верхняя даты въезда</strong><br>\r\n";
        html << "Нижняя граница: <input name=\"entry_lower\" type=\"date\" value=\""<< entry_lower << "\"><br>\r\n";
        html << "Верхняя граница: <input name=\"entry_higher\" type=\"date\" value=\""<< entry_higher << "\"><br>\r\n";
        html << "<strong>Нижняя и верхняя границы даты выезда</strong><br>\r\n";
        html << "Нижняя граница: <input name=\"departure_lower\" type=\"date\" value=\""<< departure_lower << "\"> <br>\r\n";
        html << "Верхняя граница: <input name=\"departure_higher\" type=\"date\" value=\""<< departure_higher << "\"><br>\r\n";
        html << "<input name=\"sub\" type=\"submit\" value=\"Send\">\r\n";
        html << "<br><strong>Результаты поиска:</strong><br>\r\n";
        html << "<table border=\"1\" style=\"background-color:grey;\">\r\n";
        html << "<tr><th>№</th><th>ФИО</th><th>Дата рождения</th><th>Страна происхождения</th><th>Дата вьезда</th><th>Дата выезда</th></tr>\r\n";
        for (int i = 0; i < forfind.size(); i++)
        {
            html << "<tr><td>" << i + 1 << "</td><td>" << forfind[i].FIO;
            html << "</td><td>\r\n" << localtime(&forfind[i].birthday)->tm_mday << "." << localtime(&forfind[i].birthday)->tm_mon + 1 << "." << localtime(&forfind[i].birthday)->tm_year + 1900;
            html << "</td><td>\r\n" << forfind[i].country;
            html << "</td><td>\r\n" << localtime(&forfind[i].entry)->tm_mday << "." << localtime(&forfind[i].entry)->tm_mon + 1 << "." << localtime(&forfind[i].entry)->tm_year + 1900;
            if (localtime(&forfind[i].departure)->tm_mday == 1 and localtime(&forfind[i].departure)->tm_mon + 1 == 1 and localtime(&forfind[i].departure)->tm_year + 1900 == 1970) 
            {
                html << "</td><td>\r\n" << "не выехал" << "</td></tr>\r\n";
            }
            else 
            {
                html << "</td><td>\r\n" << localtime(&forfind[i].departure)->tm_mday << "." << localtime(&forfind[i].departure)->tm_mon + 1 << "." << localtime(&forfind[i].departure)->tm_year + 1900;
                html << "</td></tr>\r\n";
            }
        }
        html << "</table>\r\n";
        html << "</form>\r\n";
        html << "</body>\r\n";
        html << "</html>\r\n";
        
    }
    else 
    {
        ProcessHTML(html, request);
    }

    
    // ответ на распознанные запросы
    // формирование HTML
    int html_length = html.str().length();

    // формирование HTTP заголовка
    head << "HTTP/1.1 200 OK\r\n";
    head << "Connection: keep-alive\r\n";
    head << "Content-type: text/html\r\n";
    head << "Content-length: " << html_length << "\r\n";
    head << "\r\n";

    // вывод заголовка и html
    nbytes = send(fd, head.str().c_str(), head.str().length() + 1, 0);
    printf("http nb = %d\n", nbytes);
    nbytes = send(fd, html.str().c_str(), html.str().length() + 1, 0);
    printf("html nb = %d\n", nbytes);
    forfind.clear();

    if (nbytes < 0) 
    {
        cout << "AAAAAA nbytes < 0" << endl;
        perror("Server: write failure");
    }
}

string Decode(string current) 
{
    string result;
    stringstream res;
    res << current;
    char ch;
    int i, ii;
    for (i = 0; i < current.length(); i++) 
    {
        if (current[i] == '%') {
            if (i + 2 < current.length()) 
            {
                stringstream pod;
                pod << current[i + 1]; pod << current[i + 2];
                if (pod >> hex >> ii) 
                {
                    ch = static_cast<char>(ii);
                    result += ch;
                    i += 2;
                }
                else 
                {
                    result += '%';
                }
            } 
            else 
            {
                result += '%';
            }
        }
        else 
        {
            result += current[i];
        }
    }
    return result;
}