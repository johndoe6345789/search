#pragma once
/**
 * @file services/search/events/SearchEventPublisher.h
 * @brief Process-wide helper that emits one
 *        @c search.reindex Kafka message per
 *        domain-table mutation.
 *
 * Domain controllers call @ref publish after a
 * successful DB write. The call is fire-and-forget
 * — never blocks the response, never throws.
 *
 * The producer is built lazily on first use via
 * @ref nextra::infra::makeKafkaProducer, so the
 * stub is selected automatically when librdkafka
 * is not linked.
 */

#include <nlohmann/json.hpp>

#include <string>

namespace nextra::search
{

/**
 * @class SearchEventPublisher
 * @brief Single entry point for search.reindex
 *        events.
 */
class SearchEventPublisher
{
  public:
    /**
     * @brief Publish one search.reindex envelope.
     * @param op        "upsert" or "delete".
     * @param indexName Logical index name from
     *                  search/constants.json
     *                  (e.g. "forum_posts").
     * @param id        Document id as a string.
     * @param doc       Document body for upsert;
     *                  ignored for delete.
     */
    static void publish(
        const std::string& op,
        const std::string& indexName,
        const std::string& id,
        const nlohmann::json& doc);

    /**
     * @brief Convenience overload for delete events
     *        where no doc body is needed.
     */
    static void publishDelete(
        const std::string& indexName,
        const std::string& id);
};

} // namespace nextra::search
