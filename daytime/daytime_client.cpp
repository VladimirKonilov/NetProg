#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>

#define DEFAULT_PORT 13  // Порт службы daytime по умолчанию

void print_help() {
    std::cout << "Usage: ./daytime_client <server_ip> [<port>]\n";
    std::cout << "Example: ./daytime_client 172.16.40.1 13\n";
    std::cout << "This program sends a request to the specified daytime server over UDP and prints the server's response.\n";
    std::cout << "If no port is specified, the default port 13 will be used.\n";
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

    // Создаем UDP сокет
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Ошибка при создании сокета");
        return 1;
    }

    // Настраиваем адрес сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // Конвертация IP-адреса сервера из строки в двоичный вид
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Ошибка преобразования IP-адреса");
        close(sock);
        return 1;
    }

    // Отправляем пустой пакет (запрос) серверу
    if (sendto(sock, "", 1, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка при отправке запроса");
        close(sock);
        return 1;
    }

    // Ожидание ответа от сервера
    socklen_t addr_len = sizeof(server_addr);
    int recv_len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&server_addr, &addr_len);
    if (recv_len < 0) {
        perror("Ошибка при получении данных");
        close(sock);
        return 1;
    }

    // Завершаем строку и выводим полученное время
    buffer[recv_len] = '\0';
    std::cout << "Текущее время от сервера: " << buffer << std::endl;

    // Закрываем сокет
    close(sock);
    return 0;
}
