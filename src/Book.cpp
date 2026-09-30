#include "../include/Book.h"
#include <iostream>

using namespace std;

Book::Book(
    int id,
    string title,
    string author,
    string isbn,
    bool available
) {
    this->id = id;
    this->title = title;
    this->author = author;
    this->isbn = isbn;
    this->available = available;
}

void Book::borrow() {
    if (available) {
        available = false;
        cout << "Book borrowed successfully.\n";
    } else {
        cout << "Book is already borrowed.\n";
    }
}

void Book::returnBook() {
    if (!available) {
        available = true;
        cout << "Book returned successfully.\n";
    } else {
        cout << "Book is already available.\n";
    }
}

int Book::getId() const {
    return id;
}

string Book::getTitle() const {
    return title;
}

string Book::getAuthor() const {
    return author;
}

string Book::getIsbn() const {
    return isbn;
}

bool Book::isAvailable() const {
    return available;
}