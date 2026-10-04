#ifndef MEMBER_REPOSITORY_H
#define MEMBER_REPOSITORY_H

#include <vector>

#include "Member.h"
#include "Database.h"

using namespace std;

class MemberRepository {
private:
    Database& database;

public:
    MemberRepository(Database& database);

    void add(const Member& member);

    vector<Member> getAll();

    bool exists(int id);
};

#endif