#pragma once

#include <memory>
#include <string>
#include <fmt/core.h>

namespace vcf_tool {
namespace utils {

class Logger {
public:
    enum class Level {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Critical
    };

    /// Get singleton instance (already has a minimal default configuration)
    static Logger& instance();

    /// Initialize / reconfigure logging (call early at startup).
    /// If log_file_path is empty => only console logging.
    /// If non-empty => console + rotating file sink.
    static void initialize(const std::string& log_file_path,
                           Level level = Level::Info);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    // Runtime log level change (can be used after initialize)
    void set_level(Level level);

    // String-based logging API internally used by fmt helpers/macros
    void trace(const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);

private:
    Logger();
    ~Logger();

    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// ======= fmt-style helper functions (header-only, no spdlog) =======

template <typename... Args>
inline void log_trace(fmt::format_string<Args...> fmt_str, Args&&... args) {
    Logger::instance().trace(fmt::format(fmt_str, std::forward<Args>(args)...));
}

template <typename... Args>
inline void log_debug(fmt::format_string<Args...> fmt_str, Args&&... args) {
    Logger::instance().debug(fmt::format(fmt_str, std::forward<Args>(args)...));
}

template <typename... Args>
inline void log_info(fmt::format_string<Args...> fmt_str, Args&&... args) {
    Logger::instance().info(fmt::format(fmt_str, std::forward<Args>(args)...));
}

template <typename... Args>
inline void log_warn(fmt::format_string<Args...> fmt_str, Args&&... args) {
    Logger::instance().warn(fmt::format(fmt_str, std::forward<Args>(args)...));
}

template <typename... Args>
inline void log_error(fmt::format_string<Args...> fmt_str, Args&&... args) {
    Logger::instance().error(fmt::format(fmt_str, std::forward<Args>(args)...));
}

template <typename... Args>
inline void log_critical(fmt::format_string<Args...> fmt_str, Args&&... args) {
    Logger::instance().critical(fmt::format(fmt_str, std::forward<Args>(args)...));
}

} // namespace utils
} // namespace vcf_tool

// ======= Convenience macros =======
// Plain string macros
#define LOG_TRACE(msg)    ::vcf_tool::utils::Logger::instance().trace(msg)
#define LOG_DEBUG(msg)    ::vcf_tool::utils::Logger::instance().debug(msg)
#define LOG_INFO(msg)     ::vcf_tool::utils::Logger::instance().info(msg)
#define LOG_WARN(msg)     ::vcf_tool::utils::Logger::instance().warn(msg)
#define LOG_ERROR(msg)    ::vcf_tool::utils::Logger::instance().error(msg)
#define LOG_CRITICAL(msg) ::vcf_tool::utils::Logger::instance().critical(msg)

// fmt-style macros using {} placeholders
#define LOG_TRACE_F(fmt_str, ...) ::vcf_tool::utils::log_trace(fmt_str, ##__VA_ARGS__)
#define LOG_DEBUG_F(fmt_str, ...) ::vcf_tool::utils::log_debug(fmt_str, ##__VA_ARGS__)
#define LOG_INFO_F(fmt_str, ...)  ::vcf_tool::utils::log_info(fmt_str,  ##__VA_ARGS__)
#define LOG_WARN_F(fmt_str, ...)  ::vcf_tool::utils::log_warn(fmt_str,  ##__VA_ARGS__)
#define LOG_ERROR_F(fmt_str, ...) ::vcf_tool::utils::log_error(fmt_str, ##__VA_ARGS__)
#define LOG_CRIT_F(fmt_str, ...)  ::vcf_tool::utils::log_critical(fmt_str, ##__VA_ARGS__)
