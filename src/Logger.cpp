#include "../include/Logger.h"
#include <iostream>

using namespace std;

void Logger::log(const string& message) {

    lock_guard<mutex> lock(logMutex);

    cout << message << endl;
}