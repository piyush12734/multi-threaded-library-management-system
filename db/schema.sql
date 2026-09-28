-- ============================================
-- Library Management System Database
-- PostgreSQL Schema
-- ============================================


-- ============================================
-- USERS
-- ============================================

CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(150) UNIQUE NOT NULL,

    role VARCHAR(20) NOT NULL
        CHECK (role IN ('MEMBER', 'ADMIN'))
);


-- ============================================
-- BOOKS
-- ============================================

CREATE TABLE books (
    id SERIAL PRIMARY KEY,
    title VARCHAR(200) NOT NULL,
    author VARCHAR(150) NOT NULL,
    isbn VARCHAR(20) UNIQUE NOT NULL,

    available BOOLEAN NOT NULL DEFAULT TRUE
);


-- ============================================
-- TRANSACTIONS
-- ============================================

CREATE TABLE transactions (
    id SERIAL PRIMARY KEY,

    member_id INTEGER NOT NULL,
    book_id INTEGER NOT NULL,

    type VARCHAR(20) NOT NULL
        CHECK (type IN ('BORROW', 'RETURN')),

    transaction_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT fk_transaction_member
        FOREIGN KEY (member_id)
        REFERENCES users(id),

    CONSTRAINT fk_transaction_book
        FOREIGN KEY (book_id)
        REFERENCES books(id)
);


-- ============================================
-- INDEXES
-- ============================================

CREATE INDEX idx_transactions_member_id
ON transactions(member_id);

CREATE INDEX idx_transactions_book_id
ON transactions(book_id);

CREATE INDEX idx_transactions_time
ON transactions(transaction_time);