// 日志系统类定义
// 提供线程安全的日志记录功能，支持不同类型的日志消息
#pragma once
#ifndef __NR_LOGGER_HPP__
#define __NR_LOGGER_HPP__

#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <ctime>
#include <mutex>

#include "common/macros.hpp"

#undef ERROR

namespace NRenderer
{
    using namespace std;
    // 日志记录器类
    // 管理系统日志，支持多种日志类型和线程安全的日志记录
    class DLL_EXPORT Logger
    {
    public:
        // 日志类型枚举
        // 定义了不同级别的日志消息
        enum class LogType
        {
            NORMAL,     // 普通信息
            WARNING,    // 警告信息
            ERROR,      // 错误信息
            SUCCESS     // 成功信息
        };

        // 日志文本结构体
        // 存储单条日志的类型和内容
        struct LogText
        {
            LogType type;    // 日志类型
            string message;  // 日志内容

            LogText() = delete;
            // 创建普通类型的日志
            LogText(const string& str)
                : type          (LogType::NORMAL)
                , message       (str)
            {}
            // 创建指定类型的日志
            LogText(LogType type, const string& str)
                : type          (type)
                , message       (str)
            {}
        };
    private:
        vector<LogText> msgs;    // 日志消息列表
        mutex   mtx;             // 互斥锁，保证线程安全

    public:
        // 默认构造函数
        Logger();
        ~Logger() = default;
        // 禁用拷贝和移动构造
        Logger(const Logger&) = delete;
        Logger(Logger&&) = delete;

        // 记录指定类型的日志
        void log(const string& msg, LogType type);

        // 记录普通日志
        void log(const string& msg);

        // 记录警告日志
        void warning(const string& msg);

        // 记录错误日志
        void error(const string& msg);

        // 记录成功日志
        void success(const string& msg);

        // 清空所有日志
        void clear();

        // 日志消息集合结构体
        // 用于批量获取日志消息
        struct LogMessages
        {
            unsigned nums;   // 日志消息数量
            LogText* msgs;   // 日志消息数组
        };

        // 获取所有日志消息
        LogMessages get();
    };
    using SharedLogger = shared_ptr<Logger>;  // 日志记录器的智能指针类型
} // namespace NRenderer


#endif