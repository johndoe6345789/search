#pragma once
/**
 * @file services/search/KafkaConsumerStub.h
 * @brief Adapter implementing the search-local
 *        @ref nextra::search::IKafkaConsumer (start/stop) on
 *        top of the unified @ref nextra::infra::IKafkaConsumer
 *        factory. Includes a 5-min error-rate canary that
 *        emits an alert when poll errors exceed a threshold.
 */

#include "infra/backend/IKafkaConsumer.h"
#include "infra/backend/KafkaFactory.h"
#include "search/backend/SearchTypes.h"

#include <spdlog/spdlog.h>

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <utility>

namespace nextra::search
{

/// Tick the per-poll error counter and emit a warning
/// alert if errors > kThreshold within the last window.
void noteKafkaPoll(bool ok, std::atomic<int>& errCount,
    std::atomic<long long>& windowStartMs);

/**
 * @class KafkaConsumerStub
 * @brief Thin start/stop wrapper around an infra consumer.
 */
class KafkaConsumerStub : public IKafkaConsumer
{
  public:
    explicit KafkaConsumerStub(std::string topic)
        : topic_(std::move(topic)) {}

    ~KafkaConsumerStub() override { stop(); }

    void setHandler(nextra::infra::KafkaMessageHandler h)
    { handler_ = std::move(h); }

    void start() override
    {
        inner_ = nextra::infra::makeKafkaConsumer(
            std::string{}, "search-indexer", topic_);
        if (handler_) {
            inner_->setHandler(handler_);
        } else {
            inner_->setHandler(
                [this](const std::string& k,
                       const std::string& v) {
                    spdlog::debug(
                        "search reindex msg key={} "
                        "len={}", k, v.size());
                });
        }
        running_.store(true);
        worker_ = std::thread([this] {
            while (running_.load()) {
                try {
                    inner_->poll(500);
                    noteKafkaPoll(true, errCount_,
                                  windowStart_);
                } catch (...) {
                    noteKafkaPoll(false, errCount_,
                                  windowStart_);
                }
            }
        });
        spdlog::info(
            "search consumer started (topic={})",
            topic_);
    }

    void stop() override
    {
        if (!running_.exchange(false)) return;
        if (worker_.joinable()) worker_.join();
        if (inner_) inner_->close();
        spdlog::info(
            "search consumer stopped (topic={})",
            topic_);
    }

  private:
    std::string topic_;
    nextra::infra::KafkaMessageHandler handler_;
    std::unique_ptr<nextra::infra::IKafkaConsumer> inner_;
    std::atomic<bool> running_{false};
    std::atomic<int> errCount_{0};
    std::atomic<long long> windowStart_{0};
    std::thread worker_;
};

} // namespace nextra::search
