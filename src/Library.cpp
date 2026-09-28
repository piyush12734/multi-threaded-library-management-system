#include "../include/Library.h"
#include <iostream>

using namespace std;

void Library::addBook(const Book& book) {
    books.push_back(book);
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
        cout << "Member not found.\n";
        return false;
    }

    // Find the book
    for (Book& book : books) {

        if (book.getId() == bookId) {

            // Check availability
            if (!book.isAvailable()) {
                cout << "Book is already borrowed.\n";
                return false;
            }

            // Borrow the book
            book.borrow();

            // Create transaction
            int transactionId = transactions.size() + 1;

            transactions.emplace_back(
                transactionId,
                memberId,
                bookId,
                TransactionType::BORROW
            );

            return true;
        }
    }

    cout << "Book not found.\n";
    return false;
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
        cout << "Member not found.\n";
        return false;
    }

    // Find the book
    for (Book& book : books) {

        if (book.getId() == bookId) {

            // Check whether the book is already available
            if (book.isAvailable()) {
                cout << "Book is already available.\n";
                return false;
            }

            // Return the book
            book.returnBook();

            // Create transaction
            int transactionId = transactions.size() + 1;

            transactions.emplace_back(
                transactionId,
                memberId,
                bookId,
                TransactionType::RETURN
            );

            return true;
        }
    }

    cout << "Book not found.\n";
    return false;
}

void Library::displayBooks() const {

    cout << "\n--- Books ---\n";

    for (const Book& book : books) {
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

    for (const Transaction& transaction : transactions) {
        transaction.display();
        cout << endl;
    }
}