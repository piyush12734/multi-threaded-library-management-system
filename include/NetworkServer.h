
#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H

#include <winsock2.h>

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "Library.h"

class NetworkServer {
public:
    explicit NetworkServer(
        Library& library,
        std::uint16_t port = 54000
    );

    ~NetworkServer();

    // Prevent accidental copying of socket-owning objects.
    NetworkServer(const NetworkServer&) = delete;
    NetworkServer& operator=(const NetworkServer&) = delete;

    bool start();
    void stop();

private:
    void acceptLoop(SOCKET listeningSocket);
    void handleClient(SOCKET clientSocket);

    bool sendMessage(
        SOCKET clientSocket,
        const std::string& message
    );

    std::string processCommand(
        const std::string& commandLine
    );

    void removeClient(SOCKET clientSocket);

    Library& library_;
    std::uint16_t port_;

    SOCKET listenSocket_ = INVALID_SOCKET;
    bool winsockStarted_ = false;

    std::atomic<bool> running_{false};
    std::thread acceptThread_;

    std::mutex clientsMutex_;
    std::vector<SOCKET> clientSockets_;
    std::vector<std::thread> clientThreads_;
};

#endif
