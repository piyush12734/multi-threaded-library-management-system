#include "../include/Book.h"
#include <iostream>

using namespace std;

int main() {

    Book book(
        1,
        "Clean Code",
        "Robert C. Martin",
        "9780132350884"
    );

    cout << "Book: " << book.getTitle() << endl;
    cout << "Author: " << book.getAuthor() << endl;
    cout << "ISBN: " << book.getIsbn() << endl;

    cout << "\nAvailability: "
         << (book.isAvailable() ? "Available" : "Not Available")
         << endl;

    cout << "\n--- Borrowing ---\n";
    book.borrow();

    cout << "Availability: "
         << (book.isAvailable() ? "Available" : "Not Available")
         << endl;

    cout << "\n--- Trying to borrow again ---\n";
    book.borrow();

    cout << "\n--- Returning ---\n";
    book.returnBook();

    cout << "Availability: "
         << (book.isAvailable() ? "Available" : "Not Available")
         << endl;

    return 0;
}