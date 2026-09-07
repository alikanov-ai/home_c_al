#include <stdio.h> // Стандартный ввод-вывод (printf, scanf)
#include <stdlib.h> // Стандартная библиотека (exit)
#include <string.h> // Работа со строками (strstr, strchr, strcmp, strncpy, strlen)
#include <stdbool.h> // Тип bool (true/false)

// Заголовки Windows для работы с сетью
#include <winsock2.h> // Основная библиотека сокетов Windows
#include <ws2tcpip.h> // Дополнительные функции (inet_ntop и др.)
#include <conio.h> // Функции _kbhit() и _getch() для чтения клавиш без Enter

#define BUFSIZE 2048 // Размер буфера для приёма HTTP-запроса
#define PORT 8080 // Номер порта, на котором слушает сервер

// Парсинг HTTP-запроса
int parse_http_request(const char *buffer, char *path, size_t path_size)
{
	const char *start = strstr(buffer, "GET /"); // ищет в буфере подстроку "GET /". Это начало HTTP-запроса от браузера
	if (!start) return -1;
	start += 5; // Пропускаем 5 символов "GET /", чтобы start указывал на сам путь ("on1", например)
	const char *end = strchr(start, ' '); // strchr ищет первый пробел после пути — это конец пути (перед "HTTP/1.1")
	if (!end) return -1;
	size_t len = end - start; // Вычисляем длину пути как разницу указателей
	if (len >= path_size) len = path_size - 1; // Защита от переполнения: если путь длиннее буфера, обрезаем
	strncpy(path, start, len); // Копируем len символов пути в path
	path[len] = '\0'; // Добавляем нулевой символ '\0' в конец (т.к. строки должны заканчиваться нулём)
	return 0;
}

// Вывод информации о нажатии
void print_button_press(const char *path)
{
	if (strcmp(path, "on1")  == 0) printf("\n[EVENT] LED #1 -> ON\n");
	else if (strcmp(path, "off1") == 0) printf("\n[EVENT] LED #1 -> OFF\n");
	else if (strcmp(path, "on2")  == 0) printf("\n[EVENT] LED #2 -> ON\n");
	else if (strcmp(path, "off2") == 0) printf("\n[EVENT] LED #2 -> OFF\n");
	else printf("\n[INFO ] Unknown request: /%s\n", path);
}

// Формирование HTTP-ответа
void build_http_response(char *response, size_t size)
{
	const char *html_body =
		"<!DOCTYPE HTML><html><head>"
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
		"<title>ESP32 Web Server</title></head><body>"
		"<h1>ESP32 - Web Server</h1>"
		"<p>LED #1 <a href=\"/on1\"><button>ON</button></a>&nbsp;"
		"<a href=\"/off1\"><button>OFF</button></a></p>"
		"<p>LED #2 <a href=\"/on2\"><button>ON</button></a>&nbsp;"
		"<a href=\"/off2\"><button>OFF</button></a></p>"
		"</body></html>";

	snprintf(response, size,
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html; charset=utf-8\r\n"
		"Connection: close\r\n\r\n%s", html_body);
}

int main(void)
{
	// 1. Инициализация Winsock (обязательно для Windows)
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed\n");
		return 1;
	}

	SOCKET server_fd, client_fd;
	struct sockaddr_in server_addr, client_addr; // sockaddr_in — структура с IP-адресом и портом
	int client_len = sizeof(client_addr);
	char buffer[BUFSIZE]; // Буфер для HTTP-запроса от браузера
	char path[128]; // Путь (/on1, например)
	char response[BUFSIZE]; // Буфер для HTTP-ответа
	bool isExit = false; // Флаг завершения сервера

    // 2. Создание сокета
    // socket() создаёт TCP-сокет, AF_INET — IPv4, SOCK_STREAM — потоковый сокет (TCP), 0 — протокол выбирается автоматически
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == INVALID_SOCKET) // INVALID_SOCKET — константа ошибки в Windows (в Linux было бы < 0)
	{
		printf("Error establishing socket\n");
		WSACleanup(); // освобождаем Winsock
		return 1;
	}
	printf("=> Socket server has been created...\n");

	// 3. Настройка адреса
	memset(&server_addr, 0, sizeof(server_addr)); // обнуляет структуру
	server_addr.sin_family = AF_INET; // sin_family = AF_INET — семейство адресов IPv4
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY — слушать на всех сетевых интерфейсах (и 127.0.0.1, и Wi-Fi, и Ethernet)
	server_addr.sin_port = htons(PORT); // перевод чисел из порядка байт компьютера в сетевой порядок байт (big-endian). Это обязательно для работы с сетью.

	// 4. Привязка и прослушивание
	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		printf("Error binding connection\n");
		closesocket(server_fd); WSACleanup(); return 1;
	}
	if (listen(server_fd, 5) == SOCKET_ERROR)
	{
		printf("Error listening\n");
		closesocket(server_fd); WSACleanup(); return 1;
	}

	printf("=> Server is listening on port %d ...\n", PORT);
	printf("=> Open in browser: http://127.0.0.1:%d\n", PORT);
	printf("=> Press '#' to stop the server\n\n");

	// 5. Основной цикл
	while (!isExit)
	{
		// Проверка нажатия '#' в консоли (неблокирующая, через _kbhit)
		if (_kbhit())
		{
			char c = _getch();
			if (c == '#')
			{
				isExit = true;
				break;
			}
		}

        // Используем select с таймаутом, чтобы accept не блокировал программу навсегда
		fd_set readfds;
		struct timeval tv;
		FD_ZERO(&readfds);
		FD_SET(server_fd, &readfds); // добавляет server_fd в набор отслеживаемых сокетов
		tv.tv_sec = 1;  // Таймаут 1 секунда
		tv.tv_usec = 0;

		int activity = select(0, &readfds, NULL, NULL, &tv); // select() — функция, которая проверяет, есть ли данные в сокете
        
		// Если есть входящее подключение
		// FD_ISSET — проверяем, что событие произошло именно на server_fd
		if (activity > 0 && FD_ISSET(server_fd, &readfds))
		{
			// accept() — принимает подключение и создаёт новый сокет client_fd для работы именно с этим клиентом
			client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
			if (client_fd == INVALID_SOCKET) continue;
            
			char client_ip[INET_ADDRSTRLEN];
			// inet_ntop — преобразует IP-адрес из двоичного вида в строку (например, "127.0.0.1")
			inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
			printf("=> Client connected from %s\n", client_ip);

			// Чтение HTTP-запроса
			memset(buffer, 0, sizeof(buffer));
			int bytes = recv(client_fd, buffer, BUFSIZE - 1, 0); // recv() — читаем данные, которые прислал браузер (HTTP-запрос)
			if (bytes > 0)
			{
				if (parse_http_request(buffer, path, sizeof(path)) == 0) print_button_press(path); // "Разбор" кнопки
				// Формируем HTML-ответ и отправляем его браузеру через send()
				build_http_response(response, sizeof(response));
				send(client_fd, response, (int)strlen(response), 0);
			}
			// Закрываем соединение с клиентом (браузер получит ответ и отобразит страницу)
			closesocket(client_fd);
			printf("=> Client disconnected\n");
		}
	}

	// 6. Завершение работы
	printf("\n=> Server stopped. Goodbye...\n");
	closesocket(server_fd); // закрываем серверный сокет
	WSACleanup(); // Освобождаем ресурсы Winsock. Без этого могут быть утечки ресурсов.

	return 0;
}
