#pragma once

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <string>

namespace vcf_tool {
namespace utils {

/**
 * Format - Generic wrapper around fmt library for string formatting
 *
 * Provides convenient string formatting functionality without
 * directly exposing fmt implementation details.
 */
class Format {
public:
    /**
     * Format a string using fmt syntax
     */
    template<typename... Args>
    static std::string format(fmt::format_string<Args...> fmt_str, Args&&... args) {
        return fmt::format(fmt_str, std::forward<Args>(args)...);
    }

    /**
     * Format to an output iterator
     */
    template<typename OutputIt, typename... Args>
    static OutputIt format_to(OutputIt out, fmt::format_string<Args...> fmt_str, Args&&... args) {
        return fmt::format_to(out, fmt_str, std::forward<Args>(args)...);
    }

    /**
     * Format with a runtime format string
     */
    template<typename... Args>
    static std::string vformat(const std::string& fmt_str, Args&&... args) {
        return fmt::vformat(fmt_str, fmt::make_format_args(args...));
    }
};

// Convenience function that can be used directly
template<typename... Args>
inline std::string format(fmt::format_string<Args...> fmt_str, Args&&... args) {
    return Format::format(fmt_str, std::forward<Args>(args)...);
}

} // namespace utils
} // namespace vcf_tool
