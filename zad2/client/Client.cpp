#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <regex>

#pragma comment(lib, "ws2_32.lib")

void receive_messages(SOCKET sock, std::string& nickname) {
    char buffer[1024];
    while (true) {
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            std::cout << "Connection closed.\n";
            break;
        }
        buffer[bytes_received] = '\0';
        std::string received(buffer);

        std::smatch match;
        std::regex pattern(R"(^(\w+):\s)");

        std::regex nick_change_pattern(R"(^(\w+) changed nickname to (\w+)\.)");
        std::smatch match2;

        if (std::regex_search(received, match2, nick_change_pattern)) {
            if (match2[1] == nickname) {
                nickname = match2[2];
            }
        }

        if (std::regex_search(received, match, pattern)) {
            std::string sender = match[1];

            if (sender == nickname) {
                received.replace(0, sender.length(),
                                 "\033[32m" + sender + "\033[0m");
            } else {
                received.replace(0, sender.length(),
                                 "\033[33m" + sender + "\033[0m");
            }
        }

        std::cout << received << std::endl;
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    std::string nickname;
    std::cout << "Enter your nickname: ";
    std::getline(std::cin, nickname);

    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        std::cerr << "Connection failed.\n";
        return 1;
    }
    send(client_socket, nickname.c_str(), nickname.length(), 0);

    std::cout << "Connected to the server as " << nickname << "\n";

    std::thread receiver(receive_messages, client_socket, std::ref(nickname));
    receiver.detach();

    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "/exit") break;
        send(client_socket, message.c_str(), message.length(), 0);
        std::cout << "\33[A\33[2K\r";
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;
}
