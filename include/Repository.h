#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <vector>

using namespace std;

template <typename T>
class Repository {
private:
    vector<T> data;

public:
    void add(const T& item);

    T* findById(int id);

    const vector<T>& getAll() const;
};

template <typename T>
void Repository<T>::add(const T& item) {
    data.push_back(item);
}

template <typename T>
T* Repository<T>::findById(int id) {

    for (T& item : data) {
        if (item.getId() == id) {
            return &item;
        }
    }

    return nullptr;
}

template <typename T>
const vector<T>& Repository<T>::getAll() const {
    return data;
}

#endif