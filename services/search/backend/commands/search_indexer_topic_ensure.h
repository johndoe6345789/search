#pragma once
/**
 * @file commands/search_indexer_topic_ensure.h
 * @brief One-shot publish of a noop envelope so the
 *        @c search.reindex topic exists before the
 *        consumer's first poll. Eliminates the
 *        @c "Unknown topic or partition" warning at
 *        cold start when broker auto-create is on.
 *
 * Header-only to keep search_indexer.cpp under the
 * 100-LOC project cap.
 */

#include "infra/backend/IKafkaProducer.h"
#include "infra/backend/KafkaFactory.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>

namespace commands
{

/**
 * @brief Produce a single noop event and flush so
 *        the broker materialises the topic before
 *        the consumer's first metadata fetch.
 * @param topic Logical topic name (typically
 *              @c search.reindex).
 */
inline void ensureSearchTopic(const std::string& topic)
{
    auto producer = nextra::infra::makeKafkaProducer(
        std::string{}, "search-indexer-bootstrap");
    nlohmann::json env;
    env["op"] = "noop";
    const auto payload = env.dump();
    producer->produce(topic, "__bootstrap__",
                      payload);
    producer->flush(2000);
    spdlog::info(
        "search-indexer ensured topic '{}'", topic);
}

} // namespace commands
