
#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>
#include <vector>

#include "Book.h"
#include "Member.h"
#include "Transaction.h"
#include "BookRepository.h"
#include "TransactionRepository.h"
#include "MemberRepository.h"
#include "OperationResult.h"

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

    OperationResult addBook(const Book& book);
    void addMember(const Member& member);

    OperationResult borrowBook(
        int memberId,
        int bookId
    );

    OperationResult returnBook(
        int memberId,
        int bookId
    );

    // Return book data to the caller instead of only printing it.
    vector<Book> getBooks();

    // Search book titles, authors, and ISBNs.
    vector<Book> searchBooks(const string& keyword);

    void displayBooks();
    void displayMembers();
    void displayTransactions();
};

#endif
