#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <pqxx/pqxx>

#include "Book.h"
#include "Member.h"
#include "Transaction.h"

using namespace std;

class Database {
private:
    pqxx::connection connection;

public:
    Database(const string& connectionString);

    bool testConnection();

    // Book operations
    void insertBook(
        int id,
        const string& title,
        const string& author,
        const string& isbn
    );

    vector<Book> getBooks();

    void updateBookAvailability(
        int bookId,
        bool available
    );

    // Member operations
    void insertMember(
        int id,
        const string& name,
        const string& email
    );

    // Transaction operations
    int insertTransaction(
        int memberId,
        int bookId,
        TransactionType type
    );

    vector<Transaction> getTransactions();
};

#endif