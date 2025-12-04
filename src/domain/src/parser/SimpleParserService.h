#pragma once

#include "../Queues.h"
#include "NaiveLineParser.h"


namespace vcf_tool::domain::parser {

using domain::LineQueue;
using domain::RecordQueue;

/**
 * @brief Concurrent parsing service using producer-consumer pattern
 *
 * Continuously dequeues raw lines from input queue, parses them,
 * and enqueues parsed records to output queue. Handles end-of-stream
 * sentinels for proper pipeline termination.
 */
struct SimpleParserService {
    LineQueue&      input_queue;
    RecordQueue&    output_queue;
    NaiveLineParser parser;  // Injected parser (strategy pattern)

    /**
     * @brief Main processing loop - designed to run in a thread
     *
     * Continuously processes lines until an end-of-stream sentinel
     * is received, then propagates it downstream and terminates.
     */
    void operator()();
};

} // namespace vcf_tool::domain::parser
