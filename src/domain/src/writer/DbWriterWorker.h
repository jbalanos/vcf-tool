// DbWriterWorker.h
#pragma once

#include <thread>
#include <stop_token>
#include <vector>
#include <memory>

#include "../Queues.h"
#include "../entity/ParsedRecord.h"
#include "../dao/VcfDao.h"


namespace vcf_tool::domain::writer {

using entity::ParsedRecord;

/**
 * @brief Database writer worker using jthread-based RAII pattern
 *
 * Continuously dequeues parsed records from input queue, accumulates them
 * into batches, and writes batches to database. Handles multiple sentinel
 * values for proper termination with N parsers.
 */
class DbWriterWorker {
public:
    /**
     * Construct a writer worker that batches and writes parsed records.
     *
     * @param input_queue     Queue from which to read parsed records.
     * @param batch_size      Number of records to accumulate before flushing.
     * @param sentinel_count  Number of sentinels to expect (one per parser).
     * @param dao             Data access object for database operations.
     */
    DbWriterWorker(RecordQueue& input_queue,
                   std::size_t batch_size,
                   std::size_t sentinel_count,
                   std::unique_ptr<dao::VcfDao> dao);

    // Non-copyable, non-movable
    DbWriterWorker(const DbWriterWorker&) = delete;
    DbWriterWorker& operator=(const DbWriterWorker&) = delete;

    ~DbWriterWorker();

    /// Request the worker to stop (optional, std::jthread also requests stop in dtor)
    void request_stop();

private:
    // Thread entry point
    void run(std::stop_token st);

    // Flush accumulated batch to database
    void flush_batch();

    RecordQueue& input_queue_;
    std::size_t batch_size_;
    std::size_t sentinel_count_;

    std::vector<ParsedRecord> batch_;
    std::unique_ptr<dao::VcfDao> dao_;
    std::jthread thread_;
};

} // namespace vcf_tool::domain::writer
