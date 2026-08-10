#pragma once
/**
 * @file commands/search_indexer_config.h
 * @brief JSON loader for the search-indexer
 *        constants file.  Kept out of the .cpp
 *        so the daemon entry point stays under
 *        the 100-LOC limit.
 */

#include "search/backend/SearchTypes.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>
#include <string>

namespace commands
{

/**
 * @brief Parse constants/search-indexer.json.
 * @param path Filesystem path to the JSON file.
 * @return Fully populated IndexerConfig.
 * @throws std::runtime_error on IO failure.
 */
inline nextra::search::IndexerConfig
loadSearchConfig(const std::string& path)
{
    using namespace nextra::search;
    std::ifstream f(path);
    if (!f) throw std::runtime_error(
        "cannot open " + path);
    nlohmann::json j;
    f >> j;
    IndexerConfig cfg;
    cfg.esHost = j.value("esHost", cfg.esHost);
    cfg.esPort = j.value("esPort", cfg.esPort);
    cfg.batchSize =
        j.value("batchSize", cfg.batchSize);
    cfg.reindexIntervalSeconds = j.value(
        "reindexIntervalSeconds",
        cfg.reindexIntervalSeconds);
    cfg.kafkaTopic = j.value(
        "kafkaTopic", cfg.kafkaTopic);
    for (const auto& it : j.value(
             "indexes", nlohmann::json::array())) {
        IndexDef d;
        d.name = it.value("name", "");
        d.targetTable =
            it.value("targetTable", "");
        d.esIndex = it.value("esIndex", "");
        d.idField = it.value("idField", "id");
        for (const auto& x : it.value(
                 "fields",
                 nlohmann::json::array()))
            d.fields.push_back(
                x.get<std::string>());
        cfg.indexes.push_back(std::move(d));
    }
    return cfg;
}

} // namespace commands
