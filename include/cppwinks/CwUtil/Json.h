#pragma once

#include <map>
#include <memory>
#include <vector>

namespace CwUtil {

    class Json {

    public:

        // Json枚举类型
        enum Type {
            Json_null = 0,
            Json_bool,
            Json_int,
            Json_double,
            Json_string,
            Json_array,
            Json_object
        };

        /**
          * @brief  默认构造函数
          * @note   构造出一个空类型的Json
          */
        Json();

        /**
          * @brief  不同类型的有参构造函数
          */
        Json(bool value);

        Json(int value);

        Json(double value);

        Json(const char *value);

        Json(const std::string &value);

        Json(const Json &json) = default;

        /**
          * @brief  运算符重载
          * @Note   如果类型不对应将会抛出logic_error异常
          */
        operator bool();

        operator std::string();

        operator int();

        operator double();

        /**
          * @brief  获取当前Json节点的类型
          * @retval Json类型枚举
          */
        Type type() const { return type_; }

        /**
          * @brief  允许使用索引方式直接访问数组类型中的元素
          * @note   如果节点本身不是数组类型将会抛出logic_error异常，如果索引错误将会抛出out_of_range异常
          * @param  索引
          * @retval Json类型节点
          */
        Json &operator[](int index);

        /**
          * @brief  允许使用中括号运算符访问对象中指定键的值
          * @note   如果不是对象类型将抛出logic_error异常
          * @param  字符串
          * @retval Json类型节点
          */
        Json &operator[](const char *);

        Json &operator[](const std::string &);

        /**
          * @brief  获取数组类型的元素个数
          * @note   如果节点不是数组类型，将会抛出logic_error异常
          * @retval 数组元素个数(长度)
          */
        size_t length() const;

        /**
          * @brief  向数组中尾插一个Json类型的节点
          * @note   如果节点不是数组类型，将会抛出logic_error异常
          * @param  Json类型节点
          */
        void append(const Json &);

        /**
          * @brief  将节点转换为字符串类型
          * @retval 字符串
          */
        std::string toString() const;

        /**
          * @brief  将节点转换为基础数据类型
          * @note   如果类型错误将会抛出logic_error异常
          * @retval 基础数据类型
          */
        bool asBool() const;

        int asInt() const;

        double asDouble() const;

        std::string asString() const;


        /**
          * @brief  检查节点的类型
          * @retval 是否与要检查的类型一致
          */
        bool isNull() const { return type_ == Json_null; }

        bool isInt() const { return type_ == Json_int; }

        bool isDouble() const { return type_ == Json_double; }

        bool isBool() const { return type_ == Json_bool; }

        bool isString() const { return type_ == Json_string; }

        bool isObject() const { return type_ == Json_object; }

        bool isArray() const { return type_ == Json_array; }

        /**
          * @brief  检查数组类型的节点中是否包含指定索引的元素
          * @param  要检查的索引
          * @retval 是否存在该元素
          */
        bool has(int) const;

        /**
          * @brief  检查对象类型的节点中是否存在指定的键值
          * @param  要检查的键
          * @retval 是否存在指定的键
          */
        bool has(const char *) const;

        bool has(const std::string &) const;

        /**
          * @brief  移除数组类型节点中的指定索引的节点元素
          * @note   如果不存在或类型错误不会抛出异常，不会对自身节点造成任何影响
          * @param  要移除的指定节点的索引
          */
        void remove(int);

        /**
          * @brief  移除对象类型节点中的指定键的节点
          * @note   如果不存在或类型错误不会抛出异常，不会对自身节点造成任何影响
          * @param  要移除的键
          */
        void remove(const char *);

        void remove(const std::string &);

        /**
          * @brief  静态函数，将字符串处理为Json节点
          * @note   处理失败将抛出logic_error异常
          * @param  要处理的Json格式字符串
          * @retval Json类型的节点
          */
        static Json parseJson(const std::string &);

        static Json parseJson(const char *);

    private:

        union Value {
            bool bool_;
            int int_;
            double double_;
            std::string *string_;
            std::vector<Json> *array_;
            std::map<std::string, Json> *object_;
        };

        static void stringDeleter(Value *value) { delete value->string_; }

        static void arrayDeleter(Value *value) { delete value->array_; }

        static void objectDeleter(Value *value) { delete value->object_; }

        static Json parseValue(std::stringstream &);

        static Json parseString(std::stringstream &);

        static Json parseObject(std::stringstream &);

        static Json parseArray(std::stringstream &);

        static Json parseNumber(std::stringstream &);

        static Json parseBool(std::stringstream &);

        Type type_;

        std::shared_ptr<Value> value_;

    };

}