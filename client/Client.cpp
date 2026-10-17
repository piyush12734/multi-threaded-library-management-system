
#include <winsock2.h>
#include <ws2tcpip.h>

#include <algorithm>
#include <cctype>
#include <climits>
#include <iostream>
#include <sstream>
#include <string>

bool sendLine(SOCKET socket, const std::string& line) {
    const std::string message = line + "\n";
    std::size_t totalSent = 0;

    while (totalSent < message.size()) {
        const std::size_t remaining = message.size() - totalSent;

        const int chunkSize = static_cast<int>(
            std::min(
                remaining,
                static_cast<std::size_t>(INT_MAX)
            )
        );

        const int bytesSent = send(
            socket,
            message.data() + totalSent,
            chunkSize,
            0
        );

        if (bytesSent == SOCKET_ERROR || bytesSent == 0) {
            std::cerr << "Send failed: "
                      << WSAGetLastError() << '\n';
            return false;
        }

        totalSent += static_cast<std::size_t>(bytesSent);
    }

    return true;
}

bool receiveLine(SOCKET socket, std::string& line) {
    line.clear();

    char ch;

    while (true) {
        const int bytesReceived = recv(
            socket,
            &ch,
            1,
            0
        );

        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Receive failed: "
                      << WSAGetLastError() << '\n';
            return false;
        }

        if (bytesReceived == 0) {
            return !line.empty();
        }

        if (ch == '\n') {
            return true;
        }

        if (ch != '\r') {
            line += ch;
        }

        if (line.size() > 8192) {
            std::cerr << "Server response is too long.\n";
            return false;
        }
    }
}

bool isQuitCommand(const std::string& command) {
    std::istringstream input(command);
    std::string word;

    input >> word;

    std::transform(
        word.begin(),
        word.end(),
        word.begin(),
        [](unsigned char ch) {
            return static_cast<char>(std::toupper(ch));
        }
    );

    return word == "QUIT";
}

int main() {
    // Initialize Windows network support.
    WSADATA data{};

    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    // Create the TCP socket.
    SOCKET clientSocket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );

    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: "
                  << WSAGetLastError() << '\n';

        WSACleanup();
        return 1;
    }

    // Configure the server address.
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(54000);

    if (InetPtonA(
            AF_INET,
            "127.0.0.1",
            &serverAddress.sin_addr
        ) != 1) {

        std::cerr << "Invalid server address.\n";

        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Connect to the library server.
    if (connect(
            clientSocket,
            reinterpret_cast<sockaddr*>(&serverAddress),
            sizeof(serverAddress)
        ) == SOCKET_ERROR) {

        std::cerr << "Connection failed: "
                  << WSAGetLastError()
                  << "\nIs the library server running?\n";

        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to library server.\n";

    std::string response;

    if (!receiveLine(clientSocket, response)) {
        std::cerr << "Could not receive server welcome message.\n";

        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server: " << response << '\n';
    std::cout << "Type HELP to see available commands.\n";

    // Interactive client loop.
    std::string command;

    while (true) {
        std::cout << "\nlibrary> " << std::flush;

        if (!std::getline(std::cin, command)) {
            break;
        }

        if (command.empty()) {
            continue;
        }

        if (!sendLine(clientSocket, command)) {
            break;
        }

        if (!receiveLine(clientSocket, response)) {
            std::cout << "Server disconnected.\n";
            break;
        }

        std::cout << "Server: " << response << '\n';

        if (isQuitCommand(command)) {
            break;
        }
    }

    // Release socket and networking resources.
    shutdown(clientSocket, SD_BOTH);
    closesocket(clientSocket);
    WSACleanup();

    std::cout << "Client disconnected.\n";

    return 0;
}
