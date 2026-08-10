#pragma once
/**
 * @file services/search/Indexer.h
 * @brief Pull rows from PG, POST to ES /_bulk.
 */

#include "search/backend/ElasticClient.h"
#include "search/backend/SearchTypes.h"

#include <drogon/orm/DbClient.h>

#include <memory>
#include <string>

namespace nextra::search
{

/**
 * @class Indexer
 * @brief Single-index reindex driver.
 *
 * @details Reads rows from @c targetTable in
 * batches of @c batchSize, formats them into ES
 * _bulk NDJSON (one index action per doc), POSTs
 * to the ES bulk endpoint, and updates the
 * search_indexes row with the new doc_count,
 * last_reindex_at, and status.
 */
class Indexer
{
  public:
    Indexer(drogon::orm::DbClientPtr db,
            std::shared_ptr<ElasticClient> es,
            IndexDef def, std::int32_t batchSize);

    /**
     * @brief Run a full reindex of one index.
     *
     * Creates the ES index if missing, walks
     * every row from the source table, and
     * finally writes status = 'idle' on success
     * or 'error' on failure.
     */
    void reindex(EsOk onOk, EsErr onErr);

    /// Name of the registered index.
    const std::string& name() const
    {
        return def_.name;
    }

  private:
    /// Build an NDJSON _bulk payload from a
    /// batch of rows, one index action per doc.
    std::string buildNdjson(
        const drogon::orm::Result& rows) const;

    /// Run a single SELECT+bulk round after the
    /// ES index has been ensured to exist.
    void runBulk(EsOk onOk, EsErr onErr);

    /// Update search_indexes bookkeeping.
    void writeStatus(
        const std::string& status,
        std::int64_t docCount);

    drogon::orm::DbClientPtr db_;
    std::shared_ptr<ElasticClient> es_;
    IndexDef def_;
    std::int32_t batchSize_;
};

} // namespace nextra::search
