#pragma once

#include "../Queues.h"


namespace vcf_tool::domain::parser {

using domain::LineQueue;
using domain::RecordQueue;

/**
 * @brief Concurrent parsing service using producer-consumer pattern
 *
 * Continuously dequeues raw lines from input queue, parses them,
 * and enqueues parsed records to output queue. Handles end-of-stream
 * sentinels for proper pipeline termination.
 *
 * @tparam Parser Type of parser to use (must implement operator()(const RawLine&))
 */
template<typename Parser>
struct SimpleParserService {
    LineQueue&  input_queue;
    RecordQueue& output_queue;
    Parser      parser;  // Injected parser (strategy pattern)

    /**
     * @brief Main processing loop - designed to run in a thread
     *
     * Continuously processes lines until an end-of-stream sentinel
     * is received, then propagates it downstream and terminates.
     */
    void operator()();
};

} // namespace vcf_tool::domain::parser
