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