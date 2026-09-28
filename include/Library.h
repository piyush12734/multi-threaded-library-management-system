#ifndef LIBRARY_H
#define LIBRARY_H

#include <vector>
#include "Book.h"
#include "Member.h"
#include "Transaction.h"
#include "Repository.h"

using namespace std;

class Library {
private:
    Repository<Book> bookRepository;
    Repository<Member> memberRepository;
    Repository<Transaction> transactionRepository;

public:
    void addBook(const Book& book);
    void addMember(const Member& member);

    bool borrowBook(int memberId, int bookId);
    bool returnBook(int memberId, int bookId);

    void displayBooks() const;
    void displayTransactions() const;
};

#endif