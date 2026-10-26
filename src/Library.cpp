#include "../include/Library.h"
#include <iostream>
#include <algorithm>
#include <cctype>

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


OperationResult Library::addBook(const Book& book) {
    return bookRepository.add(book);
}


void Library::addMember(const Member& member) {

    if (memberRepository.exists(member.getId())) {
        cout << "Member already exists." << endl;
        return;
    }

    memberRepository.add(member);
}

OperationResult Library::borrowBook(
    int memberId,
    int bookId
) {
    return transactionRepository.borrowBook(
        memberId,
        bookId
    );
}

OperationResult Library::returnBook(
    int memberId,
    int bookId
) {
    return transactionRepository.returnBook(
        memberId,
        bookId
    );
}


vector<Book> Library::getBooks() {
    return bookRepository.getAll();
}

vector<Book> Library::searchBooks(const string& keyword) {
    vector<Book> results;

    if (keyword.empty()) {
        return results;
    }

    // Convert the keyword to lowercase.
    string searchKeyword = keyword;

    transform(
        searchKeyword.begin(),
        searchKeyword.end(),
        searchKeyword.begin(),
        [](unsigned char ch) {
            return static_cast<char>(tolower(ch));
        }
    );

    // Retrieve books from the existing repository.
    vector<Book> books = bookRepository.getAll();

    for (const Book& book : books) {
        string searchableText =
            book.getTitle() + " " +
            book.getAuthor() + " " +
            book.getIsbn();

        // Convert the title, author, and ISBN to lowercase.
        transform(
            searchableText.begin(),
            searchableText.end(),
            searchableText.begin(),
            [](unsigned char ch) {
                return static_cast<char>(tolower(ch));
            }
        );

        // Match if the keyword appears in any searchable field.
        if (searchableText.find(searchKeyword) != string::npos) {
            results.push_back(book);
        }
    }

    return results;
}


void Library::displayBooks() {

    cout << "\n--- Books ---\n";

    vector<Book> books = bookRepository.getAll();

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

void Library::displayMembers() {

    cout << "\n--- Members ---\n";

    vector<Member> members = memberRepository.getAll();

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

    for (const Transaction& transaction : transactions) {
        transaction.display();
        cout << endl;
    }
}