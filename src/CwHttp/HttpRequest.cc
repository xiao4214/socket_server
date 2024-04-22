#include "HttpRequest.h"
#include <sstream>
#include <cstring>

using namespace std;
using namespace CwHttp;

HttpRequest &HttpRequest::operator=(const HttpRequest &request) {
    url_ = request.url_;
    method_ = request.method_;
    setVersion(getVersion());
    setBody(getBody());
    resetHeader(request.getAllHeader());
    return *this;
}

HttpRequest::HttpRequest(const HttpRequest &request) : HttpBase(request) {
    url_ = request.url_;
    method_ = request.method_;
    setVersion(getVersion());
    setBody(getBody());
    resetHeader(request.getAllHeader());
}

HttpRequest::HttpRequest(HttpRequest &&request) noexcept {
    url_ = std::move(request.url_);
    method_ = request.method_;
    setVersion(getVersion());
    setBody(getBody());
    request.setBody("");
    resetHeader(request.getAllHeader());
}

HttpRequest HttpRequest::paresRequest(const string &request) {
    tuple<RequestMethod, string, string> line = getRequestLine(request);
    size_t body_index = request.find("\r\n\r\n");
    unordered_map<string, string> header = getRequsetHeader(request, body_index);
    HttpRequest ret(get<0>(line), get<1>(line), get<2>(line));
    ret.resetHeader(header);
    ret.setBody(string(&request[body_index + 4], &request[request.size()]));
    return ret;
}

string HttpRequest::getUrl() const {
    string::size_type pos = url_.find('?');
    if (pos != string::npos) {
        return {&url_[0], &url_[pos]};
    }
    return url_;
}

std::string HttpRequest::getMethodStr() const {
    if (method_ == RequestMethod::POST) {
        return "POST";
    } else if (method_ == RequestMethod::GET) {
        return "GET";
    } else if (method_ == RequestMethod::HAND) {
        return "HAND";
    } else if (method_ == RequestMethod::PUT) {
        return "PUT";
    } else if (method_ == RequestMethod::DELETE) {
        return "DELETE";
    } else if (method_ == RequestMethod::CONNECT) {
        return "CONNECT";
    } else if (method_ == RequestMethod::OPTIONS) {
        return "OPTIONS";
    } else if (method_ == RequestMethod::PATCH) {
        return "PATCH";
    } else {
        throw runtime_error("unknow request method");
    }
}

unordered_map<string, string> HttpRequest::getUrlParameter() const {
    size_t start = url_.find_first_of('?');
    if (start == string::npos) {
        return unordered_map<string, string>();
    }
    size_t end = 0;
    unordered_map<string, string> ret;
    while (true) {
        end = url_.find_first_of('=', start + 1);
        if (end == string::npos) { break; }
        string key = string(&url_[start + 1], &url_[end]);
        start = end;
        end = url_.find_first_of('&', start + 1);
        if (end == string::npos) {
            string val = string(&url_[start + 1], &url_[url_.size()]);
            ret.emplace(key, val);
            break;
        }
        string val = string(&url_[start + 1], &url_[end]);
        start = end;
        ret.emplace(key, val);
    }
    return ret;
}

RequestMethod HttpRequest::getRequestMethod(const string &request) {
    if (strncmp(request.c_str(), "POST", 4) == 0)
        return RequestMethod::POST;
    else if (strncmp(request.c_str(), "GET", 3) == 0)
        return RequestMethod::GET;
    else if (strncmp(request.c_str(), "HAND", 4) == 0)
        return RequestMethod::HAND;
    else if (strncmp(request.c_str(), "PUT", 3) == 0)
        return RequestMethod::PUT;
    else if (strncmp(request.c_str(), "DELETE", 6) == 0)
        return RequestMethod::DELETE;
    else if (strncmp(request.c_str(), "CONNECT", 6) == 0)
        return RequestMethod::CONNECT;
    else if (strncmp(request.c_str(), "OPTIONS", 6) == 0)
        return RequestMethod::OPTIONS;
    else if (strncmp(request.c_str(), "PATCH", 5) == 0)
        return RequestMethod::PATCH;
    throw runtime_error("unknow request method");
}

tuple<RequestMethod, string, string> HttpRequest::getRequestLine(const string &request) {
    RequestMethod method = getRequestMethod(request);
    size_t url_start = request.find(' ');
    size_t url_end = request.find(' ', url_start + 1);
    string url(&request[url_start + 1], &request[url_end]);
    string version(&request[url_end + 1], &request[request.find("\r\n")]);
    return tuple<RequestMethod, string, string>(method, url, version);
}

unordered_map<string, string>
HttpRequest::getRequsetHeader(const string &request, const size_t body_index) {
    string key, val;
    unordered_map<string, string> ret;
    size_t pre_index = request.find("\r\n") + 2;
    size_t end_index = pre_index;
    while (pre_index < body_index) {
        end_index = request.find(": ", pre_index);
        key = string(&request[pre_index], &request[end_index]);
        pre_index = request.find("\r\n", end_index);
        val = string(&request[end_index + 2], &request[pre_index]);
        ret.emplace(key, val);
        pre_index += 2;
    }
    return ret;
}

string HttpRequest::toString() const {
    stringstream stream;
    stream << getMethodStr() << ' ' << url_ << ' ' << getVersion() << ' ' << endl;
    unordered_map<string, string> headers = getAllHeader();
    for (const pair<const string, string> &i: headers) {
        stream << i.first << ": " << i.second << endl;
    }
    stream << endl << getBody();
    return stream.str();
}
