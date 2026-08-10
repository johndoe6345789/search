/// @file ElasticClient.cpp -- ES constructor and indexDoc.
/// search is in ElasticClientSearch.cpp.
#include "elasticsearch/backend/ElasticClient.h"
#include "elasticsearch/backend/elastic_client_init.h"
#include "elasticsearch/backend/elastic_request.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

#include <format>

namespace services
{

ElasticClient::ElasticClient()
    : host_(esHost()), port_(esPort())
{
    auto base =
        std::format("http://{}:{}", host_, port_);
    client_ = drogon::HttpClient::newHttpClient(
        base, drogon::app().getLoop());
    spdlog::info(
        "ElasticClient -> {}:{}", host_, port_);
}

void ElasticClient::indexDoc(
    const std::string& index,
    const std::string& docId,
    const json& body,
    EsCallback onOk, EsErrCb onErr)
{
    auto path =
        std::format("/{}/_doc/{}", index, docId);
    auto req = makeEsPutRequest(path, body);

    client_->sendRequest(
        req,
        [onOk, onErr, index, docId](
            drogon::ReqResult result,
            const drogon::HttpResponsePtr& resp) {
            if (result != drogon::ReqResult::Ok) {
                onErr(502, "ES connection failed");
                return;
            }
            auto code = static_cast<int>(
                resp->getStatusCode());
            if (code >= 200 && code < 300) {
                onOk(json::parse(
                    resp->getBody(),
                    nullptr, false));
            } else {
                spdlog::warn(
                    "ES index {}/{}: {}",
                    index, docId, code);
                onErr(code,
                    std::string(resp->getBody()));
            }
        });
}

} // namespace services
