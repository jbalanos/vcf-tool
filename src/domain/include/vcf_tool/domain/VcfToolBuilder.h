// VcfToolBuilder.h
#pragma once

#include <cstddef>


namespace vcf_tool::domain::api {

// Forward declaration
class VcfTool;

/**
 * @brief Builder for VcfTool with fluent API and validation
 *
 * Provides:
 * - Fluent configuration API (method chaining)
 * - Parameter validation before construction
 * - Sensible defaults
 * - Preset configurations for common scenarios
 *
 * Example:
 *   auto tool = VcfToolBuilder()
 *       .with_parser_threads(4)
 *       .with_batch_size(5000)
 *       .build();
 */
class VcfToolBuilder {
public:
    VcfToolBuilder();

    // Fluent API - each method returns *this for chaining
    VcfToolBuilder& with_parser_threads(std::size_t n);
    VcfToolBuilder& with_batch_size(std::size_t n);
    VcfToolBuilder& with_line_queue_capacity(std::size_t n);
    VcfToolBuilder& with_record_queue_capacity(std::size_t n);

    // Preset configurations
    static VcfToolBuilder for_large_files();
    static VcfToolBuilder for_low_memory();

    // Build and validate
    VcfTool build() const;

private:
    std::size_t parser_threads_ = 0;  // 0 = auto-detect from hardware_concurrency
    std::size_t batch_size_ = 1000;
    std::size_t line_queue_capacity_ = 20000;
    std::size_t record_queue_capacity_ = 10000;

    // Validation helper
    void validate() const;
};

} // namespace vcf_tool::domain::api
