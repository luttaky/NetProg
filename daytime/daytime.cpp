#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Использование: " << argv[0] << " <IP> [порт]\n";
        std::cerr << "Пример: " << argv[0] << " 172.16.40.1 13\n";
        return 1;
    }

    const char* serverIP = argv[1];
    int port = (argc > 2) ? std::stoi(argv[2]) : 13;

    // cоздание UDP сокета
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Ошибка: не удалось создать сокет\n";
        return 1;
    }

    // настройка адреса сервера
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, serverIP, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Ошибка: неверный IP-адрес\n";
        close(sock);
        return 1;
    }

    const char* request = "\n";
    sendto(sock, request, strlen(request), 0,
           (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    // получение ответа
    char buffer[1024];
    struct sockaddr_in fromAddr;
    socklen_t fromLen = sizeof(fromAddr);
    int bytesRead = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, 
                             (struct sockaddr*)&fromAddr, &fromLen);
    
    if (bytesRead < 0) {
        std::cerr << "Ошибка: не удалось получить ответ\n";
        close(sock);
        return 1;
    }

    buffer[bytesRead] = '\0';
    std::cout << "Ответ от " << inet_ntoa(fromAddr.sin_addr) << ":\n";
    std::cout << buffer << std::endl;

    close(sock);
    return 0;
}