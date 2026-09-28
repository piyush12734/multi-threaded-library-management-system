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