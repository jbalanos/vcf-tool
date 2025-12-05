#include <vcf_tool/core/MongoDatabase.h>
#include <vcf_tool/utils/Logger.h>
#include <vcf_tool/utils/Errors.h>
#include <vcf_tool/utils/Format.h>

#include <mongocxx/uri.hpp>
#include <mongocxx/exception/exception.hpp>
#include <bsoncxx/builder/stream/document.hpp>

namespace vcf_tool::core {

namespace {
    // Flag to track initialization status
    std::atomic<bool> g_initialized{false};

    // Pointer to singleton (initially null)
    std::unique_ptr<MongoDatabase> g_instance{nullptr};

    // Mutex for initialization
    std::mutex g_init_mutex;
}

MongoDatabase::MongoDatabase(const MongoConfig& config)
    : config_(config)
{
    try {
        LOG_INFO("Initializing MongoDB driver");

        // Initialize mongocxx instance (required once per process)
        instance_ = std::make_unique<mongocxx::instance>();

        LOG_INFO_F("Creating MongoDB connection pool to: {}", config_.uri);

        // Create connection pool with URI
        mongocxx::uri uri{config_.uri};
        pool_ = std::make_unique<mongocxx::pool>(uri);

        // Test connection by acquiring client from pool
        auto client_entry = pool_->acquire();

        // Verify database connection by sending ping
        auto admin_db = client_entry->database("admin");
        auto ping_cmd = bsoncxx::builder::stream::document{}
            << "ping" << 1
            << bsoncxx::builder::stream::finalize;
        admin_db.run_command(ping_cmd.view());

        LOG_INFO_F("Successfully connected to MongoDB database: {}", config_.db_name);
        LOG_INFO_F("Using collection: {}", config_.collection_name);

    } catch (const mongocxx::exception& e) {
        throw utils::errors::DatabaseError(
            utils::format("MongoDB initialization failed: {}", e.what()),
            utils::errors::Component::Database
        );
    } catch (const std::exception& e) {
        throw utils::errors::DatabaseError(
            utils::format("Unexpected error during MongoDB initialization: {}", e.what()),
            utils::errors::Component::Database
        );
    }
}

void MongoDatabase::initialize(const MongoConfig& config) {
    std::lock_guard<std::mutex> lock(g_init_mutex);

    if (g_initialized.load()) {
        LOG_WARN("MongoDatabase::initialize() called multiple times - ignoring");
        return;
    }

    g_instance = std::unique_ptr<MongoDatabase>(new MongoDatabase(config));
    g_initialized.store(true);
}

MongoDatabase& MongoDatabase::instance() {
    if (!g_initialized.load()) {
        throw utils::errors::DatabaseError(
            "MongoDatabase not initialized. Call MongoDatabase::initialize() first.",
            utils::errors::Component::Database
        );
    }
    return *g_instance;
}

bool MongoDatabase::is_initialized() {
    return g_initialized.load();
}

mongocxx::pool& MongoDatabase::pool() {
    return *pool_;
}

mongocxx::database MongoDatabase::get_database() {
    auto client = pool_->acquire();
    return (*client)[config_.db_name];
}

mongocxx::collection MongoDatabase::get_collection() {
    auto client = pool_->acquire();
    return (*client)[config_.db_name][config_.collection_name];
}

} // namespace vcf_tool::core
