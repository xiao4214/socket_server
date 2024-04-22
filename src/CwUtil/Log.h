#pragma once

#include <iostream>
#include <cstring>

#define NONE               "\e[0m"    // 无色
#define RED                "\e[0;31m" // 失败
#define L_RED              "\e[1;31m" // 错误
#define GREEN              "\e[1;32m" // 调试
#define YELLOW             "\e[1;33m" // 警告
#define BLUE               "\e[1;34m" // 信息
#define PURPLE             "\e[1;35m" // 预留颜色
#define CYAN               "\e[1;36m" // 预留颜色
#define GRAY               "\e[0;37m" // 预留颜色

// 获取系统当前时间
std::string getCurrentTime();

// 截取文件名
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)
// 调试日志
#define LOG_DEBUG std::cout << GREEN << getCurrentTime() << ' ' << __FILENAME__ << ":" << __LINE__ <<' '
// 信息日志
#define LOG_INFO std::cout << BLUE << getCurrentTime() << ' ' << __FILENAME__ << ":" << __LINE__ <<' '
// 警告日志
#define LOG_WARN std::cout << YELLOW << getCurrentTime() << ' ' << __FILENAME__ << ":" << __LINE__ <<' '
// 错误日志
#define LOG_ERROR std::cout << L_RED << getCurrentTime() << ' ' << __FILENAME__ << ":" << __LINE__ <<' '
// 失败日志
#define LOG_FAIL std::cout << RED << getCurrentTime() << ' ' << __FILENAME__ << ":" << __LINE__ <<' '
// 日志结束
#define LOG_END NONE
// 日志结束并换行
#define LOG_ENDL "\e[0m\n"
