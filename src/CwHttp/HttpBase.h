#pragma once

#include <string>
#include <unordered_map>

namespace CwHttp {

    class HttpBase {

    public:

        HttpBase() = default;

        virtual ~HttpBase() = default;

        /**
          * @brief  获取Http版本号
          * @retval string
          */
        const std::string &getVersion() const { return version_; }

        /**
          * @brief  设置Http版本号
          * @param  Http的版本号
          */
        void setVersion(const std::string &version) { version_ = version; }

        /**
          * @brief  按照传入参数重置全部Http头
          * @note   使用了std::move，将会移动传入参数!
          * @param  新的全部Http头
          */
        void resetHeader(std::unordered_map<std::string, std::string> header) { header_ = std::move(header); }

        /**
          * @brief  获取全部Http头
          * @retval 一个包含全部Http头的std::unordered_map<std::string, std::string>类型
          */
        std::unordered_map<std::string, std::string> getAllHeader() const { return header_; }

        /**
          * @brief  添加一个头
          * @param  一个头的key和val
          * @retval 是否成功添加
          */
        bool addHeader(const std::string &key, const std::string &val);

        /**
          * @brief  根据key删除一个头
          * @param  一个头的key
          * @retval 是否成功删除
          */
        bool removeHeader(const std::string &key) { return header_.erase(key) != 0; }

        /**
          * @brief  获取头对应的值
          * @param  一个头的key
          * @retval 该key对应的val，如果不存在返回空字符串
          */
        std::string getHeader(const std::string &key) const;

        /**
          * @brief  将对应key的头的val修改为指定的值
          * @param  要修改的头的key和新的val
          * @retval 是否成功修改
          */
        bool setHeader(const std::string &key, const std::string &val);

        /**
          * @brief  设置Http体
          * @param  要设置的体
          */
        void setBody(const std::string &body) { body_ = body; }

        /**
          * @brief  获取Http体
          * @retval Http体
          */
        std::string getBody() const { return body_; }

        /**
          * @brief  将Http请求类转化为字符串
          * @retval 一个字符串
          */
        virtual std::string toString() const = 0;

    private:

        //Http版本号
        std::string version_ = "HTTP/1.1";
        //Http头部
        std::unordered_map<std::string, std::string> header_;
        //Http体
        std::string body_;

    };
}
