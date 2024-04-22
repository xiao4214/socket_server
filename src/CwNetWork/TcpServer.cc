#include "TcpServer.h"
#include <cstring>
#include <utility>

using namespace std;
using namespace CwNetWork;

TcpServer::TcpServer() {
    accept_cb_ = acceptCallBack;
}

TcpServer::TcpServer(unsigned short port, RecvCallBack recv_cb, size_t rbuf_size)
        : TcpServer() {
    port_ = port;
    recv_cb_ = std::move(recv_cb);
    rbuf_size_ = rbuf_size;
}

TcpServer::TcpServer(unsigned short port, RecvCallBack recv_cb, AcceptCallBack accept_cb, size_t rbuf_size)
        : TcpServer(port, std::move(recv_cb), rbuf_size) {
    accept_cb_ = std::move(accept_cb);
}

TcpServer::TcpServer(unsigned short port, RecvCallBack recv_cb, AcceptCallBack accept_cb, int backlog,
                     size_t rbuf_size) : TcpServer(port, std::move(recv_cb), std::move(accept_cb), rbuf_size) {
    backlog_ = backlog;
}

TcpServer::~TcpServer() {
    server_socket_.closeFd();
    epoll_.freeEpoll();
    delete[]rbuf_;
}

void TcpServer::sendAll(int fd, const string &message) {
    string all = clients_sbuf_.at(fd).append(message);
    ssize_t slen = send(fd, all.c_str(), all.size(), 0);
    if (slen != -1) {
        clients_sbuf_.at(fd).erase(0, slen);
    }
}

bool TcpServer::run() {
    if (!initServer()) {
        return false;
    }
    int ev_num = 0, i = 0, fd = 0;
    while (true) {
        ev_num = epoll_.wait(-1);
        for (i = 0; i < ev_num; ++i) {
            fd = epoll_[i].data.fd;
            if (fd == server_socket_.getFd()) {
                Socket client = acceptCallBack(server_socket_);
                client.setNonBlock();
                epoll_.add(client.getFd(), EPOLLIN | EPOLLOUT | EPOLLET);
                clients_.emplace(client.getFd(), client);
                clients_sbuf_.emplace(client.getFd(), string());
            } else if (epoll_[i].events & EPOLLIN) {
                string message;
                ssize_t rlen = 0;
                while (true) {
                    rlen = recv(fd, rbuf_, rbuf_size_, 0);
                    if (rlen == -1 && errno == EAGAIN) {
                        recv_cb_(clients_.at(fd), message, this);
                        break;
                    } else if (rlen == 0 || rlen == -1) {
                        Socket client = clients_.at(fd);
                        if (close_cb_ != nullptr) {
                            close_cb_(client, this);
                        }
                        epoll_.del(fd);
                        client.closeFd();
                        clients_.erase(fd);
                        break;
                    }
                    message.append(rbuf_, rlen);
                }
            } else if (epoll_[i].events & EPOLLOUT) {
                string message = clients_sbuf_.at(fd);
                ssize_t slen = send(fd, message.c_str(), message.size(), 0);
                clients_sbuf_.at(fd).erase(0, slen);
            }
        }
    }
    return true;
}

bool TcpServer::initServer() {
    error_ = "the server is running normally";
    rbuf_ = new char[rbuf_size_];
    bzero(rbuf_, rbuf_size_);
    if (recv_cb_ == nullptr || accept_cb_ == nullptr) {
        error_ = "the callback functions for receiving messages and accepting connections are not set";
        return false;
    }
    if (!server_socket_.setSockReuable()) {
        error_ = "the port multiplexing setting failed";
        return false;
    }
    if (!server_socket_.serverBind(port_)) {
        error_ = "failed to bind the port";
        return false;
    }
    if (!server_socket_.serverListen(backlog_)) {
        error_ = "listening failed";
        return false;
    }
    if (!epoll_.add(server_socket_.getFd(), EPOLLIN)) {
        error_ = "failed to add server-side sockets to the epoll model";
        return false;
    }
    return true;
}

Socket TcpServer::acceptCallBack(const ServerSocket &server_socket) {
    return server_socket.serverAccept();
}

void TcpServer::disConnect(int client_fd) {
    epoll_.del(client_fd);
    clients_.at(client_fd).closeFd();
    clients_.erase(client_fd);
    clients_sbuf_.erase(client_fd);
}
