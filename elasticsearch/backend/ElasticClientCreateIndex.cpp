/**
 * @file ElasticClientCreateIndex.cpp
 * @brief ES createIndex operation (extracted from
 *        ElasticClientAdmin.cpp).
 */

#include "elasticsearch/backend/ElasticClient.h"
#include "elasticsearch/backend/elastic_request.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

#include <format>

namespace services
{

void ElasticClient::createIndex(
    const std::string& index,
    const json& settings,
    EsCallback onOk, EsErrCb onErr)
{
    auto path = std::format("/{}", index);
    auto req = makeEsPutRequest(path, settings);

    client_->sendRequest(
        req,
        [onOk, onErr, index](
            drogon::ReqResult r,
            const drogon::HttpResponsePtr& resp) {
            if (r != drogon::ReqResult::Ok) {
                onErr(502, "ES connection failed");
                return;
            }
            auto code = static_cast<int>(
                resp->getStatusCode());
            if (code >= 200 && code < 300) {
                spdlog::info(
                    "ES index created: {}",
                    index);
                onOk(json::parse(
                    resp->getBody(),
                    nullptr, false));
            } else {
                spdlog::warn(
                    "ES create index {}: {}",
                    index, code);
                onErr(code,
                    std::string(resp->getBody()));
            }
        });
}

} // namespace services
