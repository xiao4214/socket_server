#include "Log.h"
#include <iostream>
#include <sstream>

using namespace std;

string getCurrentTime() {
    time_t t;
    time(&t);
    tm *now_time = localtime(&t);
    stringstream stream;
    stream << 1900 + now_time->tm_year << '-';
    stream << now_time->tm_mon + 1 << '-';
    stream << now_time->tm_mday << ' ';
    stream << now_time->tm_hour << ':';
    stream << now_time->tm_min << ':';
    stream << now_time->tm_sec;
    return stream.str();
}
