#ifndef MEMBER_H
#define MEMBER_H

#include "User.h"

class Member : public User {
public:
    Member(int id, string name, string email);

    void displayRole() const override;
};

#endif