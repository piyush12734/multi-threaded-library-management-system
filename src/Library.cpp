#include "../include/Library.h"
#include <iostream>

using namespace std;

Library::Library(
    BookRepository& bookRepository,
    TransactionRepository& transactionRepository
)
    : bookRepository(bookRepository),
      transactionRepository(transactionRepository) {
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

    // Find the book from PostgreSQL
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

    // Check availability
    if (!book.isAvailable()) {
        cout << "Book is already borrowed." << endl;
        return false;
    }

    // Update book state in PostgreSQL
    bookRepository.updateAvailability(
        bookId,
        false
    );

    // Create transaction in PostgreSQL
    int transactionId =
        transactionRepository.add(
            memberId,
            bookId,
            TransactionType::BORROW
        );

    if (transactionId == -1) {
        cout << "Failed to create borrow transaction." << endl;

        // Roll back book availability
        bookRepository.updateAvailability(
            bookId,
            true
        );

        return false;
    }

    cout << "Book borrowed successfully." << endl;

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

    // Find the book from PostgreSQL
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

    // Check availability
    if (book.isAvailable()) {
        cout << "Book is already available." << endl;
        return false;
    }

    // Update book state in PostgreSQL
    bookRepository.updateAvailability(
        bookId,
        true
    );

    // Create transaction in PostgreSQL
    int transactionId =
        transactionRepository.add(
            memberId,
            bookId,
            TransactionType::RETURN
        );

    if (transactionId == -1) {
        cout << "Failed to create return transaction." << endl;

        // Restore previous state
        bookRepository.updateAvailability(
            bookId,
            false
        );

        return false;
    }

    cout << "Book returned successfully." << endl;

    return true;
}

void Library::displayBooks() {

    cout << "\n--- Books ---\n";

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

void Library::displayTransactions() {

    cout << "\n--- Transactions ---\n";

    vector<Transaction> transactions =
        transactionRepository.getAll();

    for (const Transaction& transaction :
         transactions) {

        transaction.display();
        cout << endl;
    }
}