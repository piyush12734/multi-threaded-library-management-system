#include "../include/TransactionRepository.h"

using namespace std;

TransactionRepository::TransactionRepository(
    Database& database
)
    : database(database) {
}

int TransactionRepository::add(
    int memberId,
    int bookId,
    TransactionType type
) {

    return database.insertTransaction(
        memberId,
        bookId,
        type
    );
}

vector<Transaction> TransactionRepository::getAll() {

    return database.getTransactions();
}

bool TransactionRepository::borrowBook(
    int memberId,
    int bookId
) {

    return database.borrowBook(
        memberId,
        bookId
    );
}

bool TransactionRepository::returnBook(
    int memberId,
    int bookId
) {

    return database.returnBook(
        memberId,
        bookId
    );
}