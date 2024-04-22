#include "Socket.h"
#include <unistd.h>
#include <fcntl.h>

using namespace std;
using namespace CwNetWork;

void Socket::setNonBlock() const {
    int flag = fcntl(fd_, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(fd_, F_SETFL, flag);
}

bool Socket::connectToHost(const char *ip, unsigned short port) const {
    AddrInfo info(ip, port);
    bool ret = connect(fd_, info.getSockAddrPtr(), kSocklen);
    return !ret;
}

bool Socket::connectToHost(const AddrInfo &server_info) const {
    bool ret = connect(fd_, server_info.getSockAddrPtr(), kSocklen);
    return !ret;
}

bool Socket::closeFd() const {
    bool ret = close(fd_);
    return !ret;
}

Socket Socket::newSocket() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    return Socket(fd);
}

size_t Socket::sendAll(const string &msg) const {
    return send(fd_, msg.c_str(), msg.size(), 0);
}
