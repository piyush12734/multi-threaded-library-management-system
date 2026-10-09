#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <condition_variable>
#include <cstddef>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include <stdexcept>

#include <pqxx/pqxx>

using namespace std;

class ConnectionPool {
private:
    // Returns a connection to the pool.
    void release(pqxx::connection* connection);

public:
    // RAII wrapper: automatically returns a connection
    // when the lease goes out of scope.
    class ConnectionLease {
    private:
        ConnectionPool* pool = nullptr;
        pqxx::connection* connection = nullptr;

        void returnConnection() noexcept;

    public:
        ConnectionLease() = default;

        ConnectionLease(
            ConnectionPool* pool,
            pqxx::connection* connection
        ) noexcept;

        // A connection lease must not be copied.
        ConnectionLease(const ConnectionLease&) = delete;

        ConnectionLease& operator=(
            const ConnectionLease&
        ) = delete;

        // Moving transfers responsibility for the connection.
        ConnectionLease(ConnectionLease&& other) noexcept;

        ConnectionLease& operator=(
            ConnectionLease&& other
        ) noexcept;

        ~ConnectionLease();

        pqxx::connection& get() const;

        pqxx::connection& operator*() const;

        pqxx::connection* operator->() const;
    };

private:
    // Owns all PostgreSQL connections.
    vector<unique_ptr<pqxx::connection>> connections;

    // Contains pointers to currently available connections.
    queue<pqxx::connection*> availableConnections;

    // Protects the pool's available-connection queue.
    mutex poolMutex;

    // Wakes a thread when a connection becomes available.
    condition_variable availableCondition;

public:
    // Creates poolSize PostgreSQL connections.
    ConnectionPool(
        const string& connectionString,
        size_t poolSize = 4
    );

    // Waits until a connection is available.
    ConnectionLease acquire();
};

#endif