// VcfTool.h
#pragma once

#include <string>
#include <cstddef>



namespace vcf_tool::domain::api {

// Forward declaration
class VcfToolBuilder;

/**
 * @brief Public API for VCF file processing
 *
 * Simple facade that hides internal complexity (Context, Pipeline, workers).
 * Reusable instance - can process multiple VCF files sequentially.
 *
 * Usage:
 *   auto tool = VcfToolBuilder()
 *       .with_parser_threads(4)
 *       .with_batch_size(1000)
 *       .build();
 *
 *   tool.run("file1.vcf");
 *   tool.run("file2.vcf");  // Reusable
 *
 * Thread model: N + 2 threads total
 *   - 1 reader thread (jthread)
 *   - N parser threads (from ThreadPool)
 *   - 1 writer thread (jthread)
 */
class VcfTool {
public:
    /**
     * @brief Configuration for VcfTool instance
     *
     * Internal config structure - users should use VcfToolBuilder instead
     */
    struct Config {
        std::size_t parser_count;
        std::size_t batch_size;
        std::size_t line_queue_capacity;
        std::size_t record_queue_capacity;
    };

    /**
     * Process a VCF file with configured thread count and batch size.
     * Creates fresh Context and Pipeline for each run (no state pollution).
     *
     * @param file_path  Path to VCF file to process
     * @throws std::exception  If file doesn't exist or processing fails
     */
    void run(const std::string& file_path);

    // Accessors for current configuration
    std::size_t parser_count() const { return config_.parser_count; }
    std::size_t batch_size() const { return config_.batch_size; }

private:
    // Private constructor - only VcfToolBuilder can create instances
    explicit VcfTool(Config config);

    Config config_;

    // Builder is friend to access private constructor
    friend class VcfToolBuilder;
};

} // namespace vcf_tool::domain::api
