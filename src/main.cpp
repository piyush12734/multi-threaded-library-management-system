#include "NetworkServer.h"
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
#include "../include/OperationResult.h"


#include <atomic>
#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <thread>

using namespace std;


string operationResultToString(OperationResult result) {

    switch (result) {

        case OperationResult::Success:
            return "SUCCESS";

        case OperationResult::MemberNotFound:
            return "FAILED - member not found";

        case OperationResult::BookNotFound:
            return "FAILED - book not found";

        case OperationResult::BookNotAvailable:
            return "FAILED - book already borrowed";

        case OperationResult::BookAlreadyAvailable:
            return "FAILED - book is already available";

        case OperationResult::DatabaseError:
            return "FAILED - database error";
    }

    return "FAILED - unknown result";
}

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

                OperationResult result = library.borrowBook(101, 1);

                if (result == OperationResult::Success) {
                    successfulBorrows.fetch_add(1);
                }

                stringstream message;

                message << "Request " << request
                        << " | Thread "
                        << this_thread::get_id()
                        << " | Result: "
                        << operationResultToString(result);

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
    OperationResult result = library.returnBook(101, 1);
    cout << operationResultToString(result)<< endl;
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


    cout << "\n===== OPERATION RESULT TESTS =====\n";

    // Test 1: Member does not exist
    cout << "Missing member: "
        << operationResultToString(library.borrowBook(99999, 1))
        << '\n';

    // Test 2: Book does not exist during borrowing
    cout << "Missing book (borrow): "
        << operationResultToString(library.borrowBook(101, 99999))
        << '\n';

    // Test 3: Book does not exist during returning
    cout << "Missing book (return): "
        << operationResultToString(library.returnBook(101, 99999))
        << '\n';

    // Test 4: Book is already available
    cout << "Return available book: "
        << operationResultToString(library.returnBook(101, 1))
        << '\n';
    
    
    // ============================================================
    // NORMAL BORROW TEST
    // ============================================================

    cout << "\n===== BORROW BOOK =====\n";

    OperationResult borrowed = library.borrowBook(101, 1);

    cout << operationResultToString(borrowed) << endl;


    // ============================================================
    // DISPLAY BOOKS AFTER BORROW
    // ============================================================

    cout << "\n===== BOOKS AFTER BORROW =====\n";

    library.displayBooks();


    // ============================================================
    // TRY TO BORROW THE SAME BOOK AGAIN
    // ============================================================

    cout << "\n===== BORROW SAME BOOK AGAIN =====\n";


    OperationResult borrowedAgain = library.borrowBook(101, 1);

    cout << operationResultToString(borrowedAgain) << endl;


    // ============================================================
    // RETURN BOOK
    // ============================================================

    cout << "\n===== RETURN BOOK =====\n";

    OperationResult returned = library.returnBook(101, 1);

    cout << operationResultToString(returned) << endl;

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

    cout << "\n===== NETWORK SERVER =====\n";

        NetworkServer server(library, 54000);

        if (!server.start()) {
            cerr << "Failed to start network server.\n";
            return 1;
        }

        cout << "Server is running on port 54000.\n";
        cout << "Press Enter to stop the server...\n";

        cin.get();

        server.stop();


    return 0;
}
