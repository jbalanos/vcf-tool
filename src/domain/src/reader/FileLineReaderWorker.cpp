// FileLineReaderWorker.cpp
#include "FileLineReaderWorker.h"

#include <fstream>
#include <iostream>  // or your Logger


namespace vcf_tool::domain::reader {

FileLineReaderWorker::FileLineReaderWorker(std::string file_path,
                                           LineQueue& output_queue,
                                           bool emit_sentinel,
                                           std::size_t sentinel_count)
    : file_path_(std::move(file_path))
    , output_queue_(output_queue)
    , emit_sentinel_(emit_sentinel)
    , sentinel_count_(sentinel_count)
    , thread_([this](std::stop_token st) {
        run(st);
      })
{
    // Thread starts immediately in constructor
}

FileLineReaderWorker::~FileLineReaderWorker()
{
    // std::jthread destructor automatically:
    //  - calls request_stop()
    //  - joins the thread
    //
    // So we don't need extra logic here.
}

void FileLineReaderWorker::request_stop()
{
    thread_.request_stop();
}

void FileLineReaderWorker::run(std::stop_token st)
{
    std::ifstream in(file_path_);
    if (!in.is_open()) {
        std::cerr << "FileLineReaderWorker: failed to open file: "
                  << file_path_ << "\n";

        if (emit_sentinel_) {
            // Even on failure, emit N sentinels to prevent downstream parser deadlock
            for (std::size_t i = 0; i < sentinel_count_; ++i) {
                output_queue_.enqueue(RawLine{.line_number = 0, .text = {}, .is_end = true});
            }
        }
        return;
    }

    std::string line;
    std::uint64_t line_number = 0;

    while (!st.stop_requested() && std::getline(in, line)) {
        ++line_number;

        RawLine raw{
            .line_number = line_number,
            .text        = line,
            .is_end      = false
        };

        // This will block if queue is full (bounded queue)
        output_queue_.enqueue(std::move(raw));
    }

    // Emit N sentinels (one per downstream parser) to signal end-of-stream
    // This ensures all N parsers receive a termination signal
    if (emit_sentinel_) {
        for (std::size_t i = 0; i < sentinel_count_; ++i) {
            output_queue_.enqueue(RawLine{.line_number = 0, .text = {}, .is_end = true});
        }
    }
}

} // namespace vcf_tool::domain::reader