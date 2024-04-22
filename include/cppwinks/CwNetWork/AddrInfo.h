#pragma once

#include <string>
#include <arpa/inet.h>

namespace CwNetWork {

    static socklen_t kSocklen = sizeof(struct sockaddr_in);

    class AddrInfo {

    public:

        AddrInfo() = default;

        AddrInfo(const AddrInfo &) = default;

        AddrInfo(AddrInfo &&) = default;

        AddrInfo &operator=(const AddrInfo &) = default;

        /**
          * @brief  根据一个网络地址构造网络地址对象
          * @param  原生sockaddr_in结构体
          */
        explicit AddrInfo(sockaddr_in addr) : addr_(addr) {}

        /**
          * @brief  根据参数构造一个网络地址对象
          * @note   None
          * @param  _1:网路地址对象的ip _2:网络地址对象的端口
          * @retval None
          */
        AddrInfo(const char *, unsigned short);

        /**
          * @brief  根据参数构造一个网络地址对象
          * @param  _1:网路地址对象的ip _2:网络地址对象的端口
          */
        AddrInfo(const std::string &ip, unsigned short port) : AddrInfo(ip.c_str(), port) {}

        /**
          * @brief  获取该网络地址对象的ip
          * @retval C++风格字符串
          */
        std::string getIp() const { return {inet_ntoa(addr_.sin_addr)}; }

        /**
          * @brief  获取该网络地址对象的端口
          * @retval 端口号的值
          */
        unsigned short getPort() const { return ntohs(addr_.sin_port); }

        /**
          * @brief  设置sockaddr_in成员
          * @param  要设置的sockaddr_in
          */
        void setSockAddr_in(struct sockaddr_in addr) { addr_ = addr; }

        /**
          * @brief  获取该网络地址的sockaddr指针用于其他接口的参数传递
          * @retval struct sockaddr *
          */
        struct sockaddr *getSockAddrPtr() const { return (struct sockaddr *) &addr_; }

    private:

        struct sockaddr_in addr_ = {0};

    };

}
