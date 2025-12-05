#pragma once

#include <vector>
#include <cstddef>
#include <mongocxx/collection.hpp>

#include "../entity/VcfRecord.h"
#include "../entity/ParsedRecord.h"

namespace vcf_tool::domain::dao {

/**
 * @brief Data Access Object for VCF records in MongoDB
 *
 * Provides high-level interface for inserting VCF records into MongoDB.
 *
 * Thread Safety:
 *   - Stateless (only holds collection reference)
 *   - mongocxx::collection is thread-safe for operations
 *   - Safe to use from single DbWriterWorker thread
 *
 * Usage:
 *   VcfDao dao;
 *   dao.bulk_insert(parsed_records);
 */
class VcfDao {
public:
    /**
     * Default constructor - uses MongoDatabase singleton and ensures indexes
     */
    VcfDao();

    /**
     * Insert a single VCF record
     *
     * @param record  VcfRecord to insert
     * @throws DatabaseError on failure
     */
    void insert(const VcfRecord& record);

    /**
     * Bulk insert multiple ParsedRecords (batch write)
     *
     * More efficient than individual inserts for large batches.
     * Uses MongoDB bulk_write with ordered=false for best performance.
     *
     * @param records  Vector of ParsedRecords (extracts vcf_data)
     * @return Number of successfully inserted documents
     * @throws DatabaseError on failure
     */
    std::size_t bulk_insert(const std::vector<entity::ParsedRecord>& records);

private:
    /**
     * Create indexes if they don't exist
     * Creates compound index on {chromosome: 1, position: 1}
     */
    void ensure_indexes();

    // Collection reference (lightweight, copyable)
    mongocxx::collection collection_;
};

} // namespace vcf_tool::domain::dao
