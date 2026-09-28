#include "../include/Library.h"
#include <iostream>

using namespace std;

void Library::addBook(const Book& book) {
    bookRepository.add(book);
}

void Library::addMember(const Member& member) {
    memberRepository.add(member);
}

bool Library::borrowBook(int memberId, int bookId) {

    // Check whether the member exists
    Member* member = memberRepository.findById(memberId);

    if (member == nullptr) {
        cout << "Member not found.\n";
        return false;
    }

    // Find the book
    Book* book = bookRepository.findById(bookId);

    if (book == nullptr) {
        cout << "Book not found.\n";
        return false;
    }

    // Check availability
    if (!book->isAvailable()) {
        cout << "Book is already borrowed.\n";
        return false;
    }

    // Borrow the book
    book->borrow();

    // Create transaction
    int transactionId =
        transactionRepository.getAll().size() + 1;

    transactionRepository.add(
        Transaction(
            transactionId,
            memberId,
            bookId,
            TransactionType::BORROW
        )
    );

    return true;
}

bool Library::returnBook(int memberId, int bookId) {

    // Check whether the member exists
    Member* member = memberRepository.findById(memberId);

    if (member == nullptr) {
        cout << "Member not found.\n";
        return false;
    }

    // Find the book
    Book* book = bookRepository.findById(bookId);

    if (book == nullptr) {
        cout << "Book not found.\n";
        return false;
    }

    // Check whether the book is already available
    if (book->isAvailable()) {
        cout << "Book is already available.\n";
        return false;
    }

    // Return the book
    book->returnBook();

    // Create transaction
    int transactionId =
        transactionRepository.getAll().size() + 1;

    transactionRepository.add(
        Transaction(
            transactionId,
            memberId,
            bookId,
            TransactionType::RETURN
        )
    );

    return true;
}

void Library::displayBooks() const {

    cout << "\n--- Books ---\n";

    for (const Book& book : bookRepository.getAll()) {

        cout << "ID: " << book.getId() << endl;
        cout << "Title: " << book.getTitle() << endl;
        cout << "Author: " << book.getAuthor() << endl;

        cout << "Status: "
             << (book.isAvailable() ? "Available" : "Borrowed")
             << endl;

        cout << endl;
    }
}

void Library::displayTransactions() const {

    cout << "\n--- Transactions ---\n";

    for (const Transaction& transaction :
         transactionRepository.getAll()) {

        transaction.display();
        cout << endl;
    }
}