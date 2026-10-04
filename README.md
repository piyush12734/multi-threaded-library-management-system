# Multi-Threaded Library Management System

A backend-oriented Library Management System built in **C++** with **PostgreSQL**, focusing on Object-Oriented Programming, database design, repository architecture, and concurrent system development.

The project is being developed incrementally, with the goal of building a complete backend system that can later be connected to a web-based frontend.

## Tech Stack

- **C++**
- **PostgreSQL**
- **libpqxx** — PostgreSQL client library for C++
- **MSYS2 UCRT64**
- **Git & GitHub**

## Current Architecture

```text
                    Library Management System
                              │
                              ▼
                           Library
                              │
             ┌────────────────┼────────────────┐
             ▼                ▼                ▼
      BookRepository   MemberRepository   TransactionRepository
             │                │                │
             └────────────────┼────────────────┘
                              ▼
                           Database
                              │
                              ▼
                         PostgreSQL
```

## Project Structure

```text
multi-threaded-library-management-system/
│
├── include/
│   ├── Book.h
│   ├── User.h
│   ├── Member.h
│   ├── Admin.h
│   ├── Transaction.h
│   ├── Repository.h
│   ├── Database.h
│   ├── BookRepository.h
│   ├── MemberRepository.h
│   ├── TransactionRepository.h
│   └── Library.h
│
├── src/
│   ├── main.cpp
│   ├── Book.cpp
│   ├── User.cpp
│   ├── Member.cpp
│   ├── Admin.cpp
│   ├── Transaction.cpp
│   ├── Database.cpp
│   ├── BookRepository.cpp
│   ├── MemberRepository.cpp
│   ├── TransactionRepository.cpp
│   └── Library.cpp
│
├── db/
│   └── schema.sql
│
├── .gitignore
└── README.md
```

## Features Implemented

### Object-Oriented Design

The project currently demonstrates:

- Classes and objects
- Encapsulation
- Inheritance
- Runtime polymorphism
- Constructors and member functions
- Virtual functions

The main user hierarchy is:

```text
          User
         /    \
        /      \
    Member    Admin
```

### Book Management

Books contain:

- ID
- Title
- Author
- ISBN
- Availability status

The system supports:

- Adding books
- Reading books from PostgreSQL
- Finding books by ID
- Borrowing books
- Returning books
- Updating availability in PostgreSQL
- Preventing a second borrow when a book is already borrowed

### Member Management

Members contain:

- ID
- Name
- Email

Members are stored in PostgreSQL and can be checked for existence before insertion.

### Transaction Management

The system records:

- Transaction ID
- Member ID
- Book ID
- Transaction type
- Transaction timestamp

Supported transaction types:

```text
BORROW
RETURN
```

Transactions are persisted in PostgreSQL.

## Database Design

The PostgreSQL database currently contains three main tables:

```text
users
books
transactions
```

Relationships:

```text
users
  │
  │ member_id
  ▼
transactions
  ▲
  │ book_id
  │
books
```

The schema uses:

- Primary keys
- Foreign keys
- Unique constraints
- Check constraints
- Indexes

## Repository Pattern

The project separates business logic from data access.

For example:

```text
Library
   ↓
BookRepository
   ↓
Database
   ↓
PostgreSQL
```

This separation makes the system easier to maintain and allows the data-storage implementation to evolve independently from the library business logic.

## Database Integration

The C++ application communicates with PostgreSQL using `libpqxx`.

Example flow:

```text
C++ application
      ↓
    libpqxx
      ↓
 PostgreSQL
```

Database credentials are provided through the environment variable:

```text
LIBRARY_DB_PASSWORD
```

The password is intentionally kept outside the source code and should never be committed to GitHub.

## Current Borrow Flow

```text
Member requests a book
        ↓
Library checks member
        ↓
BookRepository finds the book
        ↓
Check availability
        ↓
Update book availability
        ↓
Create transaction
        ↓
Store transaction in PostgreSQL
```

## How to Build

Make sure PostgreSQL is running and the required environment variable is configured.

Compile from the project root:

```powershell
g++ src/main.cpp src/Book.cpp src/User.cpp src/Member.cpp src/Admin.cpp src/Transaction.cpp src/Library.cpp src/Database.cpp src/BookRepository.cpp src/TransactionRepository.cpp src/MemberRepository.cpp -Iinclude -I"C:\msys64\ucrt64\include" -L"C:\msys64\ucrt64\lib" -o library -lpqxx -lpq
```

Run:

```powershell
.\library.exe
```

## Database Setup

Create the PostgreSQL database:

```sql
CREATE DATABASE library_management;
```

Then execute:

```text
db/schema.sql
```

The schema creates:

```text
users
books
transactions
```

## Example

A successful borrow operation looks like:

```text
===== BORROW BOOK =====
Book availability updated in database.
Transaction inserted into database successfully.
Book borrowed successfully.
```

Trying to borrow the same book again:

```text
===== BORROW SAME BOOK AGAIN =====
Book is already borrowed.
```

Returning the book:

```text
===== RETURN BOOK =====
Book availability updated in database.
Transaction inserted into database successfully.
Book returned successfully.
```

## Future Development

The project is being expanded toward a complete concurrent backend system.

Planned improvements include:

- Atomic borrow/return database transactions
- Improved member and book management
- Search and reservation functionality
- Multithreading
- Mutexes and thread synchronization
- Condition variables
- Thread pool
- Concurrent client handling
- Networking / REST API
- Authentication and authorization
- Logging and error handling
- Unit testing
- Docker support
- React-based frontend
- Admin dashboard
- Member dashboard

## Learning Goals

This project is designed to demonstrate practical understanding of:

```text
C++
OOP
Data Structures
DBMS
SQL
PostgreSQL
Repository Pattern
Database Transactions
Multithreading
Concurrency
Networking
Backend Development
Software Architecture
```

## Author

**Piyush Negi**

B.Tech Computer Science  
Bennett University