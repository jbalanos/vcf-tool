#pragma once

#include <exception>
#include <string>
#include <utility>
#include <source_location>
#include <fmt/format.h>
#include <vcf_tool/utils/Logger.h>

namespace vcf_tool {
namespace utils {
namespace errors {

/// High-level error categories mapped to exit codes.
enum class ErrorCode : int {
    Ok                = 0,
    Validation        = 10,
    Parsing           = 11,
    FileNotFound      = 20,
    IO                = 21,
    QueueOverflow     = 30,
    ThreadPool        = 40,
    Database          = 50,
    Unknown           = 99
};

/// Component where the error originated.
enum class Component {
    Core,
    CLI,
    IO,
    Parser,
    Queue,
    ThreadPool,
    Database,
    Unknown
};

/// Base class for all custom errors in vcf_tool.
class BaseError : public std::exception {
public:
    BaseError(ErrorCode code,
              std::string message,
              Component component = Component::Unknown,
              std::source_location location = std::source_location::current())
        : code_(code),
          component_(component),
          message_(std::move(message)),
          location_(location) {}

    // Rule of 5: explicit special member functions
    BaseError(const BaseError&) = default;
    BaseError& operator=(const BaseError&) = default;
    BaseError(BaseError&&) noexcept = default;
    BaseError& operator=(BaseError&&) noexcept = default;
    virtual ~BaseError() noexcept = default;

    const char* what() const noexcept override {
        return message_.c_str();
    }

    ErrorCode code() const noexcept {
        return code_;
    }

    Component component() const noexcept {
        return component_;
    }

    const std::string& message() const noexcept {
        return message_;
    }

    const std::source_location& location() const noexcept {
        return location_;
    }

    /// Format location information for debugging
    std::string location_string() const {
        return fmt::format("{}:{} in {}",
                          location_.file_name(),
                          location_.line(),
                          location_.function_name());
    }

    /// Get detailed error info including location
    std::string detailed_message() const {
        return fmt::format("{}\n  at {}", message_, location_string());
    }

private:
    ErrorCode            code_;
    Component            component_;
    std::string          message_;
    std::source_location location_;
};

/// Errors due to invalid user input or invalid configuration.
class ValidationError : public BaseError {
public:
    explicit ValidationError(
        const std::string& msg,
        Component component = Component::CLI,
        std::source_location location = std::source_location::current())
        : BaseError(ErrorCode::Validation,
                    fmt::format("Validation error: {}", msg),
                    component,
                    location) {}
};

/// Generic execution error (runtime issues, not user validation).
class ExecutionError : public BaseError {
public:
    explicit ExecutionError(
        const std::string& msg,
        ErrorCode code = ErrorCode::Unknown,
        Component component = Component::Core,
        std::source_location location = std::source_location::current())
        : BaseError(code,
                    fmt::format("Execution error: {}", msg),
                    component,
                    location) {}

protected:
    // Protected constructor for derived classes that want to provide their own prefix
    ExecutionError(ErrorCode code,
                   std::string formatted_message,
                   Component component,
                   std::source_location location)
        : BaseError(code, std::move(formatted_message), component, location) {}
};

/// Parsing-related errors (e.g., malformed VCF line).
class ParsingError : public ExecutionError {
public:
    explicit ParsingError(
        const std::string& msg,
        Component component = Component::Parser,
        std::source_location location = std::source_location::current())
        : ExecutionError(ErrorCode::Parsing,
                         fmt::format("Parsing error: {}", msg),
                         component,
                         location) {}
};

/// File not found / missing path when required.
class FileNotFoundError : public ExecutionError {
public:
    explicit FileNotFoundError(
        const std::string& path,
        Component component = Component::IO,
        std::source_location location = std::source_location::current())
        : ExecutionError(ErrorCode::FileNotFound,
                         fmt::format("File not found: {}", path),
                         component,
                         location) {}
};

/// IO-related failures (read/write errors, permissions, etc.).
class IOError : public ExecutionError {
public:
    explicit IOError(
        const std::string& msg,
        Component component = Component::IO,
        std::source_location location = std::source_location::current())
        : ExecutionError(ErrorCode::IO,
                         fmt::format("I/O error: {}", msg),
                         component,
                         location) {}
};

/// Queue overflow / backpressure issues.
class QueueOverflowError : public ExecutionError {
public:
    explicit QueueOverflowError(
        const std::string& msg,
        Component component = Component::Queue,
        std::source_location location = std::source_location::current())
        : ExecutionError(ErrorCode::QueueOverflow,
                         fmt::format("Queue overflow: {}", msg),
                         component,
                         location) {}
};

/// Thread pool / concurrency issues (e.g., workers not starting).
class ThreadPoolError : public ExecutionError {
public:
    explicit ThreadPoolError(
        const std::string& msg,
        Component component = Component::ThreadPool,
        std::source_location location = std::source_location::current())
        : ExecutionError(ErrorCode::ThreadPool,
                         fmt::format("Thread pool error: {}", msg),
                         component,
                         location) {}
};

/// DB / storage backend issues (connection, write failure, etc.).
class DatabaseError : public ExecutionError {
public:
    explicit DatabaseError(
        const std::string& msg,
        Component component = Component::Database,
        std::source_location location = std::source_location::current())
        : ExecutionError(ErrorCode::Database,
                         fmt::format("Database error: {}", msg),
                         component,
                         location) {}
};

/// Map ErrorCode to a process exit code.
/// This is stable contract for the CLI.
inline int to_exit_code(ErrorCode code) {
    switch (code) {
        case ErrorCode::Ok:            return 0;
        case ErrorCode::Validation:    return 2;
        case ErrorCode::Parsing:       return 3;
        case ErrorCode::FileNotFound:  return 4;
        case ErrorCode::IO:            return 5;
        case ErrorCode::QueueOverflow: return 6;
        case ErrorCode::ThreadPool:    return 7;
        case ErrorCode::Database:      return 8;
        case ErrorCode::Unknown:
        default:                       return 1;
    }
}

/// Overload for convenience.
inline int to_exit_code(const BaseError& e) {
    return to_exit_code(e.code());
}

// ========== Logging Helper Functions ==========

/// Convert Component enum to human-readable string
inline std::string component_to_string(Component c) {
    switch (c) {
        case Component::Core:       return "Core";
        case Component::CLI:        return "CLI";
        case Component::IO:         return "IO";
        case Component::Parser:     return "Parser";
        case Component::Queue:      return "Queue";
        case Component::ThreadPool: return "ThreadPool";
        case Component::Database:   return "Database";
        case Component::Unknown:
        default:                    return "Unknown";
    }
}

/// Log an error with full context (code, component, location)
inline void log_error(const BaseError& e) {
    std::string formatted = fmt::format(
        "{} [code: {}, component: {}] at {}",
        e.message(),
        static_cast<int>(e.code()),
        component_to_string(e.component()),
        e.location_string()
    );

    utils::Logger::instance().error(formatted);
}

/// Log error and then throw it - useful for logging before propagating
template<typename ErrorType>
[[noreturn]] inline void log_and_throw(const ErrorType& error) {
    static_assert(std::is_base_of_v<BaseError, ErrorType>,
                  "ErrorType must derive from BaseError");
    log_error(error);
    throw error;
}

} // namespace errors
} // namespace utils
} // namespace vcf_tool
