
#ifndef OPERATION_RESULT_H
#define OPERATION_RESULT_H

enum class OperationResult {
    Success,
    MemberNotFound,
    BookNotFound,
    BookNotAvailable,
    BookAlreadyAvailable,
    BookAlreadyExists,
    MemberAlreadyExists,
    DatabaseError
};

#endif
