/// @file SearchIndexInit.cpp — ES index bootstrap.
#include "search/backend/SearchIndexInit.h"
#include "search/backend/SearchIndexMappings.h"

#include <spdlog/spdlog.h>

namespace services
{

static const json kSettings = {
    {"number_of_shards", 1},
    {"number_of_replicas", 0},
    {"refresh_interval", "1s"}
};

void SearchIndexInit::createAll(ElasticClient& es)
{
    spdlog::info("SearchIndexInit: creating indices");
    for (const auto& [name, mappings] :
         allIndexMappings()) {
        createOne(es, name, mappings);
    }
}

void SearchIndexInit::createOne(
    ElasticClient& es, const std::string& name,
    const json& mappings)
{
    json body = {{"settings", kSettings},
                 {"mappings", mappings}};

    es.createIndex(
        name, body,
        [name](json /*res*/) {
            spdlog::info("Index {} ready", name);
        },
        [name](int code, std::string msg) {
            // 400 = index already exists — not an error.
            if (code == 400) {
                spdlog::debug("Index {} exists", name);
            } else {
                spdlog::error("Index {} failed {}: {}",
                              name, code, msg);
            }
        });
}

} // namespace services
