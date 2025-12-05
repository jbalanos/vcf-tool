#pragma once

#include <string>
#include <cstdlib>
#include <vcf_tool/utils/Errors.h>

namespace vcf_tool::core {

/**
 * @brief MongoDB configuration from environment variables
 *
 * Reads and validates environment variables for MongoDB connection.
 * Used during MongoDatabase initialization.
 */
struct MongoConfig {
    std::string uri;
    std::string db_name;
    std::string collection_name;

    /**
     * Load configuration from environment variables.
     *
     * Required:
     *   - MONGODB_URI: Connection string (e.g., "mongodb://localhost:27017")
     *   - MONGODB_DB_NAME: Database name (e.g., "vcf_db")
     *
     * Optional:
     *   - MONGODB_COLLECTION_NAME: Collection name (default: "vcf_records")
     *
     * @throws ValidationError if required env vars are missing
     */
    static MongoConfig from_environment() {
        MongoConfig config;

        // Read required environment variables
        const char* uri_env = std::getenv("MONGODB_URI");
        const char* db_env = std::getenv("MONGODB_DB_NAME");

        if (!uri_env || std::string(uri_env).empty()) {
            throw utils::errors::ValidationError(
                "Missing required environment variable: MONGODB_URI",
                utils::errors::Component::Database
            );
        }

        if (!db_env || std::string(db_env).empty()) {
            throw utils::errors::ValidationError(
                "Missing required environment variable: MONGODB_DB_NAME",
                utils::errors::Component::Database
            );
        }

        config.uri = uri_env;
        config.db_name = db_env;

        // Optional collection name with default
        const char* coll_env = std::getenv("MONGODB_COLLECTION_NAME");
        config.collection_name = (coll_env && std::string(coll_env).length() > 0)
            ? coll_env
            : "vcf_records";

        return config;
    }
};

} // namespace vcf_tool::core
