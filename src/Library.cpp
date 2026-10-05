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

bool Library::borrowBook(
    int memberId,
    int bookId
) {

    bool success =
        transactionRepository.borrowBook(
            memberId,
            bookId
        );

    if (success) {

        cout << "Book borrowed successfully."
             << endl;

        return true;
    }

    return false;
}

bool Library::returnBook(
    int memberId,
    int bookId
) {

    bool success =
        transactionRepository.returnBook(
            memberId,
            bookId
        );

    if (success) {

        cout << "Book returned successfully."
             << endl;

        return true;
    }

    return false;
}

void Library::displayBooks() {

    cout << "\n--- Books ---\n";

    vector<Book> books =
        bookRepository.getAll();

    for (const Book& book : books) {

        cout << "ID: "
             << book.getId()
             << endl;

        cout << "Title: "
             << book.getTitle()
             << endl;

        cout << "Author: "
             << book.getAuthor()
             << endl;

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

        cout << "ID: "
             << member.getId()
             << endl;

        cout << "Name: "
             << member.getName()
             << endl;

        cout << "Email: "
             << member.getEmail()
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