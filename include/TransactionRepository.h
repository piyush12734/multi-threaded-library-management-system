#ifndef TRANSACTION_REPOSITORY_H
#define TRANSACTION_REPOSITORY_H

#include <vector>

#include "Transaction.h"
#include "Database.h"

using namespace std;

class TransactionRepository {
private:
    Database& database;

public:
    TransactionRepository(Database& database);

    int add(
        int memberId,
        int bookId,
        TransactionType type
    );

    vector<Transaction> getAll();

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