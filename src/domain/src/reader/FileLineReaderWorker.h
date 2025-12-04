// FileLineReaderWorker.h
#pragma once

#include <string>
#include <thread>
#include <stop_token>
#include <atomic>

#include "../Queues.h"

namespace vcf_tool::domain::reader {

class FileLineReaderWorker {
public:
    /**
     * Construct a worker that reads the given file line-by-line
     * and enqueues each RawLine into the provided LineQueue.
     *
     * @param file_path       Path to the file to read.
     * @param output_queue    Queue where lines will be pushed.
     * @param emit_sentinel   Whether to push a RawLine{.is_end = true} when done.
     */
    FileLineReaderWorker(std::string file_path,
                         LineQueue& output_queue,
                         bool emit_sentinel = true);

    // Non-copyable, non-movable
    FileLineReaderWorker(const FileLineReaderWorker&) = delete;
    FileLineReaderWorker& operator=(const FileLineReaderWorker&) = delete;

    ~FileLineReaderWorker();

    /// Request the worker to stop (optional, std::jthread also requests stop in dtor)
    void request_stop();

private:
    // Thread entry
    void run(std::stop_token st);

    std::string  file_path_;
    LineQueue&   output_queue_;
    bool         emit_sentinel_;

    std::jthread thread_;
};

} // namespace vcf_tool::domain::reader