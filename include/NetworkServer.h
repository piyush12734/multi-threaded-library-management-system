
#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H

#include <winsock2.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
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

    NetworkServer(const NetworkServer&) = delete;
    NetworkServer& operator=(const NetworkServer&) = delete;

    bool start();
    void stop();

private:
    // Tracks whether an individual client handler has finished.
    struct ClientThread {
        std::thread worker;
        std::shared_ptr<std::atomic<bool>> finished;
    };

    static constexpr std::size_t MAX_CLIENTS = 64;

    void acceptLoop(SOCKET listeningSocket);
    void handleClient(SOCKET clientSocket);

    void reapFinishedClientThreads();

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
    std::vector<ClientThread> clientThreads_;
};

#endif
