#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

using namespace std;

enum class TransactionType {
    BORROW,
    RETURN
};

class Transaction {
private:
    int transactionId;
    int memberId;
    int bookId;
    TransactionType type;

public:
    Transaction(
        int transactionId,
        int memberId,
        int bookId,
        TransactionType type
    );

    int getTransactionId() const;
    int getMemberId() const;
    int getBookId() const;
    TransactionType getType() const;

    void display() const;
};

#endif