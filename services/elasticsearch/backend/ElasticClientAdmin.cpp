/**
 * @file ElasticClientAdmin.cpp
 * @brief Admin ops: deleteDoc and health.
 *
 * createIndex is in ElasticClientCreateIndex.cpp.
 */

#include "elasticsearch/backend/ElasticClient.h"
#include "elasticsearch/backend/elastic_request.h"

#include <drogon/drogon.h>

#include <format>

namespace services
{

void ElasticClient::deleteDoc(
    const std::string& index,
    const std::string& docId,
    EsCallback onOk, EsErrCb onErr)
{
    auto path =
        std::format("/{}/_doc/{}", index, docId);
    auto req = makeEsSimpleRequest(
        path, drogon::Delete);

    client_->sendRequest(
        req,
        [onOk, onErr](
            drogon::ReqResult r,
            const drogon::HttpResponsePtr& resp) {
            if (r != drogon::ReqResult::Ok) {
                onErr(502, "ES connection failed");
                return;
            }
            onOk(json::parse(
                resp->getBody(),
                nullptr, false));
        });
}

void ElasticClient::health(
    EsCallback onOk, EsErrCb onErr)
{
    auto req = makeEsSimpleRequest(
        "/_cluster/health", drogon::Get);

    client_->sendRequest(
        req,
        [onOk, onErr](
            drogon::ReqResult r,
            const drogon::HttpResponsePtr& resp) {
            if (r != drogon::ReqResult::Ok) {
                onErr(502, "ES connection failed");
                return;
            }
            onOk(json::parse(
                resp->getBody(),
                nullptr, false));
        });
}

} // namespace services
