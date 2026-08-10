#pragma once
/**
 * @file SearchIndexInit.h
 * @brief Creates all nextra Elasticsearch indices on startup.
 */

#include "elasticsearch/backend/ElasticClient.h"

#include <nlohmann/json.hpp>

namespace services
{

using json = nlohmann::json;

/**
 * @class SearchIndexInit
 * @brief Static helper to ensure all nextra ES indices
 *        exist with correct mappings.
 */
class SearchIndexInit
{
  public:
    /**
     * @brief Create all nextra indices with mappings.
     * @param es Reference to an initialised ElasticClient.
     *
     * Safe to call repeatedly; ES ignores create-if-exists
     * when the index already exists (returns 400 which we
     * silently discard).
     */
    static void createAll(ElasticClient& es);

  private:
    /// @brief Create a single index with given mappings.
    static void createOne(ElasticClient& es,
                          const std::string& name,
                          const json& mappings);
};

} // namespace services
