#include "../include/Transaction.h"
#include <iostream>

using namespace std;

Transaction::Transaction(
    int transactionId,
    int memberId,
    int bookId,
    TransactionType type
) {
    this->transactionId = transactionId;
    this->memberId = memberId;
    this->bookId = bookId;
    this->type = type;
}

int Transaction::getTransactionId() const {
    return transactionId;
}

int Transaction::getMemberId() const {
    return memberId;
}

int Transaction::getBookId() const {
    return bookId;
}

TransactionType Transaction::getType() const {
    return type;
}

void Transaction::display() const {

    cout << "Transaction ID: " << transactionId << endl;
    cout << "Member ID: " << memberId << endl;
    cout << "Book ID: " << bookId << endl;

    if (type == TransactionType::BORROW) {
        cout << "Type: BORROW" << endl;
    } else {
        cout << "Type: RETURN" << endl;
    }
}