#pragma once

#include "HttpBase.h"

namespace CwHttp {

    enum class RequestMethod {
        POST, GET, HAND, PUT, DELETE, CONNECT, OPTIONS, TRACE, PATCH
    };

    class HttpRequest : public HttpBase {

    public:

        /**
          * @brief  构造一个空的Http请求
          * @note   该Http请求的方法默认为GET，版本默认为1.1，请求url为/，请求头中的Content-Type默认为text/plain
          */
        HttpRequest() { addHeader("Content-Type", "text/plain"); }

        // 赋值构造函数
        HttpRequest &operator=(const HttpRequest &);

        // 拷贝构造函数
        HttpRequest(const HttpRequest &);

        // 移动构造函数
        HttpRequest(HttpRequest &&) noexcept;

        /**
          * @brief  按照请求方法和请求url构造一个空的Http请求
          * @param  请求方法枚举和请求URL
          */
        HttpRequest(RequestMethod method, std::string url) : method_(method), url_(std::move(url)) {}

        ~HttpRequest() override = default;

        /**
          * @brief  按照请求方法，请求url和指定的Http版本构造一个空的Http请求
          * @param  请求方法枚举，请求URL和Http版本号
          */
        HttpRequest(RequestMethod method, const std::string &url, const std::string &version) : HttpRequest(method,
                                                                                                            url) {
            setVersion(version);
        }

        /**
          * @brief  从字符串中解析Http请求
          * @note   如果解析原文不是Http格式会抛出std::runtime_error
          * @param  要解析的Http请求原始字符串
          * @retval 一个解析完成的Http请求对象的智能指针
          */
        static HttpRequest paresRequest(const std::string &);

        /**
          * @brief  设置请求URL
          * @param  要设置的URL字符串
          */
        void setUrl(const std::string &url) { url_ = url; }

        /**
          * @brief  获取请求URL
          * @retval url字符串
          */
        std::string getUrl() const;

        /**
          * @brief  设置请求方法
          * @param  要设置的请求方法
          */
        void setMethod(RequestMethod method) { method_ = method; }

        /**
          * @brief  获取请求方法
          * @retval RequestMethod枚举
          */
        RequestMethod getMethod() const { return method_; }

        /**
          * @brief  获取请求方法
          * @retval 请求方法的字符串
          */
        std::string getMethodStr() const;

        /**
          * @brief  解析请求url中携带的参数
          * @retval 一个unordered_map<string,string>，第一个模板参数表示url请求参数中的键，第二个模板参数表示url请求参数中的值
          */
        std::unordered_map<std::string, std::string> getUrlParameter() const;

        /**
          * @brief  将Http请求类转化为字符串
          * @retval 一个字符串
          */
        std::string toString() const override;

    private:

        /**
          * @brief  获取Http请求的请求方法
          * @note   如果请求方法不属于Http请求中的一种，则返回RequestMethod::UnKnow
          * @param  要处理的Http请求原文
          * @retval 该Http请求的请求方法
          */
        static RequestMethod getRequestMethod(const std::string &);

        /**
          * @brief  解析Http请求的请求行
          * @note   如果格式非法将抛出std::runtime_error
          * @param  Http请求原文
          * @retval 一个三个成员的std::tuple的指针，分别为请求方法，请求url和Http版本号
          */
        static std::tuple<RequestMethod, std::string, std::string> getRequestLine(const std::string &);

        /**
          * @brief  解析Http请求的请求头
          * @param  Http请求原文
          * @retval 一个std::unordered_map
          */
        static std::unordered_map<std::string, std::string> getRequsetHeader(const std::string &, const size_t);

    private:

        //请求方法
        RequestMethod method_ = RequestMethod::GET;
        // 请求URL
        std::string url_ = "/";

    };

}