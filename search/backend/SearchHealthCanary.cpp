/**
 * @file SearchHealthCanary.cpp
 * @brief Implementation of the doc-count canary.
 */

#include "search/backend/SearchHealthCanary.h"

#include "alerts/backend/AlertEmitter.h"

#include <drogon/drogon.h>
#include <drogon/orm/Exception.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <format>
#include <string>

namespace nextra::search
{

namespace
{

void compareOne(const std::string& name,
                const std::string& esIndex,
                std::int64_t prior,
                std::shared_ptr<ElasticClient> es)
{
    if (prior <= 0) return;
    es->count(esIndex,
        [name, esIndex, prior](nlohmann::json r) {
            std::int64_t cur = r.value("count", -1);
            if (cur == 0) {
                nextra::alerts::alertEmitter().emit(
                    "search-indexer", "error",
                    std::format(
                        "data loss in {}: {} -> 0 docs",
                        esIndex, prior),
                    std::format(
                        "search-indexer.dataloss.{}",
                        esIndex),
                    nlohmann::json{
                        {"index", esIndex},
                        {"prior", prior},
                        {"name",  name}});
            }
        },
        [esIndex](int c, std::string m) {
            spdlog::debug(
                "canary count {}: {} {}",
                esIndex, c, m);
        });
}

} // namespace

void armHealthCanary(
    drogon::orm::DbClientPtr db,
    std::shared_ptr<ElasticClient> es)
{
    drogon::app().getLoop()->runEvery(60.0,
        [db, es] {
            db->execSqlAsync(
                "SELECT name, es_index, doc_count "
                "  FROM search_indexes",
                [es](const drogon::orm::Result& rows) {
                    for (const auto& r : rows) {
                        compareOne(
                            r["name"].as<std::string>(),
                            r["es_index"]
                                .as<std::string>(),
                            r["doc_count"]
                                .as<std::int64_t>(),
                            es);
                    }
                },
                [](const drogon::orm::DrogonDbException&
                       e) {
                    spdlog::warn(
                        "canary db read: {}",
                        e.base().what());
                });
        });
    spdlog::info("search: doc-count canary armed (60s)");
}

} // namespace nextra::search
