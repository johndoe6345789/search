#pragma once
/**
 * @file SearchIndexRegistry.h
 * @brief The set of Elasticsearch index names this
 *        deployment serves, read from the same
 *        constants.json the indexer uses. Falls back
 *        to the Nextra defaults when no file is found,
 *        so a product swaps its index set by shipping
 *        a different constants.json (overlay), not by
 *        changing code.
 *
 * Lookup order: $SEARCH_CONSTANTS_PATH,
 * constants/search-indexer.json,
 * search/constants.json, constants.json.
 */

#include <nlohmann/json.hpp>

#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

namespace services
{

inline const std::vector<std::string>&
configuredEsIndices()
{
    static const std::vector<std::string> v = [] {
        std::vector<std::string> paths;
        if (const char* e =
                std::getenv("SEARCH_CONSTANTS_PATH"))
            paths.emplace_back(e);
        paths.insert(paths.end(),
            {"constants/search-indexer.json",
             "search/constants.json",
             "constants.json"});
        for (const auto& p : paths) {
            std::ifstream f(p);
            if (!f) continue;
            auto j = nlohmann::json::parse(
                f, nullptr, false);
            if (j.is_discarded() ||
                !j.contains("indexes")) continue;
            std::vector<std::string> out;
            for (const auto& ix : j["indexes"])
                if (ix.contains("esIndex"))
                    out.push_back(
                        ix["esIndex"]
                            .get<std::string>());
            if (!out.empty()) return out;
        }
        return std::vector<std::string>{
            "nextra-forum", "nextra-wiki",
            "nextra-blog", "nextra-products",
            "nextra-gallery", "nextra-users"};
    }();
    return v;
}

inline std::string configuredEsIndexList()
{
    std::string out;
    for (const auto& n : configuredEsIndices()) {
        if (!out.empty()) out += ',';
        out += n;
    }
    return out;
}

} // namespace services
