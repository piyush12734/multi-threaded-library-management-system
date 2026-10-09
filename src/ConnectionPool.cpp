#include "../include/ConnectionPool.h"

#include <iostream>
#include <utility>
#include <stdexcept>

using namespace std;


// ============================================================
// CONNECTION POOL CONSTRUCTOR
// ============================================================

ConnectionPool::ConnectionPool(
    const string& connectionString,
    size_t poolSize
) {

    if (poolSize == 0) {
        throw invalid_argument(
            "Connection pool size must be greater than zero."
        );
    }

    // Create the requested number of PostgreSQL connections.
    for (size_t i = 0; i < poolSize; ++i) {

        auto connection =
            make_unique<pqxx::connection>(connectionString);

        // Store ownership of the connection in the pool.
        connections.push_back(move(connection));

        // Make this connection available for workers.
        availableConnections.push(
            connections.back().get()
        );
    }

    cout << "Connection pool created with "
         << poolSize
         << " connections."
         << endl;
}


// ============================================================
// ACQUIRE A CONNECTION
// ============================================================

ConnectionPool::ConnectionLease
ConnectionPool::acquire() {

    unique_lock<mutex> lock(poolMutex);

    // Wait until a connection becomes available.
    availableCondition.wait(
        lock,
        [this]() {
            return !availableConnections.empty();
        }
    );

    // Take one available connection.
    pqxx::connection* connection =
        availableConnections.front();

    availableConnections.pop();

    // Return a lease representing temporary ownership.
    return ConnectionLease(this, connection);
}


// ============================================================
// RETURN A CONNECTION TO THE POOL
// ============================================================

void ConnectionPool::release(
    pqxx::connection* connection
) {

    {
        lock_guard<mutex> lock(poolMutex);

        availableConnections.push(connection);
    }

    // Wake one worker waiting for a connection.
    availableCondition.notify_one();
}


// ============================================================
// CONNECTION LEASE CONSTRUCTOR
// ============================================================

ConnectionPool::ConnectionLease::ConnectionLease(
    ConnectionPool* pool,
    pqxx::connection* connection
) noexcept
    : pool(pool),
      connection(connection) {
}


// ============================================================
// MOVE CONSTRUCTOR
// ============================================================

ConnectionPool::ConnectionLease::ConnectionLease(
    ConnectionLease&& other
) noexcept
    : pool(exchange(other.pool, nullptr)),
      connection(exchange(other.connection, nullptr)) {
}


// ============================================================
// MOVE ASSIGNMENT
// ============================================================

ConnectionPool::ConnectionLease&
ConnectionPool::ConnectionLease::operator=(
    ConnectionLease&& other
) noexcept {

    if (this != &other) {

        returnConnection();

        pool = exchange(other.pool, nullptr);

        connection =
            exchange(other.connection, nullptr);
    }

    return *this;
}


// ============================================================
// RETURN LEASED CONNECTION
// ============================================================

void ConnectionPool::ConnectionLease::returnConnection()
    noexcept {

    if (pool != nullptr && connection != nullptr) {

        try {
            pool->release(connection);
        } catch (...) {
            // A destructor must not propagate exceptions.
            // If returning fails, the pool still owns the
            // underlying connection.
        }
    }

    pool = nullptr;
    connection = nullptr;
}


// ============================================================
// CONNECTION LEASE DESTRUCTOR
// ============================================================

ConnectionPool::ConnectionLease::~ConnectionLease() {

    returnConnection();
}


// ============================================================
// ACCESS THE CONNECTION
// ============================================================

pqxx::connection&
ConnectionPool::ConnectionLease::get() const {

    if (connection == nullptr) {
        throw logic_error(
            "Connection lease does not own a connection."
        );
    }

    return *connection;
}


pqxx::connection&
ConnectionPool::ConnectionLease::operator*() const {

    return get();
}


pqxx::connection*
ConnectionPool::ConnectionLease::operator->() const {

    return &get();
}