#pragma once

#include "AddrInfo.h"

namespace CwNetWork {

    class Socket {

    public:

        /**
          * @brief  构造一个用于连接服务端的Socket对象
          * @retval Socket对象
          */
        static Socket newSocket();

        /**
          * @brief  根据提供的套接字描述符创建Socket对象
          * @param  一个套接字描述符
          */
        explicit Socket(int fd) : fd_(fd) {}

        /**
          * @brief  根据指定的套接字描述符和网络地址构造一个Socket对象
          * @param  指定的套接字描述符，网络地址
          */
        Socket(int fd, AddrInfo info) : fd_(fd), addr_info(info) {}

        ~Socket() = default;

        /**
          * @brief  设置socket非阻塞
          */
        void setNonBlock() const;

        /**
          * @brief  获取该套接字描述符
          * @retval 套接字描述符
          */
        int getFd() const { return fd_; }

        /**
          * @brief  设置套接字
          * @param  新的套接字
          */
        void setFd(int fd) { fd_ = fd; }

        /**
          * @brief  向指定的ip和端口发起tcp连接
          * @param  _1:要连接的主机ip _2:要连接的主机端口
          * @retval 是否连接成功
          */
        bool connectToHost(const char *, unsigned short) const;

        /**
          * @brief  向指定的ip和端口发起tcp连接
          * @param  _1:要连接的主机ip _2:要连接的主机端口
          * @retval 是否连接成功
          */
        bool connectToHost(const std::string &ip, unsigned short port) const {
            return connectToHost(ip.c_str(), port);
        }

        /**
          * @brief  向指定的网络地址信息发起tcp连接
          * @param  AddrInfo
          * @retval 是否连接成功
          */
        bool connectToHost(const AddrInfo &) const;

        /**
          * @brief  断开tcp连接/关闭套接字描述符
          * @note   请勿对一个Socket对象重复执行关闭操作
          * @retval 是否断开成功
          */
        bool closeFd() const;

        /**
          * @brief  发送数据
          * @param  要发送的数据
          * @retval 成功发送的字节数
          */
        size_t sendAll(const std::string &) const;

        // 网络地址信息
        AddrInfo addr_info;

    private:

        Socket() = default;

        int fd_ = -1;

    };
}