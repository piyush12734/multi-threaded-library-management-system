#include "../include/Book.h"
#include "../include/Member.h"
#include "../include/Admin.h"
#include "../include/Library.h"
#include "../include/Database.h"
#include "../include/BookRepository.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>

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


    // ---------------- Book Repository ----------------

    BookRepository bookRepository(database);


    // ---------------- Library ----------------

    Library library(bookRepository);


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


    // ---------------- Add Member ----------------

    library.addMember(member);


    // ---------------- Display Books From Database ----------------

    cout << "\n===== BOOKS FROM DATABASE =====\n";

    library.displayBooks();


    // ---------------- Borrow Book ----------------

    cout << "===== BORROW BOOK =====\n";

    library.borrowBook(101, 1);


    // Display database-backed book status
    cout << "\n===== BOOKS AFTER BORROW =====\n";

    library.displayBooks();


    // ---------------- Try Borrowing Same Book Again ----------------

    cout << "===== BORROW SAME BOOK AGAIN =====\n";

    library.borrowBook(101, 1);


    // ---------------- Return Book ----------------

    cout << "\n===== RETURN BOOK =====\n";

    library.returnBook(101, 1);


    // Display database-backed book status
    cout << "\n===== BOOKS AFTER RETURN =====\n";

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