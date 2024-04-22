#pragma once

#include "ServerSocket.h"
#include "Epoll.h"
#include <unordered_map>
#include <functional>
#include <utility>

namespace CwNetWork {

    class TcpServer {

    public:

        /*
         * 回调函数第一个参数为当前消息发送的Socket对象
         * 回调函数第二个参数为接收到的消息
         * 回调函数第三个参数为服务端对象的指针常量
         */
        using RecvCallBack = std::function<void(Socket, const std::string &, TcpServer *const)>;

        /*
         * 回调函数要求返回一个已经建立好连接的客户端Socket对象
         * 回调函数参数为服务端ServerSocket对象
         */
        using AcceptCallBack = std::function<Socket(const ServerSocket &)>;

        /*
         * 回调函数第一个参数为即将断开连接的客户端对象
         * 回调函数第二个参数为服务端对象的指针常量
         */
        using CloseCallBack = std::function<void(const Socket &, TcpServer *const)>;

        /**
          * @brief  按默认参数构造一个Tcp服务端
          */
        TcpServer();

        /**
          * @brief  根据指定端口和指定接收数据回调函数构造一个Tcp服务器
          * @param  端口和接受数据回调函数
          */
        TcpServer(unsigned short, RecvCallBack, size_t rbuf_size = 4096);

        /**
          * @brief  根据指定端口、指定接收数据回调函数和接受连接回调函数构造一个Tcp服务器
          * @param  端口、接受数据回调函数和接受连接回调函数
          */
        TcpServer(unsigned short, RecvCallBack, AcceptCallBack, size_t rbuf_size = 4096);

        /**
          * @brief  根据指定端口、指定接收数据回调函数、接受连接回调函数和连接等待队列长度构造一个Tcp服务器
          * @param  端口、接受数据回调函数、接受连接回调函数和连接等待队列长度
          */
        TcpServer(unsigned short, RecvCallBack, AcceptCallBack, int, size_t rbuf_size = 4096);

        virtual ~TcpServer();

        /**
          * @brief  设置接收数据回调函数
          * @param  RecvCallBack
          */
        void setRecvCallBack(RecvCallBack recv_callback) { recv_cb_ = std::move(recv_callback); }

        /**
          * @brief  设置接受连接回调函数
          * @note   该回调结束后会自动将客户端Socket设置为非阻塞
          * @param  AcceptCallBack => Socket(const ServerSocket &)
          */
        void setAcceptCallBack(AcceptCallBack accept_callback) { accept_cb_ = std::move(accept_callback); }

        /**
          * @brief  设置即将断开连接的回调函数
          * @note   该回调执行时socket还未完成连接的断开，请勿手动操作断开或向缓冲区写入数据
          * @param  CloseCallBack => void(const Socket &, TcpServer *const)
          */
        void setCloseCallBack(CloseCallBack close_callback){close_cb_ = std::move(close_callback);}

        /**
          * @brief  设置等待队列最大大小
          * @param  等待队列最大大小
          */
        void setBackLogSize(int backlog) { backlog_ = backlog; }

        /**
          * @brief  设置接收缓冲区最大长度
          * @param  接收缓冲区最大长度
          */
        void setRbufSize(size_t rbuf_size) { rbuf_size_ = rbuf_size; }

        /**
          * @brief  向指定的套接字描述符发送数据
          * @note   如果对端不可写，该方法将会异步等待epoll调用
          * @param  指定的套接字描述符、要发送的数据
          */
        void sendAll(int, const std::string &);

        /**
          * @brief  向指定的Socket对象发送数据
          * @note   如果对端不可写，该方法将会异步等待epoll调用
          * @param  指定的Socket对象、要发送的数据
          */
        void sendAll(Socket client, const std::string &message) { sendAll(client.getFd(), message); }

        /**
          * @brief  根据指定的客户端文件描述符断开连接
          * @note   该函数将管理要断开的文件描述符的全部生命周期
          * @param  指定的客户端文件描述符
          */
        void disConnect(int);

        /**
          * @brief  根据指定的客户端对象断开连接
          * @note   该函数将管理要断开的客户端对象的全部生命周期
          * @param  指定的客户端对象
          */
        void disConnect(const Socket &client) { disConnect(client.getFd()); }

        /**
          * @brief  获取服务端启动失败原因
          * @retval 服务端启动失败原因的描述
          */
        std::string getError() const { return error_; }

        /**
          * @brief  获取已连接的客户端对象键值对
          * @retval 已连接的客户端对象键值对
          */
        std::unordered_map<int, Socket> getClients() const { return clients_; }

        /**
          * @brief  启动Tcp服务端
          * @note   该函数为阻塞函数
          */
        virtual bool run();

    private:

        /**
          * @brief  接受客户端连接的回调函数
          * @note   accept_cb_默认指向该函数，可被用户修改指向
          * @param  ServerSocket对象
          * @retval 建立连接的Socket对象
          */
        static Socket acceptCallBack(const ServerSocket &);

        /**
          * @brief  调用run方法启动服务器前将进行初始化服务器
          * @note   如果失败可通过getError方法获取失败原因
          * @retval 是否成功初始化服务器
          */
        bool initServer();

    private:

        // 维护服务端套接字对象
        ServerSocket server_socket_ = ServerSocket::newServerSocket();
        // 服务端IO多路复用模型
        Epoll epoll_;
        // 维护已连接的客户端信息
        std::unordered_map<int, Socket> clients_;
        // 维护已连接的客户端端发送缓冲区
        std::unordered_map<int, std::string> clients_sbuf_;
        // 等待队列最大长度
        int backlog_ = 128;
        // 服务器开启端口
        unsigned short port_ = 8080;
        // 接收缓冲区指针
        char *rbuf_ = nullptr;
        // 单次IO事件最大读出数据大小
        size_t rbuf_size_ = 4096;
        // 收到客户端数据时执行的回调函数
        RecvCallBack recv_cb_ = nullptr;
        // 新的连接到来时执行的回调函数
        AcceptCallBack accept_cb_ = nullptr;
        // 客户端关闭连接后执行的回调函数
        CloseCallBack close_cb_ = nullptr;
        // 服务端异常日志
        std::string error_ = "the server was not started";

    };

}
