#ifndef LIBRARY_H
#define LIBRARY_H

#include "Book.h"
#include "Member.h"
#include "Transaction.h"
#include "BookRepository.h"
#include "TransactionRepository.h"

using namespace std;

class Library {
private:
    BookRepository& bookRepository;
    TransactionRepository& transactionRepository;

    vector<Member> members;

public:
    Library(
        BookRepository& bookRepository,
        TransactionRepository& transactionRepository
    );

    void addBook(const Book& book);
    void addMember(const Member& member);

    bool borrowBook(int memberId, int bookId);
    bool returnBook(int memberId, int bookId);

    void displayBooks();
    void displayTransactions();
};

#endif