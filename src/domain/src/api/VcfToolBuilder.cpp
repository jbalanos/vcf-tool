// VcfToolBuilder.cpp
#include <vcf_tool/domain/VcfToolBuilder.h>
#include <vcf_tool/domain/VcfTool.h>

#include <thread>
#include <stdexcept>
#include <iostream>  // TODO: Replace with Logger


namespace vcf_tool::domain::api {

VcfToolBuilder::VcfToolBuilder()
{
    // Use default values from member initializers
}

VcfToolBuilder& VcfToolBuilder::with_parser_threads(std::size_t n)
{
    parser_threads_ = n;
    return *this;
}

VcfToolBuilder& VcfToolBuilder::with_batch_size(std::size_t n)
{
    batch_size_ = n;
    return *this;
}

VcfToolBuilder& VcfToolBuilder::with_line_queue_capacity(std::size_t n)
{
    line_queue_capacity_ = n;
    return *this;
}

VcfToolBuilder& VcfToolBuilder::with_record_queue_capacity(std::size_t n)
{
    record_queue_capacity_ = n;
    return *this;
}

VcfToolBuilder VcfToolBuilder::for_large_files()
{
    return VcfToolBuilder()
        .with_parser_threads(0)  // Use all available cores
        .with_batch_size(5000)
        .with_line_queue_capacity(50000)
        .with_record_queue_capacity(25000);
}

VcfToolBuilder VcfToolBuilder::for_low_memory()
{
    return VcfToolBuilder()
        .with_parser_threads(2)
        .with_batch_size(500)
        .with_line_queue_capacity(5000)
        .with_record_queue_capacity(2500);
}

void VcfToolBuilder::validate() const
{
    // Batch size must be > 0
    if (batch_size_ == 0) {
        throw std::invalid_argument("VcfToolBuilder: batch_size must be > 0");
    }

    // Queue capacities must be >= batch_size to prevent deadlock
    if (line_queue_capacity_ < batch_size_) {
        throw std::invalid_argument(
            "VcfToolBuilder: line_queue_capacity must be >= batch_size"
        );
    }

    if (record_queue_capacity_ < batch_size_) {
        throw std::invalid_argument(
            "VcfToolBuilder: record_queue_capacity must be >= batch_size"
        );
    }

    // Warn if thread count is very high (more than 2x available cores)
    if (parser_threads_ > 0) {
        unsigned int hw_threads = std::thread::hardware_concurrency();
        if (hw_threads > 0 && parser_threads_ > 2 * hw_threads) {
            std::cerr << "VcfToolBuilder: Warning - parser_threads ("
                     << parser_threads_ << ") is more than 2x hardware_concurrency ("
                     << hw_threads << "). This may reduce performance.\n";
        }
    }
}

VcfTool VcfToolBuilder::build() const
{
    // Validate configuration
    validate();

    // Resolve parser thread count (0 = auto-detect)
    std::size_t threads = parser_threads_;
    if (threads == 0) {
        threads = std::thread::hardware_concurrency();
        if (threads == 0) {
            threads = 4;  // Fallback if hardware_concurrency() returns 0
        }
        if( threads > 2 ) {
            threads=threads -2; // leave some room for other tasks
        }
        std::cerr << "VcfToolBuilder: auto-detected " << threads << " parser threads\n";
    }

    // Create config
    VcfTool::Config config{
        .parser_count = threads,
        .batch_size = batch_size_,
        .line_queue_capacity = line_queue_capacity_,
        .record_queue_capacity = record_queue_capacity_
    };

    // Construct and return VcfTool (using friend access to private constructor)
    return VcfTool(config);
}

} // namespace vcf_tool::domain::api
