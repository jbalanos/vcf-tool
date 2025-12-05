// DbWriterWorker.cpp
#include "DbWriterWorker.h"

#include <vcf_tool/utils/Logger.h>
#include <vcf_tool/utils/Errors.h>


namespace vcf_tool::domain::writer {

DbWriterWorker::DbWriterWorker(RecordQueue& input_queue,
                               std::size_t batch_size,
                               std::size_t sentinel_count,
                               std::unique_ptr<dao::VcfDao> dao)
    : input_queue_(input_queue)
    , batch_size_(batch_size)
    , sentinel_count_(sentinel_count)
    , dao_(std::move(dao))
    , thread_([this](std::stop_token st) {
        run(st);
      })
{
    batch_.reserve(batch_size_);
    // Thread starts immediately in constructor
}

DbWriterWorker::~DbWriterWorker()
{
    // std::jthread destructor automatically:
    //  - calls request_stop()
    //  - joins the thread
}

void DbWriterWorker::request_stop()
{
    thread_.request_stop();
}

void DbWriterWorker::run([[maybe_unused]] std::stop_token st)
{
    std::size_t sentinels_received = 0;
    std::size_t records_processed = 0;
    std::size_t records_skipped = 0;
    std::size_t batches_flushed = 0;

    for (;;) {
        ParsedRecord record;
        input_queue_.wait_dequeue(record);

        // Check for sentinel (end-of-stream signal)
        if (record.is_end) {
            ++sentinels_received;
            LOG_DEBUG_F("DbWriterWorker: received sentinel {}/{}", sentinels_received, sentinel_count_);

            if (sentinels_received >= sentinel_count_) {
                // All parsers have finished, flush remaining records and exit
                if (!batch_.empty()) {
                    LOG_DEBUG_F("Flushing final batch of {} records", batch_.size());
                    flush_batch();
                    ++batches_flushed;
                }
                LOG_INFO_F("DbWriterWorker: processed {} records, skipped {} empty, flushed {} batches",
                          records_processed, records_skipped, batches_flushed);
                break;
            }
            // Not all parsers finished yet, continue waiting
            continue;
        }

        // Skip empty/invalid records (e.g., header lines)
        // Valid VCF records always have a chromosome
        if (record.vcf_data.chromosome.empty()) {
            ++records_skipped;
            LOG_DEBUG_F("Skipping empty record at line {} (total skipped: {})",
                       record.line_number, records_skipped);
            continue;
        }

        // Accumulate record in batch
        ++records_processed;
        batch_.push_back(std::move(record));
        LOG_DEBUG_F("Added record to batch (batch size: {}/{})", batch_.size(), batch_size_);

        // Flush if batch is full
        if (batch_.size() >= batch_size_) {
            LOG_DEBUG_F("Batch full, flushing {} records (batch #{})", batch_.size(), batches_flushed + 1);
            flush_batch();
            ++batches_flushed;
            batch_.clear();
            LOG_DEBUG_F("Batch cleared, continuing...");
        }
    }
}

void DbWriterWorker::flush_batch()
{
    if (batch_.empty()) {
        return;
    }

    try {
        LOG_DEBUG_F("Flushing batch of {} records to MongoDB", batch_.size());
        std::size_t inserted = dao_->bulk_insert(batch_);

        if (inserted != batch_.size()) {
            LOG_WARN_F("Partial insert: {} of {} records written", inserted, batch_.size());
        } else {
            LOG_DEBUG_F("Successfully flushed {} records", inserted);
        }

    } catch (const utils::errors::DatabaseError& e) {
        LOG_ERROR_F("Database write failed: {}", e.what());
        // Log but don't throw - continue processing
    }
}

} // namespace vcf_tool::domain::writer
