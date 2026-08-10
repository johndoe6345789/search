/**
 * @file services/search/SearchQuery.cpp
 * @brief SearchQueryBuilder implementation.
 */

#include "search/backend/SearchQuery.h"

namespace nextra::search
{

json SearchQueryBuilder::build(
    const SearchQueryParams& p)
{
    json body = json::object();
    body["from"] = p.from;
    body["size"] = p.size;

    json bool_ = json::object();
    json must = json::array();
    json filter = json::array();

    if (!p.q.empty()) {
        must.push_back({
            {"multi_match", {
                {"query", p.q},
                {"fields", json::array({
                    "title^2", "name^2", "body",
                    "description", "username",
                    "email"})},
                {"fuzziness", "AUTO"}
            }}
        });
    } else {
        must.push_back({
            {"match_all", json::object()}});
    }

    for (const auto& [k, v] : p.filters) {
        filter.push_back({
            {"term", {{k, v}}}
        });
    }

    bool_["must"] = must;
    if (!filter.empty()) bool_["filter"] = filter;
    body["query"] = {{"bool", bool_}};
    return body;
}

SearchQueryParams SearchQueryBuilder::fromParams(
    const std::string& q,
    const std::string& type,
    std::int32_t from,
    std::int32_t size)
{
    SearchQueryParams p;
    p.q = q;
    p.from = from < 0 ? 0 : from;
    p.size = (size <= 0 || size > 100) ? 20 : size;
    if (!type.empty()) {
        p.filters.emplace_back(
            "resource_type", type);
    }
    return p;
}

} // namespace nextra::search
