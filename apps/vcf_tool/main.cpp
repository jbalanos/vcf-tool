#include <iostream>
#include <string>
#include <thread>
#include <filesystem>
#include <algorithm>
#include <cctype>

#include <CLI/CLI.hpp>
#include <vcf_tool/utils/Logger.h>

namespace fs = std::filesystem;
using vcf_tool::utils::Logger;

// Helper to map string -> Logger::Level
Logger::Level parse_log_level(const std::string& level_str) {
    std::string s = level_str;
    // make lowercase
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return static_cast<char>(std::tolower(c)); });

    if (s == "trace")   return Logger::Level::Trace;
    if (s == "debug")   return Logger::Level::Debug;
    if (s == "info")    return Logger::Level::Info;
    if (s == "warn" || s == "warning") return Logger::Level::Warn;
    if (s == "error")   return Logger::Level::Error;
    if (s == "critical" || s == "fatal") return Logger::Level::Critical;

    // Default fallback
    return Logger::Level::Info;
}

// Placeholder for your actual import/processing pipeline
int run_vcf_import(const std::string& vcf_path, int num_threads) {
    LOG_INFO_F("Running VCF import for file '{}' using {} threads", vcf_path, num_threads);

    // Example: validate that file exists
    if (!fs::exists(vcf_path)) {
        LOG_ERROR_F("VCF file does not exist: '{}'", vcf_path);
        return 1;
    }

    // TODO: here you would:
    //  - create thread pool
    //  - start reader worker(s)
    //  - start parser worker(s)
    //  - push results to DB / output queue

    LOG_DEBUG_F("Simulating processing of file '{}'", vcf_path);
    // ... your real logic goes here ...

    LOG_INFO("VCF import completed successfully");
    return 0;
}

int main(int argc, char** argv) {
    CLI::App app{"vcf_importer - Multi-threaded VCF import CLI"};

    std::string vcf_path;
    int threads = 0;

    // Logging-related options
    std::string log_level_str = "info";
    std::string log_file_path;  // empty => console only

    // Required VCF argument
    app.add_option("--vcf", vcf_path, "Path to the input VCF file")
       ->required()
       ->check(CLI::ExistingFile);

    // Optional threads argument
    app.add_option("--threads", threads,
                   "Number of threads to use for reading/parsing")
       ->check(CLI::PositiveNumber);

    // Optional log level argument
    app.add_option("--log-level", log_level_str,
                   "Log level: trace|debug|info|warn|error|critical")
       ->capture_default_str();

    // Optional log file path
    app.add_option("--log-file", log_file_path,
                   "Path to log file (if omitted, logs only to console)");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        // Log CLI parsing error to stderr (Logger not yet initialized)
        std::cerr << "CLI parsing error: " << e.what() << std::endl;
        return app.exit(e);
    }

    // Determine sensible default for threads if not provided or zero
    if (threads <= 0) {
        unsigned int hw = std::thread::hardware_concurrency();
        if (hw == 0) {
            threads = 4;  // conservative fallback
        } else {
            threads = static_cast<int>(hw);
        }
    }

    // Ensure parent directory of log file exists if user provided one
    if (!log_file_path.empty()) {
        try {
            fs::path log_path(log_file_path);
            if (log_path.has_parent_path()) {
                fs::create_directories(log_path.parent_path());
            }
        } catch (const std::exception& ex) {
            // If log directory creation fails, we can still proceed with console logging
            std::cerr << "Warning: could not create log directory for '"
                      << log_file_path << "': " << ex.what() << "\n";
            log_file_path.clear();  // fall back to console-only
        }
    }

    // Initialize logger with user's options
    Logger::Level level = parse_log_level(log_level_str);
    Logger::initialize(log_file_path, level);

    LOG_INFO_F("vcf_importer starting");
    LOG_INFO_F("Input VCF file: '{}'", vcf_path);
    LOG_INFO_F("Threads: {}", threads);
    LOG_INFO_F("Log level: {}", log_level_str);
    if (!log_file_path.empty()) {
        LOG_INFO_F("Logging to file: '{}'", log_file_path);
    } else {
        LOG_INFO("Logging to console only");
    }

    int rc = run_vcf_import(vcf_path, threads);

    if (rc != 0) {
        LOG_ERROR_F("vcf_importer finished with errors (code {})", rc);
    } else {
        LOG_INFO("vcf_importer finished successfully");
    }

    return rc;
}
