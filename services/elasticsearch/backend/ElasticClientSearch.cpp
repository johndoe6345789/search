/**
 * @file ElasticClientSearch.cpp
 * @brief ES search operation (extracted from
 *        ElasticClient.cpp).
 */

#include "elasticsearch/backend/ElasticClient.h"
#include "elasticsearch/backend/elastic_request.h"

#include <drogon/drogon.h>

#include <format>

namespace services
{

void ElasticClient::search(
    const std::string& index,
    const json& query,
    EsCallback onOk, EsErrCb onErr)
{
    auto path =
        std::format("/{}/_search", index);
    auto req = makeEsPostRequest(path, query);

    client_->sendRequest(
        req,
        [onOk, onErr](
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
                onErr(code,
                    std::string(resp->getBody()));
            }
        });
}

} // namespace services
