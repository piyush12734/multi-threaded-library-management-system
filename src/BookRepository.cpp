#include "../include/BookRepository.h"
#include <iostream>

using namespace std;

BookRepository::BookRepository(Database& database)
    : database(database) {
}

void BookRepository::add(const Book& book) {

    database.insertBook(
        book.getId(),
        book.getTitle(),
        book.getAuthor(),
        book.getIsbn()
    );
}

vector<Book> BookRepository::getAll() {

    return database.getBooks();
}

bool BookRepository::findById(int id, Book& book) {

    vector<Book> books = database.getBooks();

    for (const Book& currentBook : books) {

        if (currentBook.getId() == id) {
            book = currentBook;
            return true;
        }
    }

    return false;
}

void BookRepository::updateAvailability(
    int bookId,
    bool available
) {

    database.updateBookAvailability(
        bookId,
        available
    );
}