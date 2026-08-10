#pragma once
/**
 * @file elastic_client_init.h
 * @brief ES client environment-based initialization
 *        helpers.
 */

#include <cstdint>
#include <cstdlib>
#include <string>

namespace services
{

/**
 * @brief Read ES_HOST from environment.
 * @return Host string or "localhost".
 */
inline auto esHost() -> std::string
{
    auto* env = std::getenv("ES_HOST");
    return env ? std::string(env) : "localhost";
}

/**
 * @brief Read ES_PORT from environment.
 * @return Port number or 9200.
 */
inline auto esPort() -> std::uint16_t
{
    auto* env = std::getenv("ES_PORT");
    if (env) {
        return static_cast<std::uint16_t>(
            std::stoi(env));
    }
    return 9200;
}

} // namespace services
