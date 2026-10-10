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

#include <atomic>
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
    // CREATE AND ADD MEMBER
    // ============================================================

    Member member(
        101,
        "Piyush",
        "piyush@gmail.com"
    );

    library.addMember(member);


    // ============================================================
    // DAY 14: CONCURRENT BORROW STRESS TEST
    // ============================================================

    cout << "\n===== CONCURRENT BORROW STRESS TEST =====\n";

    constexpr int requestCount = 10;

    atomic<int> successfulBorrows{0};

    Logger borrowLogger;

    {
        // Eight worker threads and four database connections.
        ThreadPool pool(8);

        for (int request = 1;
             request <= requestCount;
             ++request) {

            pool.enqueue([
                request,
                &library,
                &borrowLogger,
                &successfulBorrows
            ]() {

                bool success =
                    library.borrowBook(101, 1);

                if (success) {
                    successfulBorrows.fetch_add(1);
                }

                stringstream message;

                message << "Request " << request
                        << " | Thread "
                        << this_thread::get_id()
                        << " | Result: "
                        << (success ? "SUCCESS" : "FAILED");

                borrowLogger.log(message.str());
            });
        }

        // The pool destructor waits for all tasks to finish.
    }

    cout << "\nSuccessful borrows: "
         << successfulBorrows.load()
         << " / "
         << requestCount
         << endl;


    // ============================================================
    // VERIFY BOOK AFTER STRESS TEST
    // ============================================================

    cout << "\n===== BOOK AFTER STRESS TEST =====\n";

    library.displayBooks();


    // Return Book 1 so the subsequent demonstration can run.
    // This assumes Book 1 was available before the stress test.
    cout << "\n===== RETURN BOOK AFTER STRESS TEST =====\n";


    if (successfulBorrows.load() > 0) {
    bool returned = library.returnBook(101, 1);

    cout << (returned
        ? "Book returned after stress test."
        : "Failed to return book after stress test.")
        << endl;
    }


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
    // NORMAL BORROW TEST
    // ============================================================

    cout << "\n===== BORROW BOOK =====\n";

    bool borrowed = library.borrowBook(101, 1);
    
    cout << (borrowed
    ? "Book borrowed successfully."
    : "Borrow failed: book unavailable or invalid member.")
    << endl;


    // ============================================================
    // DISPLAY BOOKS AFTER BORROW
    // ============================================================

    cout << "\n===== BOOKS AFTER BORROW =====\n";

    library.displayBooks();


    // ============================================================
    // TRY TO BORROW THE SAME BOOK AGAIN
    // ============================================================

    cout << "\n===== BORROW SAME BOOK AGAIN =====\n";


    bool borrowedAgain = library.borrowBook(101, 1);

    cout << (borrowedAgain
    ? "Book borrowed successfully."
    : "Second borrow correctly rejected.")
    << endl;


    // ============================================================
    // RETURN BOOK
    // ============================================================

    cout << "\n===== RETURN BOOK =====\n";

    bool returned = library.returnBook(101, 1);

    cout << (returned
    ? "Book returned successfully."
    : "Return failed: book unavailable or invalid member.")
    << endl;


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
