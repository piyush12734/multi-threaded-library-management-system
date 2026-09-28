#include "../include/Book.h"
#include "../include/Member.h"
#include "../include/Admin.h"
#include "../include/Transaction.h"
#include <iostream>

using namespace std;

int main() {

    // ---------------- Book ----------------
    Book book(
        1,
        "Clean Code",
        "Robert C. Martin",
        "9780132350884"
    );

    cout << "Book: " << book.getTitle() << endl;
    cout << "Author: " << book.getAuthor() << endl;

    cout << "\nAvailability: "
         << (book.isAvailable() ? "Available" : "Not Available")
         << endl;


    // ---------------- Member ----------------
    Member member(
        101,
        "Piyush",
        "piyush@gmail.com"
    );

    cout << "\n--- Member ---\n";
    cout << "ID: " << member.getId() << endl;
    cout << "Name: " << member.getName() << endl;
    cout << "Email: " << member.getEmail() << endl;

    member.displayRole();


    // ---------------- Admin ----------------
    Admin admin(
        1,
        "Library Admin",
        "admin@library.com"
    );

    cout << "\n--- Admin ---\n";
    cout << "ID: " << admin.getId() << endl;
    cout << "Name: " << admin.getName() << endl;
    cout << "Email: " << admin.getEmail() << endl;

    admin.displayRole();


    // ---------------- Borrow Book ----------------
    cout << "\n--- Borrowing Book ---\n";

    book.borrow();

    cout << "Availability: "
         << (book.isAvailable() ? "Available" : "Not Available")
         << endl;


    // ---------------- Transaction ----------------
    Transaction transaction(
        1,
        member.getId(),
        book.getId(),
        TransactionType::BORROW
    );

    cout << "\n--- Transaction ---\n";
    transaction.display();


    // ---------------- Runtime Polymorphism ----------------
    cout << "\n--- Runtime Polymorphism ---\n";

    User* user1 = &member;
    User* user2 = &admin;

    user1->displayRole();
    user2->displayRole();

    return 0;
}