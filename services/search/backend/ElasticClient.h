#pragma once
/**
 * @file services/search/ElasticClient.h
 * @brief Minimal async ES client used by the
 *        search-indexer daemon.  Lives in its
 *        own namespace so it does not collide
 *        with services::ElasticClient.
 */

#include "search/backend/SearchTypes.h"

#include <drogon/HttpClient.h>

#include <string>

namespace nextra::search
{

/**
 * @class ElasticClient
 * @brief Bulk-capable ES wrapper for the indexer.
 *
 * Unlike services::ElasticClient (single-doc
 * oriented), this one exposes the operations the
 * reindex loop needs: create / drop an index,
 * POST a _bulk payload, and count documents.
 */
class ElasticClient
{
  public:
    /**
     * @brief Construct from a host/port pair.
     */
    ElasticClient(std::string host,
                  std::uint16_t port);

    /// POST /{index}/_bulk with NDJSON body.
    void bulk(const std::string& index,
              const std::string& ndjson,
              EsOk onOk, EsErr onErr);

    /// PUT /{index} (idempotent — swallows 400).
    void createIndex(const std::string& index,
                     EsOk onOk, EsErr onErr);

    /// DELETE /{index}.
    void deleteIndex(const std::string& index,
                     EsOk onOk, EsErr onErr);

    /// GET /{index}/_count -> {"count": N}.
    void count(const std::string& index,
               EsOk onOk, EsErr onErr);

    /// GET /{index}/_search with JSON body.
    void search(const std::string& index,
                const json& query,
                EsOk onOk, EsErr onErr);

    /// PUT /{index}/_doc/{id} with JSON body.
    void indexDoc(const std::string& index,
                  const std::string& id,
                  const json& body,
                  EsOk onOk, EsErr onErr);

    /// DELETE /{index}/_doc/{id}.
    void deleteDoc(const std::string& index,
                   const std::string& id,
                   EsOk onOk, EsErr onErr);

  private:
    std::string host_;
    std::uint16_t port_;
    drogon::HttpClientPtr client_;
};

} // namespace nextra::search
