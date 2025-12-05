// VcfTool.cpp
#include <vcf_tool/domain/VcfTool.h>

#include <filesystem>
#include <fstream>
#include <iostream>  // TODO: Replace with Logger

#include <vcf_tool/utils/Errors.h>
#include "../pipeline/Context.h"
#include "../pipeline/Pipeline.h"


namespace vcf_tool::domain::api {

using vcf_tool::domain::pipeline::Context;
using vcf_tool::domain::pipeline::Pipeline;

VcfTool::VcfTool(Config config)
    : config_(config)
{
    std::cerr << "VcfTool: created with " << config_.parser_count
              << " parser threads, batch size " << config_.batch_size << "\n";
}

void VcfTool::run(const std::string& file_path)
{
    std::cerr << "VcfTool: processing file: " << file_path << "\n";

    // 1. Validate file path is not empty
    if (file_path.empty()) {
        throw utils::errors::ValidationError(
            "File path cannot be empty",
            utils::errors::Component::IO
        );
    }

    // 2-4. Validate file with proper exception handling for filesystem operations
    try {
        // Check if file exists
        if (!std::filesystem::exists(file_path)) {
            throw utils::errors::FileNotFoundError(
                file_path,
                utils::errors::Component::IO
            );
        }

        // Check if it's a regular file (not a directory/symlink/device)
        if (!std::filesystem::is_regular_file(file_path)) {
            throw utils::errors::ValidationError(
                "Path exists but is not a regular file: " + file_path,
                utils::errors::Component::IO
            );
        }

        // Check file permissions
        auto perms = std::filesystem::status(file_path).permissions();
        using std::filesystem::perms;
        bool has_read = (perms & perms::owner_read) != perms::none ||
                        (perms & perms::group_read) != perms::none ||
                        (perms & perms::others_read) != perms::none;

        if (!has_read) {
            throw utils::errors::IOError(
                "File exists but has no read permissions: " + file_path,
                utils::errors::Component::IO
            );
        }

    } catch (const std::filesystem::filesystem_error& e) {
        // Convert filesystem errors to our custom error type
        throw utils::errors::IOError(
            "Filesystem error accessing '" + file_path + "': " + e.what(),
            utils::errors::Component::IO
        );
    }

    // Note: TOCTOU race condition still exists between validation and actual use,
    // but FileLineReaderWorker will handle runtime file open failures gracefully

    // Create fresh context for this run (no state pollution between runs)
    Context::Config ctx_config{
        .parser_count = config_.parser_count,
        .batch_size = config_.batch_size,
        .line_queue_capacity = config_.line_queue_capacity,
        .record_queue_capacity = config_.record_queue_capacity
    };

    Context ctx(ctx_config);

    // Create and execute pipeline
    Pipeline pipeline(ctx, file_path);
    pipeline.execute();

    // Context destroyed here (RAII cleanup of queues, thread pool)

    std::cerr << "VcfTool: completed processing: " << file_path << "\n";
}

} // namespace vcf_tool::domain::api
