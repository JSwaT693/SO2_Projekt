#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <map>

const uint16_t PORT = 12345;

std::vector<SOCKET> clients;
std::mutex clients_mutex;
std::map<SOCKET, std::string> client_names;
std::vector<std::string> messages_history;
size_t HISTORY_SIZE = 100;

void broadcast(const std::string& message, SOCKET sender) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (SOCKET client : clients) {
        if (sender == -1 || client != sender) {
            send(client, message.c_str(), message.length(), 0);
        }
    }
}

void handle_client(SOCKET client_socket) {
    char buffer[1024];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) return;
    buffer[bytes_received] = '\0';

    std::string nickname = buffer;

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        client_names[client_socket] = nickname;
    }
    std::string join_msg = nickname + " has joined the chat.\n";
    std::cout << "\n>>> " << join_msg;
    broadcast(join_msg, -1);

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (const auto& msg : messages_history) {
            send(client_socket, msg.c_str(), msg.length(), 0);
            Sleep(10);
        }
    }

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytes_received] = '\0';
        std::string sender;
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            sender = client_names[client_socket];
        }
        std::string input(buffer);

        if (input == "/who") {
            std::ostringstream response;
            response << "Active users:\n";

            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                for (const auto& pair : client_names) {
                    response << "- " << pair.second << "\n";
                }
            }

            send(client_socket, response.str().c_str(), response.str().length(), 0);
            continue;
        }
        if (input.substr(0, 6) == "/nick ") {
            std::string new_nick = input.substr(6);

            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                client_names[client_socket] = new_nick;
            }

            std::ostringstream info;
            info << nickname << " changed nickname to " << new_nick << ".\n";
            nickname = new_nick;

            std::cout << ">>> " << info.str();
            broadcast(info.str(), -1);
            continue;
        }

        std::ostringstream msg;
        msg << sender << ": " << buffer;
        std::cout << msg.str() << std::endl;
        broadcast(msg.str(), -1);

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            messages_history.push_back(msg.str());
            if (messages_history.size() > HISTORY_SIZE) {
                messages_history.erase(messages_history.begin());
            }
        }

    }

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
        client_names.erase(client_socket);
    }

    std::string leave_msg = nickname + " has left the chat.\n";
    std::cout << "<<< " << leave_msg;
    broadcast(leave_msg, -1);

    closesocket(client_socket);
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, SOMAXCONN);

    std::cout << "[SERVER STARTING UP]...\n";

    std::cout << "Server listening on port " << PORT << "...\n";

    while (true) {
        SOCKET client_socket;
        sockaddr_in client_addr{};
        int client_len = sizeof(client_addr);
        client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_len);

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(client_socket);
        }

        std::thread t(handle_client, client_socket);
        t.detach();
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
