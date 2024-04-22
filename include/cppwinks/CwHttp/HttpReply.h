#pragma once

#include "HttpBase.h"

namespace CwHttp {

    class HttpReply : public HttpBase {

    public:

        /**
          * @brief  构造一个空的Http回复，默认状态码为200，状态为OK
          */
        HttpReply();

        // 赋值构造
        HttpReply &operator=(const HttpReply &);

        // 拷贝构造
        HttpReply(const HttpReply &);

        // 移动构造
        HttpReply(HttpReply &&) noexcept;

        /**
          * @brief  根据指定的状态码构造一个空的Http回复，状态默认为OK
          * @param  接受C和C++两种风格的字符串
          */
        explicit HttpReply(const std::string &status_code);

        explicit HttpReply(const char *status_code);

        /**
          * @brief  根据指定的状态码和状态构造一个空的Http回复
          * @param  C和C++风格字符串
          */
        HttpReply(const char *status_code, const char *status);

        HttpReply(const std::string &status_code, const std::string &status);

        ~HttpReply() override = default;

        /**
          * @brief  设置Http响应状态码
          * @note   包括两种重载形式
          * @param  接受C风格字符串和C++风格字符串
          */
        void setStatusCode(const char *status_code);

        void setStatusCode(const std::string &status_code);

        /**
          * @brief  获取Http响应状态码
          * @retval C++风格字符串
          */
        std::string getStatusCode() const { return {&status_code_[0], &status_code_[3]}; }

        /**
          * @brief  设置Http响应状态
          * @param  C和C++风格字符串
          */
        void setStatus(const std::string &status) { status_ = status; }

        void setStatus(const char *status) { status_ = status; }

        /**
          * @brief  获取Http响应状态
          */
        std::string getStatus() const { return status_; }

        /**
          * @brief  将Http请求类转化为字符串
          * @retval 一个字符串
          */
        std::string toString() const override;

    private:

        // 状态码
        char status_code_[4];
        // 状态
        std::string status_ = "OK";

    };
}
