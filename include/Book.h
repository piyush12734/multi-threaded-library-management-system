#ifndef BOOK_H
#define BOOK_H

#include <string>

using namespace std;

class Book {
private:
    int id;
    string title;
    string author;
    string isbn;
    bool available;

public:
    Book(int id, string title, string author, string isbn);

    void borrow();
    void returnBook();

    int getId() const;
    string getTitle() const;
    string getAuthor() const;
    string getIsbn() const;
    bool isAvailable() const;
};

#endif