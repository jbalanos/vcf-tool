// Context.h
#pragma once

#include <cstddef>

#include <vcf_tool/core/ThreadPool.h>
#include "../Queues.h"


namespace vcf_tool::domain::pipeline {

using vcf_tool::core::ThreadPool;
using vcf_tool::domain::LineQueue;
using vcf_tool::domain::RecordQueue;

/**
 * @brief State container for VCF processing pipeline
 *
 * Owns all shared resources: queues, thread pool, and configuration.
 * Contains zero orchestration logic - just data and resource management.
 * Fresh instance created for each VCF file processed.
 */
class Context {
public:
    /**
     * @brief Configuration for pipeline resources
     */
    struct Config {
        std::size_t parser_count;           // Number of parser threads
        std::size_t batch_size;             // Records per batch for DB writes
        std::size_t line_queue_capacity;    // Max lines in reader->parser queue
        std::size_t record_queue_capacity;  // Max records in parser->writer queue
    };

    /**
     * Construct a context with the given configuration.
     * Initializes queues with configured capacities and thread pool with parser_count threads.
     *
     * @param config  Configuration for queues and thread pool
     */
    explicit Context(Config config);

    // Accessors
    LineQueue& line_queue() { return line_queue_; }
    const LineQueue& line_queue() const { return line_queue_; }

    RecordQueue& record_queue() { return record_queue_; }
    const RecordQueue& record_queue() const { return record_queue_; }

    ThreadPool& thread_pool() { return thread_pool_; }
    const ThreadPool& thread_pool() const { return thread_pool_; }

    std::size_t parser_count() const { return config_.parser_count; }
    std::size_t batch_size() const { return config_.batch_size; }

    const Config& config() const { return config_; }

private:
    Config config_;

    // Queues for pipeline communication
    LineQueue line_queue_;
    RecordQueue record_queue_;

    // Thread pool for parser workers
    ThreadPool thread_pool_;
};

} // namespace vcf_tool::domain::pipeline
