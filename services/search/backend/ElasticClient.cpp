/**
 * @file services/search/ElasticClient.cpp
 * @brief Async ES calls via drogon::HttpClient.
 */

#include "search/backend/ElasticClient.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

#include <format>

namespace nextra::search
{

namespace
{

/// Forward one Drogon response to our callbacks.
void handle(drogon::ReqResult r,
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

drogon::HttpRequestPtr jsonReq(
    drogon::HttpMethod m, const std::string& path,
    const std::string& body)
{
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(m);
    req->setPath(path);
    req->setContentTypeString(
        "application/x-ndjson");
    req->setBody(body);
    return req;
}

} // namespace

ElasticClient::ElasticClient(std::string host,
                             std::uint16_t port)
    : host_(std::move(host)), port_(port)
{
    auto base =
        std::format("http://{}:{}", host_, port_);
    client_ = drogon::HttpClient::newHttpClient(
        base, drogon::app().getLoop());
    spdlog::info(
        "search::ElasticClient -> {}:{}",
        host_, port_);
}

void ElasticClient::bulk(
    const std::string& index,
    const std::string& ndjson,
    EsOk ok, EsErr err)
{
    auto req = jsonReq(drogon::Post,
        std::format("/{}/_bulk", index), ndjson);
    client_->sendRequest(
        req, [ok, err](auto r, const auto& resp) {
            handle(r, resp, ok, err);
        });
}

void ElasticClient::deleteIndex(
    const std::string& index,
    EsOk ok, EsErr err)
{
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Delete);
    req->setPath("/" + index);
    client_->sendRequest(
        req, [ok, err](auto r, const auto& resp) {
            handle(r, resp, ok, err);
        });
}

} // namespace nextra::search
