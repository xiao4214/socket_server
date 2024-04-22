#include "Epoll.h"
#include <unistd.h>
#include <stdexcept>

using namespace std;
using namespace CwNetWork;

Epoll::Epoll(int max_size) {
    fd_ = epoll_create(1);
    max_size_ = max_size;
    event_array_ = new struct epoll_event[max_size_];
}

struct epoll_event &Epoll::operator[](int index) const {
    if (index >= max_size_) {
        throw out_of_range("out of max epoll_event array length");
    }
    return event_array_[index];
}

bool Epoll::add(int fd, uint32_t events) {
    event.events = events;
    event.data.fd = fd;
    bool ret = epoll_ctl(fd_, EPOLL_CTL_ADD, fd, &event);
    return !ret;
}

bool Epoll::add(int fd, uint32_t events, void *ptr) {
    event.events = events;
    event.data.ptr = ptr;
    bool ret = epoll_ctl(fd_, EPOLL_CTL_ADD, fd, &event);
    return !ret;
}

bool Epoll::mod(int fd, uint32_t events) {
    event.events = events;
    event.data.fd = fd;
    bool ret = epoll_ctl(fd_, EPOLL_CTL_MOD, fd, &event);
    return !ret;
}

bool Epoll::mod(int fd, uint32_t events, void *ptr) {
    event.events = events;
    event.data.ptr = ptr;
    bool ret = epoll_ctl(fd_, EPOLL_CTL_MOD, fd, &event);
    return !ret;
}

bool Epoll::del(int fd) const {
    bool ret = epoll_ctl(fd_, EPOLL_CTL_DEL, fd, nullptr);
    return !ret;
}

int Epoll::wait(int timeout) {
    return epoll_wait(fd_, event_array_, max_size_, timeout);
}

void Epoll::freeEpoll() {
    close(fd_);
    delete[]event_array_;
}
