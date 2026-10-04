#ifndef LIBRARY_H
#define LIBRARY_H

#include <vector>

#include "Book.h"
#include "Member.h"
#include "Transaction.h"
#include "BookRepository.h"
#include "TransactionRepository.h"
#include "MemberRepository.h"

using namespace std;

class Library {
private:
    BookRepository& bookRepository;
    TransactionRepository& transactionRepository;
    MemberRepository& memberRepository;

public:
    Library(
        BookRepository& bookRepository,
        TransactionRepository& transactionRepository,
        MemberRepository& memberRepository
    );

    void addBook(const Book& book);
    void addMember(const Member& member);

    bool borrowBook(int memberId, int bookId);
    bool returnBook(int memberId, int bookId);

    void displayBooks();
    void displayMembers();
    void displayTransactions();
};

#endif