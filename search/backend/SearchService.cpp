/// @file SearchService.cpp — searchAll implementation.
#include "search/backend/SearchService.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <format>
#include <string>
#include <vector>

namespace services
{

static const std::vector<std::string> kAllIndices = {
    "nextra-forum", "nextra-wiki", "nextra-blog",
    "nextra-products", "nextra-gallery", "nextra-users"};

static constexpr std::int32_t kMaxSize = 100;
static constexpr std::int32_t kDefaultSize = 20;

SearchService::SearchService() = default;

void SearchService::searchAll(
    const std::string& query, std::int32_t page,
    std::int32_t perPage, Callback onOk, ErrCallback onErr)
{
    if (query.empty()) {
        onErr(drogon::k400BadRequest, "query is required");
        return;
    }

    perPage = std::clamp(perPage, 1, kMaxSize);
    page = std::max(page, 1);
    auto from = (page - 1) * perPage;

    // Comma-separated index list for multi-index search.
    std::string indexList;
    for (std::size_t i = 0; i < kAllIndices.size(); ++i) {
        if (i > 0) indexList += ',';
        indexList += kAllIndices[i];
    }

    json esQuery = {
        {"from", from},
        {"size", perPage},
        {"query", {
            {"multi_match", {
                {"query", query},
                {"fields", {"*"}},
                {"type", "best_fields"},
                {"fuzziness", "AUTO"}
            }}
        }},
        {"highlight", {
            {"pre_tags", {"<em>"}},
            {"post_tags", {"</em>"}},
            {"fields", {{"*", json::object()}}}
        }}
    };

    spdlog::debug("searchAll q='{}' page={} size={}",
                  query, page, perPage);

    es_.search(
        indexList, esQuery,
        [onOk](json result) {
            onOk(std::move(result));
        },
        [onErr](int code, std::string msg) {
            spdlog::warn("searchAll ES err {}: {}", code, msg);
            onErr(drogon::k502BadGateway, std::move(msg));
        });
}

} // namespace services
