#include "../include/MemberRepository.h"

using namespace std;

MemberRepository::MemberRepository(
    Database& database
)
    : database(database) {
}

void MemberRepository::add(
    const Member& member
) {

    database.insertMember(
        member.getId(),
        member.getName(),
        member.getEmail()
    );
}

vector<Member> MemberRepository::getAll() {

    return database.getMembers();
}

bool MemberRepository::exists(int id) {

    return database.memberExists(id);
}