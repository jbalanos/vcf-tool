#include "VcfDao.h"
#include "VcfSchema.h"

#include <vcf_tool/core/MongoDatabase.h>
#include <vcf_tool/utils/Logger.h>
#include <vcf_tool/utils/Errors.h>
#include <vcf_tool/utils/Format.h>

#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/options/insert.hpp>
#include <mongocxx/options/index.hpp>
#include <bsoncxx/builder/stream/document.hpp>

namespace vcf_tool::domain::dao {

VcfDao::VcfDao()
    : collection_(core::MongoDatabase::instance().get_collection())
{
    ensure_indexes();
}

void VcfDao::ensure_indexes() {
    using bsoncxx::builder::stream::document;
    using bsoncxx::builder::stream::finalize;

    try {
        // Create compound index on {chromosome: 1, position: 1}
        // This optimizes queries filtering by chromosome and/or position
        auto index_spec = document{}
            << "chromosome" << 1
            << "position" << 1
            << finalize;

        mongocxx::options::index index_opts{};
        index_opts.background(true);  // Non-blocking index creation

        collection_.create_index(index_spec.view(), index_opts);
        LOG_DEBUG("Created index on {chromosome, position}");

    } catch (const mongocxx::operation_exception& e) {
        // Index likely already exists - this is fine
        LOG_DEBUG_F("Index creation note: {}", e.what());
    }
}

void VcfDao::insert(const VcfRecord& record) {
    try {
        auto bson_doc = VcfSchema::to_bson(record);
        collection_.insert_one(bson_doc.view());

    } catch (const mongocxx::exception& e) {
        throw utils::errors::DatabaseError(
            utils::format("MongoDB insert failed: {}", e.what()),
            utils::errors::Component::Database
        );
    }
}

std::size_t VcfDao::bulk_insert(const std::vector<entity::ParsedRecord>& records) {
    if (records.empty()) {
        return 0;
    }

    try {
        // Extract VcfRecords from ParsedRecords
        std::vector<VcfRecord> vcf_records;
        vcf_records.reserve(records.size());
        for (const auto& parsed : records) {
            vcf_records.push_back(parsed.vcf_data);
        }

        // Convert to BSON batch
        auto bson_docs = VcfSchema::to_bson_batch(vcf_records);

        // Prepare bulk write operation
        mongocxx::options::insert insert_opts;
        insert_opts.ordered(false);  // Parallel writes, continue on error

        // Build vector of document views
        std::vector<bsoncxx::document::view> doc_views;
        doc_views.reserve(bson_docs.size());
        for (const auto& doc : bson_docs) {
            doc_views.push_back(doc.view());
        }

        // Execute bulk insert
        auto result = collection_.insert_many(doc_views, insert_opts);

        std::size_t inserted_count = result ? static_cast<std::size_t>(result->inserted_count()) : 0;

        LOG_DEBUG_F("Bulk inserted {} VCF records into MongoDB", inserted_count);

        return inserted_count;

    } catch (const mongocxx::bulk_write_exception& e) {
        // Partial success case - some documents inserted, some failed
        // bulk_write_exception is thrown even on partial success
        // We'll log the error but return 0 since we can't get partial count easily
        LOG_WARN_F("Bulk insert failed: {} Error: {}", records.size(), e.what());
        return 0;

    } catch (const mongocxx::exception& e) {
        throw utils::errors::DatabaseError(
            utils::format("MongoDB bulk insert failed: {}", e.what()),
            utils::errors::Component::Database
        );
    }
}

} // namespace vcf_tool::domain::dao
