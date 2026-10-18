
#include "NetworkServer.h"

#include <exception>
#include <algorithm>
#include <cctype>
#include <climits>
#include <iostream>
#include <sstream>
#include <utility>

namespace {
    constexpr std::size_t MAX_COMMAND_LENGTH = 4096;

    std::string uppercase(std::string text) {
        std::transform(
            text.begin(),
            text.end(),
            text.begin(),
            [](unsigned char ch) {
                return static_cast<char>(std::toupper(ch));
            }
        );

        return text;
    }

    std::string resultToString(OperationResult result) {
        switch (result) {
            case OperationResult::Success:
                return "SUCCESS";

            case OperationResult::MemberNotFound:
                return "FAILED - member not found";

            case OperationResult::BookNotFound:
                return "FAILED - book not found";

            case OperationResult::BookNotAvailable:
                return "FAILED - book already borrowed";

            case OperationResult::BookAlreadyAvailable:
                return "FAILED - book is already available";

            case OperationResult::DatabaseError:
                return "FAILED - database error";
        }

        return "FAILED - unknown result";
    }

    std::string commandVerb(const std::string& command) {
        std::istringstream input(command);
        std::string verb;
        input >> verb;
        return uppercase(verb);
    }
}

NetworkServer::NetworkServer(Library& library, std::uint16_t port)
    : library_(library), port_(port) {
}

NetworkServer::~NetworkServer() {
    stop();
}

bool NetworkServer::start() {
    if (running_.load()) {
        std::cerr << "Server is already running.\n";
        return false;
    }

    WSADATA data{};

    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return false;
    }

    winsockStarted_ = true;

    listenSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (listenSocket_ == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: "
                  << WSAGetLastError() << '\n';

        WSACleanup();
        winsockStarted_ = false;
        return false;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(
            listenSocket_,
            reinterpret_cast<sockaddr*>(&serverAddress),
            sizeof(serverAddress)
        ) == SOCKET_ERROR) {

        std::cerr << "Bind failed: "
                  << WSAGetLastError() << '\n';

        closesocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;

        WSACleanup();
        winsockStarted_ = false;
        return false;
    }

    if (listen(listenSocket_, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: "
                  << WSAGetLastError() << '\n';

        closesocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;

        WSACleanup();
        winsockStarted_ = false;
        return false;
    }

    running_.store(true);

    try {
        acceptThread_ = std::thread(
            &NetworkServer::acceptLoop,
            this,
            listenSocket_
        );
    } catch (const std::exception& error) {
        std::cerr << "Could not start server thread: "
                  << error.what() << '\n';

        running_.store(false);
        closesocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;

        WSACleanup();
        winsockStarted_ = false;
        return false;
    }

    std::cout << "Library server listening on port "
              << port_ << '\n';

    return true;
}


void NetworkServer::stop() {
    running_.store(false);

    // Stop accepting new connections.
    if (listenSocket_ != INVALID_SOCKET) {
        shutdown(listenSocket_, SD_BOTH);
        closesocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;
    }

    // Wait for the accept loop to finish.
    if (acceptThread_.joinable()) {
        acceptThread_.join();
    }

    // Wake up client handlers that may be waiting in recv().
    {
        std::lock_guard<std::mutex> lock(clientsMutex_);

        for (SOCKET client : clientSockets_) {
            shutdown(client, SD_BOTH);
        }
    }

    // Join every remaining client handler.
    for (ClientThread& clientThread : clientThreads_) {
        if (clientThread.worker.joinable()) {
            clientThread.worker.join();
        }
    }

    clientThreads_.clear();

    // Clean up any sockets left behind.
    {
        std::lock_guard<std::mutex> lock(clientsMutex_);

        for (SOCKET client : clientSockets_) {
            shutdown(client, SD_BOTH);
            closesocket(client);
        }

        clientSockets_.clear();
    }

    if (winsockStarted_) {
        WSACleanup();
        winsockStarted_ = false;
    }

    std::cout << "Library server stopped.\n";
}



void NetworkServer::acceptLoop(SOCKET listeningSocket) {
    while (running_.load()) {
        // Join and remove client threads that have already finished.
        reapFinishedClientThreads();

        // Check for incoming connections with a timeout.
        // This lets us periodically reclaim completed threads,
        // even when no new clients are connecting.
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(listeningSocket, &readSet);

        timeval timeout{};
        timeout.tv_sec = 1;

        int ready = select(
            0,
            &readSet,
            nullptr,
            nullptr,
            &timeout
        );

        if (ready == 0) {
            continue;
        }

        if (ready == SOCKET_ERROR) {
            if (!running_.load()) {
                break;
            }

            std::cerr << "Select failed: "
                      << WSAGetLastError() << '\n';
            continue;
        }

        if (!running_.load()) {
            break;
        }

        SOCKET clientSocket = accept(
            listeningSocket,
            nullptr,
            nullptr
        );

        if (clientSocket == INVALID_SOCKET) {
            if (!running_.load()) {
                break;
            }

            std::cerr << "Accept failed: "
                      << WSAGetLastError() << '\n';
            continue;
        }

        bool registered = false;

        {
            std::lock_guard<std::mutex> lock(clientsMutex_);

            if (clientSockets_.size() < MAX_CLIENTS) {
                clientSockets_.push_back(clientSocket);
                registered = true;
            }
        }

        // Reject connections if the server is at capacity.
        if (!registered) {
            sendMessage(
                clientSocket,
                "ERROR server at capacity\n"
            );

            shutdown(clientSocket, SD_BOTH);
            closesocket(clientSocket);
            continue;
        }

        std::shared_ptr<std::atomic<bool>> finished;
        bool entryAdded = false;

        try {
            finished =
                std::make_shared<std::atomic<bool>>(false);

            // Add the thread record before starting the worker.
            // If allocation fails, no joinable thread is created.
            clientThreads_.emplace_back();
            entryAdded = true;

            clientThreads_.back().finished = finished;

            clientThreads_.back().worker = std::thread(
                [this, clientSocket, finished]() {
                    try {
                        handleClient(clientSocket);
                    } catch (const std::exception& error) {
                        std::cerr << "Client handler error: "
                                  << error.what() << '\n';

                        removeClient(clientSocket);
                    } catch (...) {
                        std::cerr
                            << "Unknown client handler error.\n";

                        removeClient(clientSocket);
                    }

                    finished->store(
                        true,
                        std::memory_order_release
                    );
                }
            );

            std::cout << "Client connected. Active clients: ";

            {
                std::lock_guard<std::mutex> lock(clientsMutex_);
                std::cout << clientSockets_.size() << '\n';
            }
        } catch (const std::exception& error) {
            std::cerr << "Could not start client handler: "
                      << error.what() << '\n';

            if (entryAdded &&
                !clientThreads_.back().worker.joinable()) {
                clientThreads_.pop_back();
            }

            removeClient(clientSocket);
        }
    }

    // Reclaim completed threads before leaving the accept loop.
    reapFinishedClientThreads();
}



void NetworkServer::reapFinishedClientThreads() {
    auto it = clientThreads_.begin();

    while (it != clientThreads_.end()) {
        const bool finished =
            it->finished &&
            it->finished->load(std::memory_order_acquire);

        if (finished) {
            if (it->worker.joinable()) {
                it->worker.join();
            }

            it = clientThreads_.erase(it);
        } else {
            ++it;
        }
    }
}


bool NetworkServer::sendMessage(
    SOCKET clientSocket,
    const std::string& message
) {
    std::size_t totalSent = 0;

    while (totalSent < message.size()) {
        const std::size_t remaining =
            message.size() - totalSent;

        const int bytesToSend =
            remaining > static_cast<std::size_t>(INT_MAX)
                ? INT_MAX
                : static_cast<int>(remaining);

        int bytesSent = send(
            clientSocket,
            message.data() + totalSent,
            bytesToSend,
            0
        );

        if (bytesSent == SOCKET_ERROR || bytesSent == 0) {
            return false;
        }

        totalSent += static_cast<std::size_t>(bytesSent);
    }

    return true;
}

void NetworkServer::handleClient(SOCKET clientSocket) {
    if (!sendMessage(
            clientSocket,
            "Welcome to the Library Server. Type HELP for commands.\n"
        )) {
        removeClient(clientSocket);
        return;
    }

    char buffer[1024];
    std::string pending;

    bool shouldClose = false;

    while (running_.load() && !shouldClose) {
        int bytesReceived = recv(
            clientSocket,
            buffer,
            sizeof(buffer),
            0
        );

        if (bytesReceived <= 0) {
            break;
        }

        pending.append(
            buffer,
            static_cast<std::size_t>(bytesReceived)
        );

        std::size_t newlinePosition;

        while ((newlinePosition = pending.find('\n'))
               != std::string::npos) {

            std::string command = pending.substr(
                0,
                newlinePosition
            );

            pending.erase(0, newlinePosition + 1);

            // Support Windows clients that send CRLF.
            if (!command.empty() && command.back() == '\r') {
                command.pop_back();
            }

            if (command.size() > MAX_COMMAND_LENGTH) {
                if (!sendMessage(
                        clientSocket,
                        "ERROR command too long\n"
                    )) {
                    shouldClose = true;
                    break;
                }

                continue;
            }

            if (command.empty()) {
                if (!sendMessage(
                        clientSocket,
                        "ERROR empty command\n"
                    )) {
                    shouldClose = true;
                    break;
                }

                continue;
            }

            const std::string verb = commandVerb(command);
            const std::string response = processCommand(command);

            if (!sendMessage(clientSocket, response + "\n")) {
                shouldClose = true;
                break;
            }

            if (verb == "QUIT") {
                shouldClose = true;
                break;
            }
        }

        // Prevent a client from sending an unlimited line
        // without ever sending a newline.
        if (pending.size() > MAX_COMMAND_LENGTH) {
            sendMessage(clientSocket, "ERROR command too long\n");
            break;
        }
    }

    removeClient(clientSocket);
}

std::string NetworkServer::processCommand(
    const std::string& commandLine
) {
    std::istringstream input(commandLine);

    std::string command;
    input >> command;
    command = uppercase(command);

    if (command == "HELP") {
        return
            "Commands: HELP | PING | "
            "BORROW <memberId> <bookId> | "
            "RETURN <memberId> <bookId> | QUIT";
    }

    if (command == "PING") {
        std::string extra;

        if (input >> extra) {
            return "ERROR PING takes no arguments";
        }

        return "PONG";
    }

    if (command == "QUIT") {
        return "BYE";
    }

    if (command == "BORROW" || command == "RETURN") {
        int memberId;
        int bookId;
        std::string extra;

        if (!(input >> memberId >> bookId) || (input >> extra)) {
            return command == "BORROW"
                ? "ERROR Usage: BORROW <memberId> <bookId>"
                : "ERROR Usage: RETURN <memberId> <bookId>";
        }

        if (memberId <= 0 || bookId <= 0) {
            return "ERROR memberId and bookId must be positive";
        }

        OperationResult result;

        if (command == "BORROW") {
            result = library_.borrowBook(memberId, bookId);
        } else {
            result = library_.returnBook(memberId, bookId);
        }

        return resultToString(result);
    }

    return "ERROR unknown command. Type HELP";
}


void NetworkServer::removeClient(SOCKET clientSocket) {
    bool foundClient = false;

    {
        std::lock_guard<std::mutex> lock(clientsMutex_);

        auto client = std::find(
            clientSockets_.begin(),
            clientSockets_.end(),
            clientSocket
        );

        if (client != clientSockets_.end()) {
            clientSockets_.erase(client);
            foundClient = true;
        }
    }

    // Close the socket only if this function removed its record.
    // This helps prevent accidental double-closing.
    if (foundClient) {
        shutdown(clientSocket, SD_BOTH);
        closesocket(clientSocket);
    }
}

