#include "Json.h"
#include <algorithm>
#include <sstream>

using namespace std;
using namespace CwUtil;

Json::Json() {
    type_ = Json_null;
}

Json::Json(bool value) : type_(Json_bool) {
    value_ = make_shared<Value>();
    value_->bool_ = value;
}

Json::Json(int value) : type_(Json_int) {
    value_ = make_shared<Value>();
    value_->int_ = value;
}

Json::Json(double value) : type_(Json_double) {
    value_ = make_shared<Value>();
    value_->double_ = value;
}

Json::Json(const char *value) : type_(Json_string) {
    value_ = shared_ptr<Value>(new Value, stringDeleter);
    value_->string_ = new string(value);
}

Json::Json(const string &value) : type_(Json_string) {
    value_ = shared_ptr<Value>(new Value, stringDeleter);
    value_->string_ = new string(value);
}

Json::operator bool() {
    if (type_ != Json_bool) {
        throw logic_error("type error , not bool type");
    }
    return value_->bool_;
}

Json::operator string() {
    if (type_ != Json_string) {
        throw logic_error("type error , not string type");
    }
    return *value_->string_;
}

Json::operator int() {
    if (type_ != Json_int) {
        throw logic_error("type error , not int type");
    }
    return value_->int_;
}

Json::operator double() {
    if (type_ != Json_double) {
        throw logic_error("type error , not double type");
    }
    return value_->double_;
}

Json &Json::operator[](int index) {
    if (value_ == nullptr) {
        throw out_of_range("index out of Json array range");
    }
    if (type_ != Json_array) {
        throw logic_error("type error , not array type");
    }
    if (index > value_->array_->size()) {
        throw out_of_range("index out of Json array range");
    }
    return value_->array_->at(index);
}

Json &Json::operator[](const char *key) {
    if (value_ == nullptr) {
        type_ = Json_object;
        value_ = shared_ptr<Value>(new Value, objectDeleter);
        value_->object_ = new map<string, Json>();
    }
    if (type_ != Json_object) {
        throw logic_error("type error , not object type");
    }
    return (*(value_->object_))[key];
}

Json &Json::operator[](const string &key) {
    return (*this)[key.c_str()];
}

size_t Json::length() const {
    if (type_ != Json_array) {
        throw logic_error("type error , not array type");
    }
    return value_->array_->size();
}

void Json::append(const Json &json) {
    if (value_ == nullptr) {
        type_ = Json_array;
        value_ = shared_ptr<Value>(new Value, arrayDeleter);
        value_->array_ = new vector<Json>();
    } else if (type_ != Json_array) {
        throw logic_error("type error , not array type");
    }
    value_->array_->push_back(json);
}

string Json::toString() const {
    stringstream ss;
    switch (type_) {
        case Json_null:
            ss << "null";
            break;
        case Json_int:
            ss << value_->int_;
            break;
        case Json_string:
            ss << '\"' << *(value_->string_) << '\"';
            break;
        case Json_double:
            ss << value_->double_;
            break;
        case Json_bool: {
            if (value_->bool_) {
                ss << "true";
            } else {
                ss << "false";
            }
            break;
        }
        case Json_array: {
            ss << '[';
            for (auto i = value_->array_->begin(); i != value_->array_->end(); ++i) {
                if (i != value_->array_->begin()) {
                    ss << ',';
                }
                ss << i->toString();
            }
            ss << ']';
            break;
        }
        case Json_object: {
            ss << '{';
            for (auto i = value_->object_->begin(); i != value_->object_->end(); ++i) {
                if (i != value_->object_->begin()) {
                    ss << ',';
                }
                ss << '\"' << i->first << "\":" << i->second.toString();
            }
            ss << '}';
            break;
        }
        default:
            break;
    }
    return ss.str();
}

Json Json::parseValue(stringstream &ss) {
    while (ss.peek() != -1) {
        if (ss.peek() == '\t' || ss.peek() == '\r' || ss.peek() == ' ' || ss.peek() == '\n') {
            ss.get();
        } else if (ss.peek() == '\"') {
            return parseString(ss);
        } else if (ss.peek() == '{') {
            return parseObject(ss);
        } else if (ss.peek() == '[') {
            return parseArray(ss);
        } else if (ss.peek() == 'f' || ss.peek() == 't') {
            return parseBool(ss);
        } else {
            return parseNumber(ss);
        }
    }
}

Json Json::parseJson(const string &json) {
    stringstream ss;
    ss.str(json);
    return parseValue(ss);
}

Json Json::parseJson(const char *json) {
    stringstream ss;
    ss.str(json);
    return parseValue(ss);
}

Json Json::parseBool(stringstream &ss) {
    streampos pos = ss.tellg();
    if (ss.peek() == 't') {
        ss.seekg(pos + streampos(4));
        return {true};
    } else if (ss.peek() == 'f') {
        ss.seekg(pos + streampos(5));
        return {false};
    }
    throw logic_error("bad type conversion to bool");
}

Json Json::parseString(stringstream &ss) {
    ss.get();
    string ret;
    while (ss.peek() != '"') {
        ret.push_back((char) ss.get());
    }
    ss.get();
    return {ret};
}

Json Json::parseObject(stringstream &ss) {
    ss.get();
    Json json;
    while (ss.peek() != '}') {
        string key = parseValue(ss).toString().erase(0, 1);
        key.pop_back();
        while (ss.peek() == ' ' || ss.peek() == ':') {
            ss.get();
        }
        json[key] = parseValue(ss);
        while (ss.peek() != '}' &&
               (ss.peek() == ' ' || ss.peek() == '\t' || ss.peek() == '\n' || ss.peek() == '\r' || ss.peek() == ',')) {
            ss.get();
        }
    }
    return json;
}

Json Json::parseArray(stringstream &ss) {
    ss.get();
    Json json;
    while (ss.peek() != ']') {
        json.append(parseValue(ss));
        while (ss.peek() != ']' && (ss.peek() == ' ' || ss.peek() == '\t' || ss.peek() == '\n' || ss.peek() == ',')) {
            ss.get();
        }
    }
    ss.get();
    return json;
}

Json Json::parseNumber(stringstream &ss) {
    string s;
    while (isdigit(ss.peek()) || ss.peek() == 'e' || ss.peek() == '-' || ss.peek() == '+' || ss.peek() == '.') {
        s.push_back((char) ss.get());
    }
    if (count(s.begin(), s.end(), '.') || count(s.begin(), s.end(), 'e')) {
        return {stof(s)};
    } else {
        return {stoi(s)};
    }
}

bool Json::asBool() const {
    if (type_ != Json_bool) {
        throw logic_error("type error , not bool type");
    }
    return value_->bool_;
}

int Json::asInt() const {
    if (type_ != Json_int) {
        throw logic_error("type error , not int type");
    }
    return value_->int_;
}

double Json::asDouble() const {
    if (type_ != Json_double) {
        throw logic_error("type error , not double type");
    }
    return value_->double_;
}

string Json::asString() const {
    if (type_ != Json_string) {
        throw logic_error("type error , not string type");
    }
    return *(value_->string_);
}

bool Json::has(int index) const {
    if (type_ != Json_array || index < 0) {
        return false;
    }
    return index < value_->array_->size();
}

bool Json::has(const char *key) const {
    if (type_ != Json_object) {
        return false;
    }
    return value_->object_->count(key);
}

bool Json::has(const string &key) const {
    if (type_ != Json_object) {
        return false;
    }
    return value_->object_->count(key);
}

void Json::remove(int index) {
    if (!has(index)) {
        return;
    }
    value_->array_->erase(value_->array_->begin() + index);
}

void Json::remove(const char *key) {
    remove(string(key));
}

void Json::remove(const string &key) {
    if (!has(key)) {
        return;
    }
    value_->object_->erase(key);
}
