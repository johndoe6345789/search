/**
 * @file KafkaConsumerStub.cpp
 * @brief Out-of-line helper for the Kafka poll-error
 *        rate canary used by KafkaConsumerStub.
 */

#include "search/backend/KafkaConsumerStub.h"

#include "alerts/backend/AlertEmitter.h"

#include <chrono>

namespace nextra::search
{

namespace
{
constexpr int  kThreshold     = 30;
constexpr long kWindowMs      = 5 * 60 * 1000;
} // namespace

void noteKafkaPoll(bool ok, std::atomic<int>& errCount,
    std::atomic<long long>& windowStartMs)
{
    auto now = std::chrono::duration_cast<
        std::chrono::milliseconds>(
            std::chrono::steady_clock::now()
                .time_since_epoch()).count();
    long long start = windowStartMs.load();
    if (start == 0 || now - start > kWindowMs) {
        windowStartMs.store(now);
        errCount.store(0);
    }
    if (ok) {
        errCount.store(0);
        return;
    }
    int n = errCount.fetch_add(1) + 1;
    if (n > kThreshold) {
        nextra::alerts::alertEmitter().emit(
            "search-indexer", "warning",
            "Kafka poll error rate exceeded "
            "(>30 in 5 min)",
            "search-indexer.kafka.poll",
            nlohmann::json{
                {"errors_in_window", n},
                {"window_ms",        kWindowMs}});
        errCount.store(0);
        windowStartMs.store(now);
    }
}

} // namespace nextra::search
