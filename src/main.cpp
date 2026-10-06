#include "../include/Book.h"
#include "../include/Member.h"
#include "../include/Admin.h"
#include "../include/Library.h"
#include "../include/Database.h"
#include "../include/BookRepository.h"
#include "../include/TransactionRepository.h"
#include "../include/MemberRepository.h"
#include "../include/ThreadPool.h"
#include "../include/Logger.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <thread>

using namespace std;

int main() {

    // ============================================================
    // THREAD POOL TEST
    // ============================================================

    {
    cout << "\n===== THREAD POOL TEST =====\n";

    Logger logger;
    ThreadPool pool(3);

    for (int i = 1; i <= 6; ++i) {

        pool.enqueue([i, &logger]() {

            stringstream message;

            message << "Task "
                    << i
                    << " executed by thread "
                    << this_thread::get_id();

            logger.log(message.str());
        });
    }
}

    // ============================================================
    // DATABASE CONNECTION
    // ============================================================

    const char* password =
        getenv("LIBRARY_DB_PASSWORD");

    if (password == nullptr) {

        cerr << "LIBRARY_DB_PASSWORD is not set."
             << endl;

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

        cout << "\nDatabase connection successful!"
             << endl;

    } else {

        cout << "\nDatabase connection failed!"
             << endl;

        return 1;
    }


    // ============================================================
    // REPOSITORIES
    // ============================================================

    BookRepository bookRepository(database);

    TransactionRepository transactionRepository(database);

    MemberRepository memberRepository(database);


    // ============================================================
    // LIBRARY
    // ============================================================

    Library library(
        bookRepository,
        transactionRepository,
        memberRepository
    );


    // ============================================================
    // CREATE MEMBER
    // ============================================================

    Member member(
        101,
        "Piyush",
        "piyush@gmail.com"
    );


    // ============================================================
    // ADD MEMBER
    // ============================================================

    library.addMember(member);


    // ============================================================
// CONCURRENT BORROW TEST
// ============================================================

cout << "\n===== CONCURRENT BORROW TEST =====\n";

Logger logger;

{
    ThreadPool pool(2);

    pool.enqueue([&library, &logger]() {

        bool success =
            library.borrowBook(101, 1);

        stringstream message;

        message << "Thread "
                << this_thread::get_id()
                << " borrow result: "
                << (success ? "SUCCESS" : "FAILED");

        logger.log(message.str());
    });

    

    pool.enqueue([&library, &logger]() {

        bool success =
            library.borrowBook(101, 1);

        stringstream message;

        message << "Thread "
                << this_thread::get_id()
                << " borrow result: "
                << (success ? "SUCCESS" : "FAILED");

        logger.log(message.str());
    });
}

cout << "\n===== BOOK AFTER CONCURRENT BORROW =====\n";

library.displayBooks();

cout << "\n===== RETURN BOOK =====\n";

library.returnBook(101, 1);


    // ============================================================
    // CREATE ADMIN
    // ============================================================

    Admin admin(
        1,
        "Library Admin",
        "admin@library.com"
    );


    // ============================================================
    // DISPLAY BOOKS
    // ============================================================

    cout << "\n===== BOOKS FROM DATABASE =====\n";

    library.displayBooks();


    // ============================================================
    // DISPLAY MEMBERS
    // ============================================================

    cout << "\n===== MEMBERS FROM DATABASE =====\n";

    library.displayMembers();


    // ============================================================
    // BORROW BOOK
    // ============================================================

    cout << "\n===== BORROW BOOK =====\n";

    library.borrowBook(101, 1);


    // ============================================================
    // DISPLAY BOOKS AFTER BORROW
    // ============================================================

    cout << "\n===== BOOKS AFTER BORROW =====\n";

    library.displayBooks();


    // ============================================================
    // TRY TO BORROW SAME BOOK AGAIN
    // ============================================================

    cout << "===== BORROW SAME BOOK AGAIN =====\n";

    library.borrowBook(101, 1);


    // ============================================================
    // RETURN BOOK
    // ============================================================

    cout << "\n===== RETURN BOOK =====\n";

    library.returnBook(101, 1);


    // ============================================================
    // DISPLAY BOOKS AFTER RETURN
    // ============================================================

    cout << "\n===== BOOKS AFTER RETURN =====\n";

    library.displayBooks();


    // ============================================================
    // DISPLAY TRANSACTIONS
    // ============================================================

    library.displayTransactions();


    // ============================================================
    // RUNTIME POLYMORPHISM
    // ============================================================

    cout << "===== RUNTIME POLYMORPHISM =====\n";

    User* user1 = &member;
    User* user2 = &admin;

    user1->displayRole();
    user2->displayRole();


    return 0;
}