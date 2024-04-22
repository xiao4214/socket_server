#include "ServerSocket.h"
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

using namespace std;
using namespace CwNetWork;

ServerSocket::ServerSocket() {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
}

bool ServerSocket::setSockReuable() const {
    int flag = 1;
    bool ret = setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
    return !ret;
}

void ServerSocket::setNonBlock() const {
    int flag = fcntl(fd_, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(fd_, F_SETFL, flag);
}

bool ServerSocket::serverBind(int port) const {
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    bool ret = bind(fd_, (struct sockaddr *) &servaddr, sizeof(servaddr));
    return !ret;
}

bool ServerSocket::serverListen(int n) const {
    bool ret = listen(fd_, n);
    return !ret;
}

Socket ServerSocket::serverAccept() const {
    AddrInfo info;
    int client_fd = accept(fd_, info.getSockAddrPtr(), &kSocklen);
    return Socket(client_fd, info);
}

void ServerSocket::closeFd() {
    close(fd_);
}
