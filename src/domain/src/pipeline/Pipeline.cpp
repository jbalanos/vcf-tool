// Pipeline.cpp
#include "Pipeline.h"

#include <iostream>  // TODO: Replace with Logger
#include <exception>
#include <memory>

#include "../parser/SimpleParserService.h"
#include "../parser/VcfLineParser.h"
#include "../dao/VcfDao.h"


namespace vcf_tool::domain::pipeline {

using vcf_tool::domain::parser::SimpleParserService;
using vcf_tool::domain::VcfLineParser;

Pipeline::Pipeline(Context& ctx, std::string file_path)
    : ctx_(ctx)
    , file_path_(std::move(file_path))
{
}

void Pipeline::execute()
{
    std::cerr << "Pipeline: starting for file: " << file_path_ << "\n";

    // Start all workers
    auto reader = start_reader();
    auto parser_futures = start_parsers();
    auto writer = start_writer();

    // Wait for completion and check errors
    wait_and_check_errors(reader, parser_futures, writer);

    std::cerr << "Pipeline: completed successfully for file: " << file_path_ << "\n";
}

std::unique_ptr<FileLineReaderWorker> Pipeline::start_reader()
{
    return std::make_unique<FileLineReaderWorker>(
        file_path_,
        ctx_.line_queue(),
        true,  // emit_sentinel
        ctx_.parser_count()  // sentinel_count (one per parser)
    );
    // Thread starts immediately in FileLineReaderWorker constructor
}

std::vector<std::future<void>> Pipeline::start_parsers()
{
    std::vector<std::future<void>> futures;
    futures.reserve(ctx_.parser_count());

    for (std::size_t i = 0; i < ctx_.parser_count(); ++i) {
        // Create parser service (functor)
        SimpleParserService parser_service{
            .input_queue = ctx_.line_queue(),
            .output_queue = ctx_.record_queue(),
            .parser = VcfLineParser{}
        };

        // Submit to thread pool and store future
        // Note: ThreadPool internally wraps move-only lambdas in shared_ptr
        auto future = ctx_.thread_pool().submit(
            [service = std::move(parser_service)]() mutable {
                service();  // Calls operator()
            }
        );

        futures.push_back(std::move(future));
    }

    std::cerr << "Pipeline: started " << ctx_.parser_count() << " parser workers\n";
    return futures;
}

std::unique_ptr<DbWriterWorker> Pipeline::start_writer()
{
    // Create VcfDao instance for this pipeline
    auto dao = std::make_unique<dao::VcfDao>();

    return std::make_unique<DbWriterWorker>(
        ctx_.record_queue(),
        ctx_.batch_size(),
        ctx_.parser_count(),  // sentinel_count (expects N sentinels from N parsers)
        std::move(dao)  // Inject DAO
    );
    // Thread starts immediately in DbWriterWorker constructor
}

void Pipeline::wait_and_check_errors(
    [[maybe_unused]] std::unique_ptr<FileLineReaderWorker>& reader,
    std::vector<std::future<void>>& parser_futures,
    [[maybe_unused]] std::unique_ptr<DbWriterWorker>& writer)
{
    // reader and writer are intentionally passed by reference (not used in body)
    // so their RAII destructors auto-join the jthread workers when function returns

    // Collect errors from parser futures
    std::vector<std::exception_ptr> errors;

    for (auto& fut : parser_futures) {
        try {
            fut.get();  // Blocks until parser completes, re-throws if exception occurred
        } catch (...) {
            // Capture exception for later reporting
            errors.push_back(std::current_exception());
        }
    }

    // Reader and writer auto-join via jthread destructors when unique_ptrs go out of scope
    // (This happens when this function returns or when execute() completes)

    // If any parser had errors, rethrow the first one
    if (!errors.empty()) {
        std::cerr << "Pipeline: encountered " << errors.size() << " parser error(s)\n";
        std::rethrow_exception(errors[0]);
    }

    std::cerr << "Pipeline: all workers completed successfully\n";
}

} // namespace vcf_tool::domain::pipeline
