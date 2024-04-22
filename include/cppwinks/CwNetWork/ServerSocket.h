#pragma once

#include "Socket.h"

namespace CwNetWork {

    class ServerSocket {

    public:

        ~ServerSocket() = default;

        ServerSocket(ServerSocket &) = default;

        ServerSocket(ServerSocket &&) noexcept = default;

        ServerSocket &operator=(ServerSocket &&) noexcept = default;

        /**
          * @brief  构造一个服务端套接字对象
          * @retval ServerSocket
          */
        static ServerSocket newServerSocket() { return {}; }

        /**
          * @brief  开启服务端套接字端口可重用
          * @retval 是否开启成功
          */
        bool setSockReuable() const;

        /**
          * @brief  设置socket非阻塞
          */
        void setNonBlock() const;

        /**
          * @brief  将该服务端套接字对象与本机的指定端口绑定
          * @param  绑定的端口号
          * @retval 是否成功绑定
          */
        bool serverBind(int) const;

        /**
          * @brief  服务端开启监听绑定好的端口
          * @param  参数为等待队列最大长度
          * @retval 是否成功监听
          */
        bool serverListen(int) const;

        /**
          * @brief  接受一个客户端的连接请求
          * @note   该方法为阻塞函数
          * @retval 客户端Socket对象
          */
        Socket serverAccept() const;

        /**
          * @brief  关闭服务端套接字描述符
          * @note   请勿对一个ServerSocket对象重复执行关闭操作
          */
        void closeFd();

        /**
          * @brief  获取服务端套接字描述符
          * @retval 服务端套接字描述符
          */
        int getFd() const { return fd_; }

    private:

        ServerSocket();

        int fd_ = -1;

    };

}