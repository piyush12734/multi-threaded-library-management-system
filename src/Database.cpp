#include "../include/Database.h"
#include <iostream>

using namespace std;

// ============================================================
// CONSTRUCTOR
// ============================================================

Database::Database(const string& connectionString)
    : connectionPool(connectionString, 4) {
}


// ============================================================
// TEST CONNECTION
// ============================================================

bool Database::testConnection() {

    try {
        auto lease = connectionPool.acquire();

        pqxx::work transaction(lease.get());

        pqxx::result result =
            transaction.exec("SELECT 1");

        transaction.commit();

        return !result.empty() &&
               result[0][0].as<int>() == 1;

    } catch (const exception& e) {

        cerr << "Database error: "
             << e.what()
             << endl;

        return false;
    }
}


// ============================================================
// BOOK OPERATIONS
// ============================================================

void Database::insertBook(
    int id,
    const string& title,
    const string& author,
    const string& isbn
) {

    try {
        auto lease = connectionPool.acquire();

        pqxx::work transaction(lease.get());

        transaction.exec(
            "INSERT INTO books "
            "(id, title, author, isbn, available) "
            "VALUES ($1, $2, $3, $4, TRUE)",
            pqxx::params{
                id,
                title,
                author,
                isbn
            }
        );

        transaction.commit();

        cout << "Book inserted into database successfully."
             << endl;

    } catch (const exception& e) {

        cerr << "Failed to insert book: "
             << e.what()
             << endl;
    }
}


vector<Book> Database::getBooks() {

    vector<Book> books;

    try {
        auto lease = connectionPool.acquire();

        pqxx::work transaction(lease.get());

        pqxx::result result =
            transaction.exec(
                "SELECT id, title, author, isbn, available "
                "FROM books "
                "ORDER BY id"
            );

        for (const auto& row : result) {

            int id =
                row["id"].as<int>();

            string title =
                row["title"].as<string>();

            string author =
                row["author"].as<string>();

            string isbn =
                row["isbn"].as<string>();

            bool available =
                row["available"].as<bool>();

            books.emplace_back(
                id,
                title,
                author,
                isbn,
                available
            );
        }

        transaction.commit();

    } catch (const exception& e) {

        cerr << "Failed to fetch books: "
             << e.what()
             << endl;
    }

    return books;
}


void Database::updateBookAvailability(
    int bookId,
    bool available
) {

    try {
        auto lease = connectionPool.acquire();

        pqxx::work transaction(lease.get());

        transaction.exec(
            "UPDATE books "
            "SET available = $1 "
            "WHERE id = $2",
            pqxx::params{
                available,
                bookId
            }
        );

        transaction.commit();

        cout << "Book availability updated in database."
             << endl;

    } catch (const exception& e) {

        cerr << "Failed to update book availability: "
             << e.what()
             << endl;
    }
}


// ============================================================
// MEMBER OPERATIONS
// ============================================================

void Database::insertMember(
    int id,
    const string& name,
    const string& email
) {

    try {
        auto lease = connectionPool.acquire();

        pqxx::work transaction(lease.get());

        transaction.exec(
            "INSERT INTO users "
            "(id, name, email, role) "
            "VALUES ($1, $2, $3, 'MEMBER')",
            pqxx::params{
                id,
                name,
                email
            }
        );

        transaction.commit();

        cout << "Member inserted into database successfully."
             << endl;

    } catch (const exception& e) {

        cerr << "Failed to insert member: "
             << e.what()
             << endl;
    }
}


vector<Member> Database::getMembers() {

    vector<Member> members;

    try {
        auto lease = connectionPool.acquire();

        pqxx::work transaction(lease.get());

        pqxx::result result =
            transaction.exec(
                "SELECT id, name, email "
                "FROM users "
                "WHERE role = 'MEMBER' "
                "ORDER BY id"
            );

        for (const auto& row : result) {

            int id =
                row["id"].as<int>();

            string name =
                row["name"].as<string>();

            string email =
                row["email"].as<string>();

            members.emplace_back(
                id,
                name,
                email
            );
        }

        transaction.commit();

    } catch (const exception& e) {

        cerr << "Failed to fetch members: "
             << e.what()
             << endl;
    }

    return members;
}


bool Database::memberExists(int id) {

    try {
        auto lease = connectionPool.acquire();

        pqxx::work transaction(lease.get());

        pqxx::result result =
            transaction.exec(
                "SELECT 1 "
                "FROM users "
                "WHERE id = $1 "
                "AND role = 'MEMBER' "
                "LIMIT 1",
                pqxx::params{
                    id
                }
            );

        transaction.commit();

        return !result.empty();

    } catch (const exception& e) {

        cerr << "Failed to check member existence: "
             << e.what()
             << endl;

        return false;
    }
}


// ============================================================
// TRANSACTION OPERATIONS
// ============================================================

int Database::insertTransaction(
    int memberId,
    int bookId,
    TransactionType type
) {

    try {
        auto lease = connectionPool.acquire();

        pqxx::work transaction(lease.get());

        string transactionType =
            (type == TransactionType::BORROW)
                ? "BORROW"
                : "RETURN";

        pqxx::result result =
            transaction.exec(
                "INSERT INTO transactions "
                "(member_id, book_id, type) "
                "VALUES ($1, $2, $3) "
                "RETURNING id",
                pqxx::params{
                    memberId,
                    bookId,
                    transactionType
                }
            );

        int transactionId =
            result[0]["id"].as<int>();

        transaction.commit();

        cout << "Transaction inserted into database successfully."
             << endl;

        return transactionId;

    } catch (const exception& e) {

        cerr << "Failed to insert transaction: "
             << e.what()
             << endl;

        return -1;
    }
}


vector<Transaction> Database::getTransactions() {

    vector<Transaction> transactions;

    try {
        auto lease = connectionPool.acquire();

        pqxx::work transaction(lease.get());

        pqxx::result result =
            transaction.exec(
                "SELECT id, member_id, book_id, type "
                "FROM transactions "
                "ORDER BY id"
            );

        for (const auto& row : result) {

            int id =
                row["id"].as<int>();

            int memberId =
                row["member_id"].as<int>();

            int bookId =
                row["book_id"].as<int>();

            string type =
                row["type"].as<string>();

            TransactionType transactionType =
                (type == "BORROW")
                    ? TransactionType::BORROW
                    : TransactionType::RETURN;

            transactions.emplace_back(
                id,
                memberId,
                bookId,
                transactionType
            );
        }

        transaction.commit();

    } catch (const exception& e) {

        cerr << "Failed to fetch transactions: "
             << e.what()
             << endl;
    }

    return transactions;
}


// ============================================================
// ATOMIC BORROW OPERATION
// ============================================================

bool Database::borrowBook(
    int memberId,
    int bookId
) {

    try {
        auto lease = connectionPool.acquire();

        // All operations use this one leased connection.
        pqxx::work transaction(lease.get());

        // 1. Verify that the member exists.
        pqxx::result memberResult =
            transaction.exec(
                "SELECT 1 "
                "FROM users "
                "WHERE id = $1 "
                "AND role = 'MEMBER' "
                "LIMIT 1",
                pqxx::params{
                    memberId
                }
            );

            if (memberResult.empty()) {
                return false;
            }

        // 2. Claim the book only if it is available.
        pqxx::result bookResult =
            transaction.exec(
                "UPDATE books "
                "SET available = FALSE "
                "WHERE id = $1 "
                "AND available = TRUE "
                "RETURNING id",
                pqxx::params{
                    bookId
                }
            );

            if (bookResult.empty()) {
                return false;
            }

        // 3. Record the borrow.
        transaction.exec(
            "INSERT INTO transactions "
            "(member_id, book_id, type) "
            "VALUES ($1, $2, 'BORROW')",
            pqxx::params{
                memberId,
                bookId
            }
        );

        // 4. Commit the update and transaction together.
        transaction.commit();

        return true;

    } catch (const exception& e) {

        cerr << "Atomic borrow failed: "
             << e.what()
             << endl;

        return false;
    }
}


// ============================================================
// ATOMIC RETURN OPERATION
// ============================================================

bool Database::returnBook(
    int memberId,
    int bookId
) {

    try {
        auto lease = connectionPool.acquire();

        pqxx::work transaction(lease.get());

        // 1. Verify that the member exists.
        pqxx::result memberResult =
            transaction.exec(
                "SELECT 1 "
                "FROM users "
                "WHERE id = $1 "
                "AND role = 'MEMBER' "
                "LIMIT 1",
                pqxx::params{
                    memberId
                }
            );

            if (memberResult.empty()) {
                return false;
            }

        // 2. Return only a currently borrowed book.
        pqxx::result bookResult =
            transaction.exec(
                "UPDATE books "
                "SET available = TRUE "
                "WHERE id = $1 "
                "AND available = FALSE "
                "RETURNING id",
                pqxx::params{
                    bookId
                }
            );

            if (bookResult.empty()) {
                return false;
            }

        // 3. Record the return.
        transaction.exec(
            "INSERT INTO transactions "
            "(member_id, book_id, type) "
            "VALUES ($1, $2, 'RETURN')",
            pqxx::params{
                memberId,
                bookId
            }
        );

        // 4. Commit both changes together.
        transaction.commit();

        return true;

    } catch (const exception& e) {

        cerr << "Atomic return failed: "
             << e.what()
             << endl;

        return false;
    }
}