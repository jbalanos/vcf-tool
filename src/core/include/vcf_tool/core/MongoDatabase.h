#pragma once

#include <memory>
#include <string>
#include <atomic>
#include <mutex>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>

#include "MongoConfig.h"

namespace vcf_tool::core {

/**
 * @brief MongoDB singleton for driver lifecycle and connection management
 *
 * Responsibilities:
 *   - Initialize mongocxx::instance (required once per process)
 *   - Create and manage connection pool
 *   - Provide access to database and collection handles
 *
 * Thread Safety:
 *   - Meyer's singleton pattern ensures single initialization
 *   - mongocxx::pool is thread-safe for acquiring connections
 *   - mongocxx::client is thread-safe for read operations
 *
 * Usage:
 *   // In main.cpp, initialize before pipeline:
 *   MongoDatabase::initialize(MongoConfig::from_environment());
 *
 *   // In VcfDao, access database:
 *   auto& db = MongoDatabase::instance();
 *   auto collection = db.get_collection();
 */
class MongoDatabase {
public:
    /**
     * Get singleton instance (must call initialize() first)
     * @throws DatabaseError if initialize() was not called
     */
    static MongoDatabase& instance();

    /**
     * Initialize MongoDB connection (call once at startup)
     *
     * @param config  MongoDB configuration from environment
     * @throws DatabaseError if connection fails
     */
    static void initialize(const MongoConfig& config);

    /**
     * Check if MongoDB has been initialized
     */
    static bool is_initialized();

    /**
     * Get connection pool for advanced use cases
     */
    mongocxx::pool& pool();

    /**
     * Get a database handle (thread-safe)
     */
    mongocxx::database get_database();

    /**
     * Get collection handle for VCF records (convenience method)
     */
    mongocxx::collection get_collection();

    /**
     * Get current configuration
     */
    const MongoConfig& config() const { return config_; }

    // Non-copyable, non-movable
    MongoDatabase(const MongoDatabase&) = delete;
    MongoDatabase& operator=(const MongoDatabase&) = delete;
    MongoDatabase(MongoDatabase&&) = delete;
    MongoDatabase& operator=(MongoDatabase&&) = delete;

    ~MongoDatabase() = default;

private:
    MongoDatabase(const MongoConfig& config);

    MongoConfig config_;

    // MongoDB driver instance (required exactly once per process)
    // Must be constructed before any mongocxx objects
    std::unique_ptr<mongocxx::instance> instance_;

    // Connection pool for thread-safe access
    std::unique_ptr<mongocxx::pool> pool_;
};

} // namespace vcf_tool::core
