#include "../include/Database.h"
#include <iostream>

using namespace std;

Database::Database(const string& connectionString)
    : connection(connectionString) {
}

bool Database::testConnection() {

    try {
        pqxx::work transaction(connection);

        pqxx::result result =
            transaction.exec("SELECT 1");

        transaction.commit();

        return !result.empty() && result[0][0].as<int>() == 1;

    } catch (const exception& e) {

        cerr << "Database error: "
             << e.what()
             << endl;

        return false;
    }
}

void Database::insertBook(
    int id,
    const string& title,
    const string& author,
    const string& isbn
) {

    try {
        pqxx::work transaction(connection);

        transaction.exec(
            "INSERT INTO books (id, title, author, isbn, available) "
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