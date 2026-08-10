#pragma once
/**
 * @file elastic_request.h
 * @brief Shared HTTP request helpers for ElasticClient.
 */

#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>
#include <nlohmann/json.hpp>

#include <format>
#include <string>

namespace services
{

using json = nlohmann::json;

/**
 * @brief Create a JSON PUT request.
 * @param path  URL path.
 * @param body  JSON body to send.
 * @return Configured HTTP request.
 */
inline auto makeEsPutRequest(
    const std::string& path,
    const json& body) -> drogon::HttpRequestPtr
{
    auto req =
        drogon::HttpRequest::newHttpRequest();
    req->setPath(path);
    req->setMethod(drogon::Put);
    req->setContentTypeCode(
        drogon::CT_APPLICATION_JSON);
    req->setBody(body.dump());
    return req;
}

/**
 * @brief Create a JSON POST request.
 * @param path  URL path.
 * @param body  JSON body to send.
 * @return Configured HTTP request.
 */
inline auto makeEsPostRequest(
    const std::string& path,
    const json& body) -> drogon::HttpRequestPtr
{
    auto req =
        drogon::HttpRequest::newHttpRequest();
    req->setPath(path);
    req->setMethod(drogon::Post);
    req->setContentTypeCode(
        drogon::CT_APPLICATION_JSON);
    req->setBody(body.dump());
    return req;
}

/**
 * @brief Create a simple GET/DELETE request.
 * @param path    URL path.
 * @param method  HTTP method.
 * @return Configured HTTP request.
 */
inline auto makeEsSimpleRequest(
    const std::string& path,
    drogon::HttpMethod method)
    -> drogon::HttpRequestPtr
{
    auto req =
        drogon::HttpRequest::newHttpRequest();
    req->setPath(path);
    req->setMethod(method);
    return req;
}

} // namespace services
