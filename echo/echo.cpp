#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Использование: " << argv[0] << " <IP> [порт]\n";
        std::cerr << "Пример: " << argv[0] << " 127.0.0.1 8080\n";
        return 1;
    }

    const char* serverIP = argv[1];
    int port = (argc > 2) ? std::stoi(argv[2]) : 7;

    // Создание TCP сокета
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Ошибка: не удалось создать сокет\n";
        return 1;
    }

    // Настройка адреса сервера
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, serverIP, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Ошибка: неверный IP-адрес\n";
        close(sock);
        return 1;
    }

    // Установка соединения
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Ошибка: не удалось подключиться\n";
        close(sock);
        return 1;
    }

    std::cout << "Подключено к " << serverIP << ":" << port << std::endl;
    std::cout << "Введите сообщение (exit для выхода):" << std::endl;

    std::string message;
    while (std::getline(std::cin, message)) {
        if (message == "exit") break;

        // Отправка
        send(sock, message.c_str(), message.length(), 0);

        // Получение
        char buffer[1024];
        int bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            std::cerr << "Соединение разорвано\n";
            break;
        }

        buffer[bytesRead] = '\0';
        std::cout << "Эхо: " << buffer << std::endl;
    }

    close(sock);
    return 0;
}