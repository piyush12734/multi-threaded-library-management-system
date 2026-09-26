#ifndef USER_H
#define USER_H

#include <string>

using namespace std;

class User {
private:
    int id;
    string name;
    string email;

public:
    User(int id, string name, string email);

    int getId() const;
    string getName() const;
    string getEmail() const;

    virtual void displayRole() const;
};

#endif