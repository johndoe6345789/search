/**
 * @file SearchSuggestController.cpp
 * @brief /api/search/suggest handler — federated
 *        top-N autocomplete across every index.
 */

#include "SearchController.h"
#include "search/backend/SearchService.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <cstdint>
#include <memory>
#include <string>

namespace controllers
{

void SearchController::suggest(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    auto q = req->getParameter("q");
    auto limitStr = req->getParameter("limit");
    std::int32_t limit = 5;
    if (!limitStr.empty()) {
        try { limit = std::stoi(limitStr); }
        catch (...) { limit = 5; }
    }

    auto svc =
        std::make_shared<services::SearchService>();
    svc->suggest(
        q, limit,
        [cb, svc](nlohmann::json r) {
            cb(::utils::jsonOk(r));
        },
        [cb, svc](drogon::HttpStatusCode code,
                  std::string msg) {
            cb(::utils::jsonError(code, msg));
        });
}

} // namespace controllers
