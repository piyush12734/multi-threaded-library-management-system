#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>

#include "Book.h"
#include "Member.h"
#include "Transaction.h"
#include "ConnectionPool.h"
#include "OperationResult.h"

using namespace std;

class Database {
private:
    // Manages reusable PostgreSQL connections.
    ConnectionPool connectionPool;

public:
    Database(const string& connectionString);

    bool testConnection();

    // ---------------- Book Operations ----------------

    OperationResult insertBook(
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

    OperationResult borrowBook(
        int memberId,
        int bookId
    );

    OperationResult returnBook(
        int memberId,
        int bookId
    );
};

#endif