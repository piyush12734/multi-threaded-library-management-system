#include "../include/User.h"
#include <iostream>

using namespace std;

User::User(int id, string name, string email) {
    this->id = id;
    this->name = name;
    this->email = email;
}

int User::getId() const {
    return id;
}

string User::getName() const {
    return name;
}

string User::getEmail() const {
    return email;
}

void User::displayRole() const {
    cout << "Role: User" << endl;
}