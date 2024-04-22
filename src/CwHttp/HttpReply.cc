#include "HttpReply.h"
#include <cstring>
#include <sstream>

using namespace std;
using namespace CwHttp;

HttpReply::HttpReply() {
    memset(status_code_, 0, 4);
    memcpy(status_code_, "200", 3);
}

HttpReply::HttpReply(const string &status_code) : HttpReply() {
    memcpy(status_code_, status_code.c_str(), 3);
}

HttpReply::HttpReply(const char *status_code) : HttpReply() {
    memcpy(status_code_, status_code, 3);
}

HttpReply::HttpReply(const char *status_code, const char *status) {
    memset(status_code_, 0, 4);
    memcpy(status_code_, status_code, 3);
    status_ = status;
}

HttpReply::HttpReply(const string &status_code, const string &status) {
    memset(status_code_, 0, 4);
    memcpy(status_code_, status_code.c_str(), 3);
    status_ = status;
}

string HttpReply::toString() const {
    stringstream stream;
    stream << getVersion() << ' ' << status_code_ << ' ' << status_ << ' ' << endl;
    unordered_map<string, string> headers = getAllHeader();
    for (const pair<const string, string> &i: headers) {
        stream << i.first << ": " << i.second << endl;
    }
    stream << endl << getBody();
    return stream.str();
}

void HttpReply::setStatusCode(const char *status_code) {
    memcpy(status_code_, status_code, 3);
}

void HttpReply::setStatusCode(const string &status_code) {
    memcpy(status_code_, status_code.c_str(), 3);
}

HttpReply &HttpReply::operator=(const HttpReply &reply) {
    status_ = reply.status_;
    memcpy(status_code_, reply.status_code_, 4);
    resetHeader(reply.getAllHeader());
    setBody(getBody());
    return *this;
}

HttpReply::HttpReply(const HttpReply &reply) {
    status_ = reply.status_;
    memcpy(status_code_, reply.status_code_, 4);
    resetHeader(reply.getAllHeader());
    setBody(getBody());
}

HttpReply::HttpReply(HttpReply &&reply) noexcept {
    status_ = move(reply.status_);
    memmove(status_code_, reply.status_code_, 4);
    resetHeader(reply.getAllHeader());
    setBody(getBody());
    reply.setBody("");
}
