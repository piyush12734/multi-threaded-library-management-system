#include "../include/Library.h"
#include <iostream>

using namespace std;

Library::Library(
    BookRepository& bookRepository,
    TransactionRepository& transactionRepository,
    MemberRepository& memberRepository
)
    : bookRepository(bookRepository),
      transactionRepository(transactionRepository),
      memberRepository(memberRepository) {
}

void Library::addBook(const Book& book) {

    bookRepository.add(book);
}

void Library::addMember(const Member& member) {

    if (memberRepository.exists(member.getId())) {

        cout << "Member already exists." << endl;
        return;
    }

    memberRepository.add(member);
}

bool Library::borrowBook(int memberId, int bookId) {

    // Check member in PostgreSQL
    if (!memberRepository.exists(memberId)) {

        cout << "Member not found." << endl;
        return false;
    }

    // Find book in PostgreSQL
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

    // Update book in PostgreSQL
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

        cout << "Failed to create borrow transaction."
             << endl;

        // Restore book state
        bookRepository.updateAvailability(
            bookId,
            true
        );

        return false;
    }

    cout << "Book borrowed successfully."
         << endl;

    return true;
}

bool Library::returnBook(int memberId, int bookId) {

    // Check member in PostgreSQL
    if (!memberRepository.exists(memberId)) {

        cout << "Member not found." << endl;
        return false;
    }

    // Find book in PostgreSQL
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

        cout << "Book is already available."
             << endl;

        return false;
    }

    // Update book in PostgreSQL
    bookRepository.updateAvailability(
        bookId,
        true
    );

    // Create return transaction
    int transactionId =
        transactionRepository.add(
            memberId,
            bookId,
            TransactionType::RETURN
        );

    if (transactionId == -1) {

        cout << "Failed to create return transaction."
             << endl;

        // Restore previous state
        bookRepository.updateAvailability(
            bookId,
            false
        );

        return false;
    }

    cout << "Book returned successfully."
         << endl;

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

void Library::displayMembers() {

    cout << "\n--- Members ---\n";

    vector<Member> members =
        memberRepository.getAll();

    for (const Member& member : members) {

        cout << "ID: " << member.getId() << endl;
        cout << "Name: " << member.getName() << endl;
        cout << "Email: " << member.getEmail() << endl;

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