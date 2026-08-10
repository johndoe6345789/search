/**
 * @file services/search/ElasticClientDoc.cpp
 * @brief Single-document upsert / delete helpers
 *        for nextra::search::ElasticClient. Split
 *        out from ElasticClient.cpp to keep each
 *        translation unit under the 100-LOC cap.
 */

#include "search/backend/ElasticClient.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

namespace nextra::search
{

namespace
{

void forward(drogon::ReqResult r,
             const drogon::HttpResponsePtr& resp,
             EsOk ok, EsErr err)
{
    if (r != drogon::ReqResult::Ok) {
        err(502, "ES connection failed");
        return;
    }
    auto code = static_cast<int>(
        resp->getStatusCode());
    if (code >= 200 && code < 300) {
        ok(json::parse(resp->getBody(),
                       nullptr, false));
    } else {
        err(code, std::string(resp->getBody()));
    }
}

} // namespace

void ElasticClient::indexDoc(
    const std::string& index,
    const std::string& id,
    const json& body,
    EsOk ok, EsErr err)
{
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Put);
    req->setPath("/" + index + "/_doc/" + id);
    req->setContentTypeString("application/json");
    req->setBody(body.dump());
    client_->sendRequest(
        req, [ok, err](auto r, const auto& resp) {
            forward(r, resp, ok, err);
        });
}

void ElasticClient::deleteDoc(
    const std::string& index,
    const std::string& id,
    EsOk ok, EsErr err)
{
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Delete);
    req->setPath("/" + index + "/_doc/" + id);
    client_->sendRequest(
        req, [ok, err](auto r, const auto& resp) {
            // 404 on delete is a non-error
            // (already gone).
            auto code = static_cast<int>(
                resp ? resp->getStatusCode() : 0);
            if (code == 404) {
                ok(json::object());
                return;
            }
            forward(r, resp, ok, err);
        });
}

} // namespace nextra::search
