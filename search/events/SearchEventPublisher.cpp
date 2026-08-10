/**
 * @file services/search/events/SearchEventPublisher.cpp
 * @brief Lazy singleton wrapper around
 *        @ref nextra::infra::IKafkaProducer.
 */

#include "search/events/SearchEventPublisher.h"

#include "infra/backend/IKafkaProducer.h"
#include "infra/backend/KafkaFactory.h"

#include <spdlog/spdlog.h>

#include <memory>
#include <mutex>

namespace nextra::search
{

namespace
{
constexpr const char* kTopic = "search.reindex";

std::mutex g_mu;
std::unique_ptr<nextra::infra::IKafkaProducer>
    g_producer;

nextra::infra::IKafkaProducer& producer()
{
    std::lock_guard lk(g_mu);
    if (!g_producer) {
        g_producer = nextra::infra::makeKafkaProducer(
            std::string{}, "search-event-publisher");
        spdlog::info(
            "SearchEventPublisher producer ready");
    }
    return *g_producer;
}

void emit(const nlohmann::json& env,
          const std::string& key)
{
    try {
        const auto payload = env.dump();
        producer().produce(kTopic, key, payload);
    } catch (const std::exception& e) {
        spdlog::warn(
            "SearchEventPublisher emit failed: {}",
            e.what());
    }
}
} // namespace

void SearchEventPublisher::publish(
    const std::string& op,
    const std::string& indexName,
    const std::string& id,
    const nlohmann::json& doc)
{
    nlohmann::json env;
    env["op"] = op;
    env["index"] = indexName;
    env["id"] = id;
    if (op == "upsert") env["doc"] = doc;
    emit(env, indexName + ":" + id);
}

void SearchEventPublisher::publishDelete(
    const std::string& indexName,
    const std::string& id)
{
    publish("delete", indexName, id,
            nlohmann::json::object());
}

} // namespace nextra::search
