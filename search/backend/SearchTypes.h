#pragma once
/**
 * @file SearchTypes.h
 * @brief Plain value types shared across the
 *        search-indexer daemon.
 */

#include <nlohmann/json.hpp>

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace nextra::search
{

using json = nlohmann::json;

/**
 * @brief One registered logical index.
 */
struct IndexDef
{
    std::string name;
    std::string targetTable;
    std::string esIndex;
    std::vector<std::string> fields;
    std::string idField;
};

/**
 * @brief Daemon-wide configuration loaded from
 *        constants/search-indexer.json.
 */
struct IndexerConfig
{
    std::string esHost{"elasticsearch"};
    std::uint16_t esPort{9200};
    std::int32_t batchSize{500};
    std::int32_t reindexIntervalSeconds{3600};
    std::string kafkaTopic{"search.reindex"};
    std::string kafkaGroup{"search-indexer"};
    std::uint16_t httpPort{8086};
    std::vector<IndexDef> indexes;
};

/// Async result of an Elasticsearch HTTP call.
using EsOk = std::function<void(json)>;
using EsErr = std::function<
    void(int, std::string)>;

/**
 * @brief Simple search query descriptor: a text
 *        term plus a filter map (field -> value).
 */
struct SearchQueryParams
{
    std::string q;
    std::int32_t from{0};
    std::int32_t size{20};
    std::vector<std::pair<std::string,
                          std::string>> filters;
};

// Wire shape of `search.reindex` Kafka events is
// documented in KAFKA_EVENT.md alongside this file.

/**
 * @brief Interface for the Kafka consumer. The
 *        real librdkafka-backed impl lives behind
 *        NEXTRA_HAVE_KAFKA; a no-op stub is used
 *        in dev builds today.
 */
class IKafkaConsumer
{
  public:
    virtual ~IKafkaConsumer() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
};

} // namespace nextra::search
