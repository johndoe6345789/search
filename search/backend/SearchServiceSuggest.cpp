/// @file SearchServiceSuggest.cpp — federated
/// autocomplete across all 6 search indexes.
#include "search/backend/SearchService.h"
#include "search/backend/SearchIndexRegistry.h"
#include "search/backend/SuggestShape.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <string>
#include <vector>

namespace services
{

namespace
{

constexpr std::int32_t kSuggestMax = 50;
constexpr std::int32_t kSuggestDefault = 5;

std::string indexList()
{
    return configuredEsIndexList();
}

} // namespace

void SearchService::suggest(
    const std::string& query, std::int32_t limit,
    Callback onOk, ErrCallback onErr)
{
    if (query.empty()) {
        onOk({{"items", json::array()}});
        return;
    }
    limit = std::clamp(
        limit > 0 ? limit : kSuggestDefault,
        1, kSuggestMax);

    json esQuery = {
        {"size", limit},
        {"query", {
            {"multi_match", {
                {"query", query},
                {"fields", {"title^3", "name^3",
                            "username^2",
                            "display_name^2",
                            "sku^2", "slug",
                            "description", "body",
                            "body_md", "bio", "tags"}},
                {"type", "best_fields"},
                {"fuzziness", "AUTO"}
            }}
        }}
    };

    es_.search(
        indexList(), esQuery,
        [onOk](json r) {
            json items = json::array();
            auto hits = r.value(
                "/hits/hits"_json_pointer,
                json::array());
            for (const auto& h : hits) {
                items.push_back(
                    suggest_shape::shape(h));
            }
            onOk({{"items", items}});
        },
        [onErr](int c, std::string m) {
            spdlog::warn(
                "suggest ES err {}: {}", c, m);
            onErr(drogon::k502BadGateway,
                  std::move(m));
        });
}

} // namespace services
