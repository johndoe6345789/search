#pragma once
/**
 * @file commands/search_indexer_dispatch.h
 * @brief Build a Kafka handler that applies
 *        search.reindex envelopes to ES. Header
 *        only to stay tiny. ES HTTP must run on
 *        the Drogon loop — we queueInLoop here.
 */

#include "infra/backend/IKafkaConsumer.h"
#include "search/backend/ElasticClient.h"
#include "search/backend/SearchTypes.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace commands
{

/// Build the dispatcher closure. cfg supplies the
/// logical-name → esIndex map; es is kept alive
/// by search_indexer.cpp.
inline nextra::infra::KafkaMessageHandler
buildSearchDispatcher(
    const nextra::search::IndexerConfig& cfg,
    std::shared_ptr<nextra::search::ElasticClient> es)
{
    auto map = std::make_shared<std::unordered_map<
        std::string, std::string>>();
    for (const auto& ix : cfg.indexes)
        (*map)[ix.name] = ix.esIndex;

    return [map, es](const std::string& key,
                     const std::string& payload) {
        nextra::search::json env;
        try {
            env = nextra::search::json::parse(payload);
        } catch (...) {
            spdlog::warn(
                "search dispatch: bad JSON key={}",
                key);
            return;
        }
        const auto op = env.value("op", "");
        const auto logical = env.value("index", "");
        const auto id = env.value("id", "");
        if (op == "noop") return;
        if (op.empty() || logical.empty() ||
            id.empty()) return;
        const auto it = map->find(logical);
        if (it == map->end()) {
            spdlog::warn(
                "search: unknown index {}", logical);
            return;
        }
        const auto esIndex = it->second;
        nextra::search::json doc = env.value(
            "doc", nextra::search::json::object());
        drogon::app().getLoop()->queueInLoop(
            [es, op, esIndex, id, doc]() {
                auto ok = [esIndex, id, op](auto) {
                    spdlog::info("search {} ok {}/{}",
                        op, esIndex, id);
                };
                auto err = [esIndex, id, op](
                    int c, std::string b) {
                    spdlog::error(
                        "search {} err {}/{} {} {}",
                        op, esIndex, id, c, b);
                };
                if (op == "upsert")
                    es->indexDoc(esIndex, id, doc,
                                 ok, err);
                else if (op == "delete")
                    es->deleteDoc(esIndex, id,
                                  ok, err);
            });
    };
}

} // namespace commands
