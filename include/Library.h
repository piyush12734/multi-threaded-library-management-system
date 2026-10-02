#ifndef LIBRARY_H
#define LIBRARY_H

#include <vector>

#include "Book.h"
#include "Member.h"
#include "Transaction.h"
#include "BookRepository.h"

using namespace std;

class Library {
private:
    BookRepository& bookRepository;

    // These remain in memory for now.
    vector<Member> members;
    vector<Transaction> transactions;

public:
    Library(BookRepository& bookRepository);

    void addBook(const Book& book);
    void addMember(const Member& member);

    bool borrowBook(int memberId, int bookId);
    bool returnBook(int memberId, int bookId);

    void displayBooks();
    void displayTransactions() const;
};

#endif