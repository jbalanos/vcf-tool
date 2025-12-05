// Pipeline.h
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <future>

#include "Context.h"
#include "../reader/FileLineReaderWorker.h"
#include "../writer/DbWriterWorker.h"


namespace vcf_tool::domain::pipeline {

using vcf_tool::domain::reader::FileLineReaderWorker;
using vcf_tool::domain::writer::DbWriterWorker;

/**
 * @brief Pipeline orchestrator for VCF processing
 *
 * Coordinates the lifecycle of all workers:
 * - 1 reader thread (FileLineReaderWorker)
 * - N parser threads (submitted to ThreadPool)
 * - 1 writer thread (DbWriterWorker)
 *
 * Handles:
 * - Starting all workers in correct order
 * - Waiting for completion
 * - Propagating errors from parser futures
 * - RAII cleanup via unique_ptr (reader/writer auto-join)
 */
class Pipeline {
public:
    /**
     * Construct a pipeline for processing a VCF file.
     *
     * @param ctx        Reference to Context containing queues, pool, and config
     * @param file_path  Path to VCF file to process
     */
    Pipeline(Context& ctx, std::string file_path);

    /**
     * Execute the complete pipeline:
     * 1. Start reader worker
     * 2. Submit N parser tasks to thread pool
     * 3. Start writer worker
     * 4. Wait for all to complete
     * 5. Check for errors
     *
     * @throws std::exception  If any worker encounters an error
     */
    void execute();

private:
    Context& ctx_;
    std::string file_path_;

    // Worker lifecycle management
    std::unique_ptr<FileLineReaderWorker> start_reader();
    std::vector<std::future<void>> start_parsers();
    std::unique_ptr<DbWriterWorker> start_writer();

    // Error handling
    void wait_and_check_errors(
        std::unique_ptr<FileLineReaderWorker>& reader,
        std::vector<std::future<void>>& parser_futures,
        std::unique_ptr<DbWriterWorker>& writer
    );
};

} // namespace vcf_tool::domain::pipeline
