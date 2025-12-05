// Context.cpp
#include "Context.h"


namespace vcf_tool::domain::pipeline {

Context::Context(Config config)
    : config_(config)
    , line_queue_(config.line_queue_capacity)
    , record_queue_(config.record_queue_capacity)
    , thread_pool_(config.parser_count)
{
    // All resources initialized via member initializer list
    // Queues: Created with configured capacities
    // ThreadPool: Created with parser_count threads (already running)
}

} // namespace vcf_tool::domain::pipeline
