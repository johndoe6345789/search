#pragma once
/**
 * @file SearchHealthCanary.h
 * @brief Periodic doc-count canary: detects ES indexes
 *        that have lost all docs since the last poll
 *        and emits a search-indexer.dataloss alert.
 */

#include "search/backend/ElasticClient.h"

#include <drogon/orm/DbClient.h>

#include <memory>

namespace nextra::search
{

/**
 * @brief Arm a 60-second loop on the Drogon event loop
 *        that compares ES /_count against the prior
 *        @c search_indexes.doc_count for each row.
 *
 * Fire-and-forget — installs a periodic callback and
 * returns immediately.
 *
 * @param db  Drogon DB client (search domain DB).
 * @param es  Indexer-side ES client used for /_count.
 */
void armHealthCanary(drogon::orm::DbClientPtr db,
                     std::shared_ptr<ElasticClient> es);

} // namespace nextra::search
