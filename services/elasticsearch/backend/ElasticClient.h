#pragma once
/// @file ElasticClient.h — Async ES HTTP client wrapper.

#include <drogon/HttpClient.h>
#include <nlohmann/json.hpp>

#include <cstdint>
#include <functional>
#include <string>

namespace services
{

using json = nlohmann::json;
using EsCallback = std::function<void(json)>;
using EsErrCb = std::function<void(int, std::string)>;

/**
 * @class ElasticClient
 * @brief Async Elasticsearch HTTP client via Drogon.
 *
 * Reads ES_HOST / ES_PORT from environment, falling
 * back to localhost:9200. Implements index, search,
 * delete, health, and createIndex operations.
 */
class ElasticClient
{
  public:
    ElasticClient();
    ~ElasticClient() = default;

    /// @brief Index (upsert) a document.
    void indexDoc(const std::string& index,
                  const std::string& docId,
                  const json& body,
                  EsCallback onOk, EsErrCb onErr);

    /// @brief Search an index with ES query DSL.
    void search(const std::string& index,
                const json& query,
                EsCallback onOk, EsErrCb onErr);

    /// @brief Delete a document by ID.
    void deleteDoc(const std::string& index,
                   const std::string& docId,
                   EsCallback onOk, EsErrCb onErr);

    /// @brief Check cluster health.
    void health(EsCallback onOk, EsErrCb onErr);

    /// @brief Create an index with settings/mappings.
    void createIndex(const std::string& index,
                     const json& settings,
                     EsCallback onOk, EsErrCb onErr);

  private:
    std::string host_;
    std::uint16_t port_;
    drogon::HttpClientPtr client_;
};

} // namespace services
