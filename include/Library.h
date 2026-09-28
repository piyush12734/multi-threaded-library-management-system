#ifndef LIBRARY_H
#define LIBRARY_H

#include <vector>
#include "Book.h"
#include "Member.h"
#include "Transaction.h"

using namespace std;

class Library {
private:
    vector<Book> books;
    vector<Member> members;
    vector<Transaction> transactions;

public:
    void addBook(const Book& book);
    void addMember(const Member& member);

    bool borrowBook(int memberId, int bookId);
    bool returnBook(int memberId, int bookId);

    void displayBooks() const;
    void displayTransactions() const;
};

#endif