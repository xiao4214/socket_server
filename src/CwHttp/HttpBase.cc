#include "HttpBase.h"

using namespace std;
using namespace CwHttp;

bool HttpBase::addHeader(const string &key, const string &val) {
    if (header_.count(key) != 0) {
        return false;
    }
    header_.emplace(key, val);
    return true;
}

string HttpBase::getHeader(const string &key) const {
    if (header_.count(key) == 0) {
        return {};
    }
    return header_.at(key);
}

bool HttpBase::setHeader(const string &key, const string &val) {
    if (header_.count(key) == 0) {
        return false;
    }
    header_[key] = val;
    return true;
}
