/**
 * @file SearchAdminController.cpp
 * @brief Admin /api/search/indexes and
 *        /api/search/reindex/:name endpoints.
 */

#include "SearchController.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace controllers
{

void SearchAdminController::listIndexes(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    try {
        auto db = drogon::app().getDbClient();
        auto rows = db->execSqlSync(
            "SELECT id, name, target_table, "
            "es_index, last_reindex_at, "
            "doc_count, status "
            "FROM search_indexes ORDER BY name");
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& r : rows) {
            nlohmann::json o;
            o["id"] = r["id"].as<std::int64_t>();
            o["name"] =
                r["name"].as<std::string>();
            o["target_table"] =
                r["target_table"].as<std::string>();
            o["es_index"] =
                r["es_index"].as<std::string>();
            o["last_reindex_at"] =
                r["last_reindex_at"].isNull()
                    ? ""
                    : r["last_reindex_at"]
                          .as<std::string>();
            o["doc_count"] =
                r["doc_count"].as<std::int64_t>();
            o["status"] =
                r["status"].as<std::string>();
            arr.push_back(o);
        }
        cb(::utils::jsonOk({{"items", arr}}));
    } catch (const std::exception& e) {
        cb(::utils::jsonError(
            drogon::k500InternalServerError,
            e.what()));
    }
}

void SearchAdminController::reindex(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb,
    std::string name)
{
    try {
        auto db = drogon::app().getDbClient();
        db->execSqlSync(
            "UPDATE search_indexes "
            "SET status = 'queued', "
            "updated_at = now() WHERE name = $1",
            name);
        spdlog::info(
            "search: reindex requested for {}",
            name);
        cb(::utils::jsonOk(
            {{"queued", name}}));
    } catch (const std::exception& e) {
        cb(::utils::jsonError(
            drogon::k500InternalServerError,
            e.what()));
    }
}

} // namespace controllers
