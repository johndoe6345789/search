/**
 * @file SearchHealthController.cpp
 * @brief /api/search/health — alerts probe.
 *
 * Pulls per-index metadata from search_indexes
 * (the indexer's bookkeeping table) and reports
 * a roll-up status. Returns 503 when any index
 * has zero docs, so the alerts service can flag a
 * failed boot backfill or an empty source table
 * that should not be empty.
 */

#include "SearchController.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace controllers
{

void SearchController::health(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    nlohmann::json out;
    out["status"] = "ok";
    out["perIndex"] = nlohmann::json::array();

    try {
        auto db = drogon::app().getDbClient();
        auto rows = db->execSqlSync(
            "SELECT name, doc_count, status, "
            "last_reindex_at "
            "FROM search_indexes ORDER BY name");
        bool anyZero = false;
        bool anyError = false;
        for (const auto& r : rows) {
            nlohmann::json row;
            row["name"] = r["name"].as<std::string>();
            row["docs"] =
                r["doc_count"].as<std::int64_t>();
            row["status"] =
                r["status"].as<std::string>();
            row["lastReindex"] =
                r["last_reindex_at"].isNull()
                    ? ""
                    : r["last_reindex_at"]
                          .as<std::string>();
            if (row["docs"].get<std::int64_t>() == 0)
                anyZero = true;
            if (row["status"] == "error")
                anyError = true;
            out["perIndex"].push_back(row);
        }
        if (anyError) out["status"] = "down";
        else if (anyZero) out["status"] = "degraded";

        if (anyZero || anyError) {
            auto resp = ::utils::jsonOk(out);
            resp->setStatusCode(
                drogon::k503ServiceUnavailable);
            cb(resp);
            return;
        }
        cb(::utils::jsonOk(out));
    } catch (const std::exception& e) {
        spdlog::warn("search health err: {}",
                     e.what());
        cb(::utils::jsonError(
            drogon::k500InternalServerError,
            e.what()));
    }
}

} // namespace controllers
