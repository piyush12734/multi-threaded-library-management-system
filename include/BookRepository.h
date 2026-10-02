#ifndef BOOK_REPOSITORY_H
#define BOOK_REPOSITORY_H

#include <vector>
#include "Book.h"
#include "Database.h"

using namespace std;

class BookRepository {
private:
    Database& database;

public:
    BookRepository(Database& database);

    void add(const Book& book);

    vector<Book> getAll();

    bool findById(int id, Book& book);

    void updateAvailability(int bookId, bool available);
};

#endif