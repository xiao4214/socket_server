#include <map>
#include <set>
#include <fstream>
#include <iostream>
#include <thread>
#include "CwUtil/Log.h"
#include "CwUtil/Json.h"
#include "CwNetWork/TcpServer.h"
#include "CwHttp/HttpRequest.h"
#include "httplib.h"
#include <mutex>


using namespace std;
using namespace CwUtil;
using namespace CwHttp;
using namespace CwNetWork;

std::mutex map_mutex;
map<int, pair<string, bool>> online_map;
map<int, pair<string, bool>> online_map_sendall;

Json glob_config;

Json java_server_config;

Json readConfigFile(const std::string &path) {
    std::ifstream in(path, std::ios::in | std::ios::binary);
    if (!in) {
        throw std::runtime_error("Could not open the file.");
    }
    in.seekg(0, std::ios::end);
    std::streampos fileSize = in.tellg();
    if (fileSize <= 0) {
        throw std::runtime_error("File is empty.");
    }
    in.seekg(0, std::ios::beg);
    std::string contents(fileSize, '\0');
    std::istreambuf_iterator<char> begin(in), end;
    std::copy(begin, end, contents.begin());
    if (in.fail() && !in.eof()) {
        throw std::runtime_error("Failed to read the entire file.");
    }
    contents.resize(fileSize);
    return Json::parseJson(contents);
}

void checkConfig(Json &config) {
    if (!glob_config.has("port")) {
        throw runtime_error("配置文件没有定义socket服务器监听端口");
    }
    if (!glob_config.has("timeout")) {
        throw runtime_error("配置文件没有定义socket服务器监听端口");
    }
    if (!glob_config.has("java-server")) {
        throw runtime_error("配置文件没有定义java服务器配置信息");
    }
    java_server_config = glob_config["java-server"];
    if (!java_server_config.has("ip")) {
        throw runtime_error("配置文件没有定义java服务器ip信息");
    }
    if (!java_server_config.has("port")) {
        throw runtime_error("配置文件没有定义java服务器端口信息");
    }
    if (!java_server_config.has("request-url")) {
        throw runtime_error("配置文件没有定义java服务器请求url信息");
    }
}

void sendHttp(const string &msg) {
    std::string post_body = msg;
    //std::cout << "---- ---- in sendHttp: msg = " << msg;
    std::string send_ip = java_server_config["ip"].asString();
    int send_port = java_server_config["port"].asInt();
    std::string send_url = java_server_config["request-url"].asString();
    LOG_INFO << "-------- in sendhttp: msg = " << msg << LOG_ENDL;
    LOG_INFO << "-------- in sendhttp: send ip = " << send_ip << LOG_ENDL;
    LOG_INFO << "-------- in sendhttp: send port = " << send_port << LOG_ENDL;
    LOG_INFO << "-------- in sendhttp: send url = " << send_url << LOG_ENDL;

    httplib::Client cli (send_ip, send_port);
    auto res = cli.Post(send_url, post_body, "application/json");
    if (res->status == 200) {
        LOG_INFO << "-------- in sendhttp: post success!!!" << LOG_ENDL;
        LOG_INFO << "-------- res->body = " << res->body << LOG_ENDL;
    } else {
        LOG_INFO << "-------- post failed: code = " << res->status << LOG_ENDL;
        LOG_INFO << "faleid msg = " << res->body << LOG_ENDL;
    }
#if 0
    HttpRequest req;
    req.setMethod(CwHttp::RequestMethod::POST);
    req.setUrl(java_server_config["request-url"].asString());
    req.setHeader("Content-Type", "text/plain; charset=UTF-8");
    req.setBody(msg);
    Socket http_client = Socket::newSocket();
    if (!http_client.connectToHost(java_server_config["ip"].asString(), java_server_config["port"].asInt())) {
        throw runtime_error("无法连接java服务器");
    }
    http_client.sendAll(req.toString());
    http_client.closeFd();
#endif // if 0
}

void recv_cb(Socket client, const std::string &msg, TcpServer *const server) {
    try {
        if (msg == "pang") {
            online_map.at(client.getFd()).second = true;
            return;
        }
        Json root = Json::parseJson(msg);
        if (!root.has("user_name")) {
            throw runtime_error("不存在user_name字段");
        }
        string user_name = root["user_name"].asString();
        map_mutex.lock();
        online_map.emplace(client.getFd(), make_pair(user_name, true));
        map_mutex.unlock();
        LOG_INFO << "新的用户登陆：" << user_name << LOG_ENDL;
    } catch (const exception &e) {
        server->disConnect(client);
        LOG_ERROR << e.what() << LOG_ENDL;
    }
}

void close_cb(const Socket &client, TcpServer *const server) {
    try {
        if (online_map.count(client.getFd()) == 0) {
            //throw runtime_error("不存在的用户");
            return;
        }
        auto client_info = online_map.at(client.getFd());
        if (client_info.second) {
            LOG_INFO << "用户：" << online_map.at(client.getFd()).first << "已离线" << LOG_ENDL;
        } else {
            LOG_INFO << "检测到未回复心跳包客户端：" << client_info.first << LOG_ENDL;
        }

        std::string str_name = online_map.at(client.getFd()).first;
        LOG_INFO << "-------- close_cb: str_name = " << str_name << LOG_ENDL;
        Json root;
        root["username"] = str_name;
        sendHttp(root.toString());  // { "username" : "xiongzp2" }

        map_mutex.lock();
        online_map.erase(client.getFd());
        map_mutex.unlock();
    } catch (const exception &e) {
        std::string str_name = online_map.at(client.getFd()).first;
        LOG_INFO << "-------- error: close_cb: str_name = " << str_name << LOG_ENDL;
        Json root;
        root["username"] = str_name;
        sendHttp(root.toString());  // { "username" : "xiongzp2" }

        map_mutex.lock();
        online_map.erase(client.getFd());
        map_mutex.unlock();

        LOG_ERROR << e.what() << LOG_ENDL;
    }
}

int main() {
    try {
        glob_config = readConfigFile("server.conf");
        checkConfig(glob_config);
    } catch (const exception &e) {
        LOG_FAIL << e.what() << LOG_ENDL;
        return 0;
    }
    int local_server_port = glob_config["port"].asInt();
    int timeout = glob_config["timeout"].asInt();
    java_server_config = glob_config["java-server"];
    TcpServer server(local_server_port, recv_cb);
    thread t([&server, timeout]() {
        while (true) {
            try {
                online_map_sendall.clear();
                map_mutex.lock();
                online_map_sendall = online_map;
                map_mutex.unlock();
                for (auto &i: online_map_sendall) {
                    if (!i.second.second) {
                        server.disConnect(i.first);
                        continue;
                    }
                    server.sendAll(i.first, "ping");
                    i.second.second = false;
                }
            } catch (const exception &e) {
                LOG_ERROR << e.what() << LOG_ENDL;
            }
            this_thread::sleep_for(chrono::seconds(timeout));
        }
    });
    server.setCloseCallBack(close_cb);
    LOG_INFO << "启动socket服务器中于端口：" << local_server_port << LOG_ENDL;
    LOG_INFO << "心跳机制间隔时间：" << timeout << "秒" << LOG_ENDL;
    LOG_DEBUG << "------------------------------------------------------" << LOG_ENDL;
    LOG_INFO << "java服务端url为：http://" << java_server_config["ip"].asString() << ":"
             << java_server_config["port"].asInt() << java_server_config["request-url"].asString()
             << LOG_ENDL;
    if (!server.run()) {
        LOG_FAIL << "启动失败：" << server.getError() << LOG_ENDL;
    }
    return 0;
}
