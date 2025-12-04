#include <vcf_tool/utils/Logger.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <vector>

namespace vcf_tool {
namespace utils {

struct Logger::Impl {
    std::shared_ptr<spdlog::logger> logger;
};

// Internal helper (not in header)
static spdlog::level::level_enum to_spdlog_level(Logger::Level level) {
    using L = spdlog::level::level_enum;
    switch (level) {
        case Logger::Level::Trace:    return L::trace;
        case Logger::Level::Debug:    return L::debug;
        case Logger::Level::Info:     return L::info;
        case Logger::Level::Warn:     return L::warn;
        case Logger::Level::Error:    return L::err;
        case Logger::Level::Critical: return L::critical;
    }
    return L::info;
}

// Constructor: minimal console logger so logging works even without initialize()
Logger::Logger()
    : impl_(std::make_unique<Impl>())
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");

    impl_->logger = std::make_shared<spdlog::logger>("vcf_tool", console_sink);
    spdlog::register_logger(impl_->logger);

    impl_->logger->set_level(spdlog::level::info);
    impl_->logger->flush_on(spdlog::level::warn);
}

Logger::~Logger() {
    spdlog::drop("vcf_tool");
}

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::initialize(const std::string& log_file_path, Level level) {
    auto& inst = instance(); // ensures singleton is constructed

    std::vector<spdlog::sink_ptr> sinks;

    // Console sink (always)
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
    sinks.push_back(console_sink);

    // Optional rotating file sink
    if (!log_file_path.empty()) {
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            log_file_path,
            10 * 1024 * 1024, // 10 MB max size
            3                 // keep 3 rotated files
        );
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
        sinks.push_back(file_sink);
    }

    // Create a new logger with the chosen sinks
    auto new_logger = std::make_shared<spdlog::logger>("vcf_tool", sinks.begin(), sinks.end());

    // Optional: if you want global default level/pattern for spdlog as well
    // spdlog::set_default_logger(new_logger);

    new_logger->set_level(to_spdlog_level(level));
    new_logger->flush_on(spdlog::level::warn);

    // Replace the underlying logger
    spdlog::drop("vcf_tool"); // drop old one if present
    spdlog::register_logger(new_logger);
    inst.impl_->logger = std::move(new_logger);
}

void Logger::set_level(Level level) {
    impl_->logger->set_level(to_spdlog_level(level));
}

void Logger::trace(const std::string& message)    { impl_->logger->trace(message); }
void Logger::debug(const std::string& message)    { impl_->logger->debug(message); }
void Logger::info(const std::string& message)     { impl_->logger->info(message); }
void Logger::warn(const std::string& message)     { impl_->logger->warn(message); }
void Logger::error(const std::string& message)    { impl_->logger->error(message); }
void Logger::critical(const std::string& message) { impl_->logger->critical(message); }

} // namespace utils
} // namespace vcf_tool
