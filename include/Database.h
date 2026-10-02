#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <pqxx/pqxx>
#include "Book.h"

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

    vector<Book> getBooks();
    void updateBookAvailability(int bookId, bool available);
};

#endif