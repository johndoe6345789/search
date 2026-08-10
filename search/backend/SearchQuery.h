#pragma once
/**
 * @file services/search/SearchQuery.h
 * @brief Builds ES query DSL from simple
 *        SearchQueryParams (q + filters).
 */

#include "search/backend/SearchTypes.h"

namespace nextra::search
{

/**
 * @class SearchQueryBuilder
 * @brief Turns a @ref SearchQueryParams into an
 *        Elasticsearch query body.
 *
 * Supported DSL: multi_match across common text
 * fields, term filters in a bool/filter clause,
 * from / size pagination.  The builder is
 * stateless — all state is in the input params.
 */
class SearchQueryBuilder
{
  public:
    /**
     * @brief Build the full JSON body for a
     *        POST /_search request.
     * @param p The caller's query parameters.
     * @return A json object ready to .dump().
     */
    static json build(const SearchQueryParams& p);

    /**
     * @brief Parse a Drogon-style query string
     *        ("q=hello&type=user&from=0")
     *        into a SearchQueryParams.
     */
    static SearchQueryParams fromParams(
        const std::string& q,
        const std::string& type,
        std::int32_t from,
        std::int32_t size);
};

} // namespace nextra::search
