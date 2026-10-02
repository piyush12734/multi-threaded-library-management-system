#include "../include/Library.h"
#include <iostream>

using namespace std;

Library::Library(BookRepository& bookRepository)
    : bookRepository(bookRepository) {
}

void Library::addBook(const Book& book) {
    bookRepository.add(book);
}

void Library::addMember(const Member& member) {
    members.push_back(member);
}

bool Library::borrowBook(int memberId, int bookId) {

    // Check whether the member exists
    bool memberExists = false;

    for (const Member& member : members) {

        if (member.getId() == memberId) {
            memberExists = true;
            break;
        }
    }

    if (!memberExists) {
        cout << "Member not found." << endl;
        return false;
    }

    // Find the book in PostgreSQL
    Book book(
        0,
        "",
        "",
        ""
    );

    if (!bookRepository.findById(bookId, book)) {
        cout << "Book not found." << endl;
        return false;
    }

    // Check whether the book is available
    if (!book.isAvailable()) {
        cout << "Book is already borrowed." << endl;
        return false;
    }

    // Change C++ object's state
    book.borrow();

    // IMPORTANT:
    // Persist the new state in PostgreSQL
    bookRepository.updateAvailability(
        bookId,
        false
    );

    // Create transaction
    int transactionId =
        transactions.size() + 1;

    transactions.emplace_back(
        transactionId,
        memberId,
        bookId,
        TransactionType::BORROW
    );

    return true;
}

bool Library::returnBook(int memberId, int bookId) {

    // Check whether the member exists
    bool memberExists = false;

    for (const Member& member : members) {

        if (member.getId() == memberId) {
            memberExists = true;
            break;
        }
    }

    if (!memberExists) {
        cout << "Member not found." << endl;
        return false;
    }

    // Find the book in PostgreSQL
    Book book(
        0,
        "",
        "",
        ""
    );

    if (!bookRepository.findById(bookId, book)) {
        cout << "Book not found." << endl;
        return false;
    }

    // Check whether the book is already available
    if (book.isAvailable()) {
        cout << "Book is already available." << endl;
        return false;
    }

    // Change C++ object's state
    book.returnBook();

    // IMPORTANT:
    // Persist the new state in PostgreSQL
    bookRepository.updateAvailability(
        bookId,
        true
    );

    // Create transaction
    int transactionId =
        transactions.size() + 1;

    transactions.emplace_back(
        transactionId,
        memberId,
        bookId,
        TransactionType::RETURN
    );

    return true;
}

void Library::displayBooks() {

    cout << "\n--- Books ---\n";

    // Read latest data from PostgreSQL
    vector<Book> books =
        bookRepository.getAll();

    for (const Book& book : books) {

        cout << "ID: " << book.getId() << endl;
        cout << "Title: " << book.getTitle() << endl;
        cout << "Author: " << book.getAuthor() << endl;

        cout << "Status: "
             << (book.isAvailable()
                     ? "Available"
                     : "Borrowed")
             << endl;

        cout << endl;
    }
}

void Library::displayTransactions() const {

    cout << "\n--- Transactions ---\n";

    for (const Transaction& transaction :
         transactions) {

        transaction.display();
        cout << endl;
    }
}