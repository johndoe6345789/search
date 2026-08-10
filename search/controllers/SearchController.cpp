/**
 * @file SearchController.cpp
 * @brief Public /api/search endpoint — delegates
 *        to the existing SearchService facade.
 */

#include "SearchController.h"
#include "search/backend/SearchService.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <cstdint>
#include <string>

namespace controllers
{

void SearchController::search(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    auto q = req->getParameter("q");
    if (q.empty()) {
        cb(::utils::jsonError(
            drogon::k400BadRequest,
            "query parameter 'q' is required"));
        return;
    }

    std::int32_t page = 1;
    std::int32_t size = 20;

    auto pageStr = req->getParameter("page");
    if (!pageStr.empty()) page = std::stoi(pageStr);
    auto sizeStr = req->getParameter("size");
    if (!sizeStr.empty()) size = std::stoi(sizeStr);

    auto svc =
        std::make_shared<services::SearchService>();

    svc->searchAll(
        q, page, size,
        [cb, svc](nlohmann::json result) {
            cb(::utils::jsonOk(result));
        },
        [cb, svc](drogon::HttpStatusCode code,
                  std::string msg) {
            cb(::utils::jsonError(code, msg));
        });
}

} // namespace controllers
