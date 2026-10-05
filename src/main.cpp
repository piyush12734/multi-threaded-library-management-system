#include "../include/Book.h"
#include "../include/Member.h"
#include "../include/Admin.h"
#include "../include/Library.h"
#include "../include/Database.h"
#include "../include/BookRepository.h"
#include "../include/TransactionRepository.h"
#include "../include/MemberRepository.h"
#include "../include/ThreadPool.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <chrono>
#include <thread>

using namespace std;

int main() {

    cout << "\n===== THREAD POOL TEST =====\n";

ThreadPool pool(3);

for (int i = 1; i <= 6; ++i) {

    pool.enqueue([i]() {

        cout << "Task "
             << i
             << " executed by thread "
             << this_thread::get_id()
             << endl;
    });
}

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


    // ---------------- Repositories ----------------

    BookRepository bookRepository(database);

    TransactionRepository transactionRepository(database);

    MemberRepository memberRepository(database);


    // ---------------- Library ----------------

    Library library(
        bookRepository,
        transactionRepository,
        memberRepository
    );


    // ---------------- Create Member ----------------

    Member member(
        101,
        "Piyush",
        "piyush@gmail.com"
    );


    // ---------------- Add Member ----------------

    library.addMember(member);


    // ---------------- Create Admin ----------------

    Admin admin(
        1,
        "Library Admin",
        "admin@library.com"
    );


    // ---------------- Display Books ----------------

    cout << "\n===== BOOKS FROM DATABASE =====\n";

    library.displayBooks();


    // ---------------- Display Members ----------------

    cout << "\n===== MEMBERS FROM DATABASE =====\n";

    library.displayMembers();


    // ---------------- Borrow Book ----------------

    cout << "\n===== BORROW BOOK =====\n";

    library.borrowBook(101, 1);


    // ---------------- Display After Borrow ----------------

    cout << "\n===== BOOKS AFTER BORROW =====\n";

    library.displayBooks();


    // ---------------- Try Borrowing Same Book ----------------

    cout << "===== BORROW SAME BOOK AGAIN =====\n";

    library.borrowBook(101, 1);


    // ---------------- Return Book ----------------

    cout << "\n===== RETURN BOOK =====\n";

    library.returnBook(101, 1);


    // ---------------- Display After Return ----------------

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