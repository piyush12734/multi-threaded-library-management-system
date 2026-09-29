#include "../include/Book.h"
#include "../include/Member.h"
#include "../include/Admin.h"
#include "../include/Library.h"
#include "../include/Database.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

int main() {

    // ---------------- Database Connection ----------------

    const char* password = getenv("LIBRARY_DB_PASSWORD");

    if (password == nullptr) {
        cerr << "LIBRARY_DB_PASSWORD is not set." << endl;
        return 1;
    }

    string connectionString =
        "host=localhost "
        "port=5432 "
        "dbname=library_management "
        "user=postgres "
        "password=" + string(password);

    Database database(connectionString);

    if (database.testConnection()) {
        cout << "Database connection successful!" << endl;
    } else {
        cout << "Database connection failed!" << endl;
        return 1;
    }
    database.insertBook(
    1,
    "Clean Code",
    "Robert C. Martin",
    "9780132350884");
    
    database.insertBook(
    2,
    "The Pragmatic Programmer",
    "Andrew Hunt",
    "9780135957059");


    // ---------------- Library ----------------

    Library library;


    // ---------------- Create Books ----------------

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


    // ---------------- Create Member ----------------

    Member member(
        101,
        "Piyush",
        "piyush@gmail.com"
    );


    // ---------------- Create Admin ----------------

    Admin admin(
        1,
        "Library Admin",
        "admin@library.com"
    );


    // ---------------- Add Data to Library ----------------

    library.addBook(book1);
    library.addBook(book2);
    library.addMember(member);


    // ---------------- Display Books ----------------

    cout << "\n===== INITIAL LIBRARY =====\n";

    library.displayBooks();


    // ---------------- Borrow Book ----------------

    cout << "===== BORROW BOOK =====\n";

    library.borrowBook(101, 1);

    library.displayBooks();


    // ---------------- Try Borrowing Again ----------------

    cout << "===== BORROW SAME BOOK AGAIN =====\n";

    library.borrowBook(101, 1);


    // ---------------- Return Book ----------------

    cout << "\n===== RETURN BOOK =====\n";

    library.returnBook(101, 1);

    library.displayBooks();


    // ---------------- Display Transactions ----------------

    library.displayTransactions();


    // ---------------- Runtime Polymorphism ----------------

    cout << "===== RUNTIME POLYMORPHISM =====\n";

    User* user1 = &member;
    User* user2 = &admin;

    user1->displayRole();
    user2->displayRole();


    return 0;
}