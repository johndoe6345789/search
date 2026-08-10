#pragma once
/**
 * @file SearchController.h
 * @brief Public + admin REST controllers for the
 *        search indexer daemon (Phase 4.5).
 */

#include <drogon/HttpController.h>

namespace controllers
{

/**
 * @class SearchController
 * @brief Public read-only search endpoints.
 *
 * - GET /api/search          full-result query
 * - GET /api/search/suggest  top-N autocomplete
 * - GET /api/search/health   indexer status probe
 */
class SearchController
    : public drogon::HttpController<SearchController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        SearchController::search,
        "/api/search", drogon::Get);
    ADD_METHOD_TO(
        SearchController::suggest,
        "/api/search/suggest", drogon::Get);
    ADD_METHOD_TO(
        SearchController::health,
        "/api/search/health", drogon::Get);
    METHOD_LIST_END

    /// Handle a public full-result search.
    void search(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /// Federated top-N autocomplete handler.
    void suggest(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /// Indexer health probe used by alerts.
    void health(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

/**
 * @class SearchAdminController
 * @brief Admin-only routes for index health and
 *        manual reindex triggers.
 */
class SearchAdminController
    : public drogon::HttpController<
          SearchAdminController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        SearchAdminController::listIndexes,
        "/api/search/indexes", drogon::Get);
    ADD_METHOD_TO(
        SearchAdminController::reindex,
        "/api/search/reindex/{name}",
        drogon::Post);
    METHOD_LIST_END

    /// List all rows from search_indexes.
    void listIndexes(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /// Trigger a reindex for a named index.
    void reindex(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb,
        std::string name);
};

} // namespace controllers
