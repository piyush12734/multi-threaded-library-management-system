#include "../include/Book.h"
#include "../include/Member.h"
#include "../include/Admin.h"
#include "../include/Library.h"
#include <iostream>

using namespace std;

int main() {

    Library library;

    // Create books
    Book book1(
        1,
        "Clean Code",
        "Robert C. Martin",
        "9780132350884"
    );

    Book book2(
        2,
        "The Pragmatic Programmer",
        "Andrew Hunt",
        "9780135957059"
    );

    // Create member
    Member member(
        101,
        "Piyush",
        "piyush@gmail.com"
    );

    // Create admin
    Admin admin(
        1,
        "Library Admin",
        "admin@library.com"
    );

    // Add data to library
    library.addBook(book1);
    library.addBook(book2);
    library.addMember(member);

    cout << "===== INITIAL LIBRARY =====\n";
    library.displayBooks();

    // Borrow
    cout << "===== BORROW BOOK =====\n";
    library.borrowBook(101, 1);

    library.displayBooks();

    // Try borrowing the same book again
    cout << "===== BORROW SAME BOOK AGAIN =====\n";
    library.borrowBook(101, 1);

    // Return
    cout << "\n===== RETURN BOOK =====\n";
    library.returnBook(101, 1);

    library.displayBooks();

    // Display transactions
    library.displayTransactions();

    // Runtime polymorphism
    cout << "===== RUNTIME POLYMORPHISM =====\n";

    User* user1 = &member;
    User* user2 = &admin;

    user1->displayRole();
    user2->displayRole();

    return 0;
}