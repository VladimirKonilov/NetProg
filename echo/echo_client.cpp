#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>

#define DEFAULT_PORT 7  // Порт службы echo по умолчанию

void print_help() {
    std::cout << "Usage: ./echo_client <server_ip> [<port>]\n";
    std::cout << "Example: ./echo_client 172.16.40.1 7\n";
    std::cout << "This program connects to the specified echo server over TCP and sends a message.\n";
    std::cout << "If no port is specified, the default port 7 will be used.\n";
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3 || std::string(argv[1]) == "-h") {
        print_help();
        return 0;
    }

    int sock;
    struct sockaddr_in server_addr;
    char buffer[256];

    // Получение порта: используем аргумент командной строки или порт по умолчанию
    int port = (argc == 3) ? std::atoi(argv[2]) : DEFAULT_PORT;

    // Создание TCP сокета
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Ошибка при создании сокета");
        return 1;
    }

    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // Конвертация IP-адреса сервера из строки в двоичный вид
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Ошибка преобразования IP-адреса");
        close(sock);
        return 1;
    }

    // Подключение к серверу
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка при подключении к серверу");
        close(sock);
        return 1;
    }

    // Ввод сообщения для отправки
    std::cout << "Введите сообщение для отправки (или 'exit' для выхода): ";
    std::cin.getline(buffer, sizeof(buffer));

    // Цикл отправки и получения сообщений
    while (std::string(buffer) != "exit") {
        // Отправляем сообщение серверу
        if (send(sock, buffer, strlen(buffer), 0) < 0) {
            perror("Ошибка при отправке данных");
            close(sock);
            return 1;
        }

        // Получаем ответ от сервера
        int recv_len = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (recv_len < 0) {
            perror("Ошибка при получении данных");
            close(sock);
            return 1;
        }

        // Завершаем строку и выводим ответ
        buffer[recv_len] = '\0';
        std::cout << "Ответ от сервера: " << buffer << std::endl;

        // Ввод следующего сообщения
        std::cout << "Введите сообщение для отправки (или 'exit' для выхода): ";
        std::cin.getline(buffer, sizeof(buffer));
    }

    // Закрываем сокет
    close(sock);
    return 0;
}
