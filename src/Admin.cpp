#include "../include/Admin.h"
#include <iostream>

using namespace std;

Admin::Admin(int id, string name, string email)
    : User(id, name, email) {
}

void Admin::displayRole() const {
    cout << "Role: Admin" << endl;
}