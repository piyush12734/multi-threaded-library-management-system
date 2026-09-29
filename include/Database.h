#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <pqxx/pqxx>

using namespace std;

class Database {
private:
    pqxx::connection connection;

public:
    Database(const string& connectionString);

    bool testConnection();

    void insertBook(
        int id,
        const string& title,
        const string& author,
        const string& isbn
    );
};

#endif