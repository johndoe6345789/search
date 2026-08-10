/**
 * @file services/search/IndexerHelpers.cpp
 * @brief NDJSON builder + status bookkeeping for
 *        Indexer.  Split out to keep Indexer.cpp
 *        under the 100-LOC cap.
 */

#include "search/backend/Indexer.h"

#include <drogon/orm/Exception.h>
#include <spdlog/spdlog.h>

#include <sstream>

namespace nextra::search
{

std::string Indexer::buildNdjson(
    const drogon::orm::Result& rows) const
{
    std::ostringstream out;
    for (const auto& row : rows) {
        json doc = json::object();
        for (const auto& f : def_.fields) {
            if (row[f].isNull()) continue;
            doc[f] = row[f].as<std::string>();
        }
        auto id = row[def_.idField]
                      .as<std::string>();
        out << "{\"index\":{\"_id\":\""
            << id << "\"}}\n";
        out << doc.dump() << "\n";
    }
    return out.str();
}

void Indexer::writeStatus(
    const std::string& status,
    std::int64_t docCount)
{
    auto name = def_.name;
    db_->execSqlAsync(
        "UPDATE search_indexes SET "
        "status = $1, doc_count = $2, "
        "last_reindex_at = now(), "
        "updated_at = now() WHERE name = $3",
        [](const drogon::orm::Result&) {},
        [name](
            const drogon::orm::DrogonDbException& e) {
            spdlog::warn(
                "search: status update {}: {}",
                name, e.base().what());
        },
        status, docCount, def_.name);
}

} // namespace nextra::search
