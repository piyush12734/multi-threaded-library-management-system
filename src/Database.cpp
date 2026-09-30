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

vector<Book> Database::getBooks() {

    vector<Book> books;

    try {
        pqxx::work transaction(connection);

        pqxx::result result =
            transaction.exec(
                "SELECT id, title, author, isbn, available "
                "FROM books "
                "ORDER BY id"
            );

        for (const auto& row : result) {

            int id = row["id"].as<int>();

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