#ifndef LOGGER_H
#define LOGGER_H

#include <mutex>
#include <string>

using namespace std;

class Logger {
private:
    mutex logMutex;

public:
    void log(const string& message);
};

#endif