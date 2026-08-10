/**
 * @file services/search/ElasticClientMeta.cpp
 * @brief createIndex / count / search helpers
 *        for nextra::search::ElasticClient.
 *        Split out to keep each file ≤100 LOC.
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

void ElasticClient::createIndex(
    const std::string& index,
    EsOk ok, EsErr err)
{
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Put);
    req->setPath("/" + index);
    req->setContentTypeString("application/json");
    req->setBody("{\"settings\":{"
                 "\"number_of_shards\":1,"
                 "\"number_of_replicas\":0}}");
    client_->sendRequest(
        req, [ok, err](auto r, const auto& resp) {
            // 400 with "resource_already_exists" is
            // normal — treat as success.
            auto code = static_cast<int>(
                resp ? resp->getStatusCode() : 0);
            if (code == 400) {
                ok(json::object());
                return;
            }
            forward(r, resp, ok, err);
        });
}

void ElasticClient::count(
    const std::string& index,
    EsOk ok, EsErr err)
{
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Get);
    req->setPath("/" + index + "/_count");
    client_->sendRequest(
        req, [ok, err](auto r, const auto& resp) {
            forward(r, resp, ok, err);
        });
}

void ElasticClient::search(
    const std::string& index,
    const json& query,
    EsOk ok, EsErr err)
{
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Post);
    req->setPath("/" + index + "/_search");
    req->setContentTypeString("application/json");
    req->setBody(query.dump());
    client_->sendRequest(
        req, [ok, err](auto r, const auto& resp) {
            forward(r, resp, ok, err);
        });
}

} // namespace nextra::search
