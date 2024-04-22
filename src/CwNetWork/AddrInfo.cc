#include "AddrInfo.h"

using namespace CwNetWork;

AddrInfo::AddrInfo(const char *ip, unsigned short port) {
    addr_.sin_family = AF_INET;  //使用IPv4地址
    addr_.sin_addr.s_addr = inet_addr(ip);  //具体的IP地址
    addr_.sin_port = htons(port);
}
