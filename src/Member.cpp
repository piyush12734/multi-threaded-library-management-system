#include "../include/Member.h"
#include <iostream>

using namespace std;

Member::Member(int id, string name, string email)
    : User(id, name, email) {
}

void Member::displayRole() const {
    cout << "Role: Member" << endl;
}