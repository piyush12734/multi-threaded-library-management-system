#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <mutex>
#include <pqxx/pqxx>

#include "Book.h"
#include "Member.h"
#include "Transaction.h"

using namespace std;

class Database {
private:
    pqxx::connection connection;

    // Protects the shared PostgreSQL connection
    mutex connectionMutex;

public:
    Database(const string& connectionString);

    bool testConnection();

    // ---------------- Book Operations ----------------

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

    // ---------------- Member Operations ----------------

    void insertMember(
        int id,
        const string& name,
        const string& email
    );

    vector<Member> getMembers();

    bool memberExists(int id);

    // ---------------- Transaction Operations ----------------

    int insertTransaction(
        int memberId,
        int bookId,
        TransactionType type
    );

    vector<Transaction> getTransactions();

    // ---------------- Atomic Library Operations ----------------

    bool borrowBook(
        int memberId,
        int bookId
    );

    bool returnBook(
        int memberId,
        int bookId
    );
};

#endif