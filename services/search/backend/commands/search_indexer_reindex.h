#pragma once
/**
 * @file commands/search_indexer_reindex.h
 * @brief Run one full reindex pass over every
 *        configured index. Split out of
 *        search_indexer.cpp to keep that file
 *        under the project's 100-LOC cap.
 */

#include "search/backend/ElasticClient.h"
#include "search/backend/Indexer.h"
#include "search/backend/SearchTypes.h"

#include <drogon/orm/DbClient.h>
#include <spdlog/spdlog.h>

#include <memory>

namespace commands
{

/**
 * @brief Kick off an async reindex per IndexDef.
 *
 * Each indexer self-reports success/failure via
 * spdlog. Returns immediately — the bulk POSTs run
 * on the Drogon event loop. Safe to call from boot
 * and from the periodic-resync timer.
 */
inline void reindexAll(
    const nextra::search::IndexerConfig& cfg,
    drogon::orm::DbClientPtr db,
    std::shared_ptr<nextra::search::ElasticClient>
        es)
{
    using nextra::search::Indexer;
    for (const auto& d : cfg.indexes) {
        auto ix = std::make_shared<Indexer>(
            db, es, d, cfg.batchSize);
        spdlog::info(
            "search: backfill start name={} "
            "table={} esIndex={}",
            d.name, d.targetTable, d.esIndex);
        ix->reindex(
            [ix](auto) {
                spdlog::info(
                    "search: backfill ok name={}",
                    ix->name());
            },
            [ix](int c, std::string m) {
                spdlog::warn(
                    "search: backfill err name={} "
                    "code={} msg={}",
                    ix->name(), c, m);
            });
    }
}

} // namespace commands
