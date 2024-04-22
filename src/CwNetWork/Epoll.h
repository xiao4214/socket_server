#pragma once

#include <sys/epoll.h>

namespace CwNetWork {

    class Epoll {
    public:

        /**
          * @brief  用于创建一个epoll对象，维护epollfd和epoll_event数组上下文
          * @param  max_size设置为每次wait返回的最大数量
          */
        explicit Epoll(int max_size = 128);

        ~Epoll() = default;

        Epoll(const Epoll &) = default;

        Epoll(Epoll &&) noexcept = default;

        Epoll &operator=(const Epoll &) = default;

        /**
          * @brief  获取第index个事件的引用
          * @note   如果index超出最大epoll_event数组长度，会抛出std::out_of_range异常
          * @param  index索引
          * @retval 对应的epoll_event结构体的引用
          */
        struct epoll_event &operator[](int index) const;

        /**
          * @brief  将一个指定的epoll_event加入到Epoll对象的红黑树上
          * @param  要加入检测的文件描述符、关心的事件
          * @retval 是否成功将事件添加到Epoll对象上
          */
        bool add(int, uint32_t);

        /**
          * @brief  将一个指定的epoll_event加入到Epoll对象的红黑树上
          * @param  要加入检测的文件描述符、关心的事件、用户自定义类型指针
          * @retval 是否成功将事件添加到Epoll对象上
          */
        bool add(int, uint32_t, void *);

        /**
          * @brief  将Epoll对象上与指定的fd修改
          * @param  要加入检测的文件描述符、关心的事件
          * @retval 是否成功修改
          */
        bool mod(int, uint32_t);

        /**
          * @brief  将Epoll对象上与指定的fd修改
          * @param  要加入检测的文件描述符、关心的事件、用户自定义类型指针
          * @retval 是否成功修改
          */
        bool mod(int, uint32_t, void *);

        /**
          * @brief  将Epoll对象上指定的fd对应的epoll_event删除
          * @note   该函数在删除指定的fd后不会自动关闭该fd，请手动关闭
          * @param  要删除的fd
          * @retval 是否成功删除
          */
        bool del(int fd) const;

        /**
          * @brief  等待epoll文件描述符上的I/O事件
          * @param  参数timeout是超时时间(毫秒，0会立即返回，-1是永久阻塞)
          * @retval 就绪事件个数
          */
        int wait(int timeout);

        /**
          * @brief  释放该Epoll对象的内存
          * @note  请确保对同一个Epoll对象只调用一次(拷贝对象属于同一个)
          */
        void freeEpoll();

    private:

        // epoll文件描述符
        int fd_;

        // 每次wait返回的epoll_event的最大数量
        int max_size_;

        // epoll_event事件集合
        struct epoll_event *event_array_;

        // epoll缓存事件
        struct epoll_event event{};

    };

}