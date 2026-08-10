/**
 * @file commands/search_indexer.cpp
 * @brief Daemon main: boot ES+DB, run backfill, arm
 *        periodic resync, run kafka loop till signal.
 */
#include "search/backend/commands/search_indexer.h"
#include "search/backend/commands/search_indexer_config.h"
#include "search/backend/commands/search_indexer_dispatch.h"
#include "search/backend/commands/search_indexer_logging.h"
#include "search/backend/commands/search_indexer_reindex.h"
#include "search/backend/commands/search_indexer_topic_ensure.h"

#include "elasticsearch/backend/ElasticClient.h"
#include "search/backend/ElasticClient.h"
#include "search/backend/KafkaConsumerStub.h"
#include "search/backend/SearchHealthCanary.h"
#include "search/backend/SearchIndexInit.h"
#include "search/backend/SearchTypes.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <csignal>
#include <future>
#include <memory>
#include <thread>

namespace
{
std::atomic<bool> g_stop{false};
void onSignal(int) { g_stop.store(true); }
} // namespace

namespace commands
{

void cmdSearchIndexer(const std::string& config)
{
    using namespace nextra::search;
    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);
    initIndexerLogging();

    drogon::app().loadConfigFile(config);
    auto cfg = loadSearchConfig(
        "constants/search-indexer.json");

    KafkaConsumerStub consumer(cfg.kafkaTopic);

    std::promise<void> started;
    auto startedFut = started.get_future();
    drogon::app().registerBeginningAdvice(
        [&started, &consumer, cfg]() {
            // Statics outlive async ES callbacks.
            static auto adminEs = std::make_shared<
                services::ElasticClient>();
            services::SearchIndexInit::createAll(
                *adminEs);
            static auto idxEs = std::make_shared<
                ElasticClient>(
                    cfg.esHost, cfg.esPort);
            static auto db =
                drogon::app().getDbClient();
            consumer.setHandler(
                buildSearchDispatcher(cfg, idxEs));
            ensureSearchTopic(cfg.kafkaTopic);
            consumer.start();
            commands::reindexAll(cfg, db, idxEs);
            drogon::app().getLoop()->runEvery(
                static_cast<double>(
                    cfg.reindexIntervalSeconds),
                [cfg] {
                    commands::reindexAll(
                        cfg, db, idxEs);
                });
            armHealthCanary(db, idxEs);
            spdlog::info(
                "search: periodic resync armed "
                "every {}s",
                cfg.reindexIntervalSeconds);
            started.set_value();
        });

    std::thread httpThread([] {
        drogon::app().run(); });
    startedFut.wait();

    spdlog::info(
        "search-indexer ready ({} indexes)",
        cfg.indexes.size());
    while (!g_stop.load())
        std::this_thread::sleep_for(
            std::chrono::milliseconds(500));
    consumer.stop();
    drogon::app().quit();
    if (httpThread.joinable()) httpThread.join();
}

} // namespace commands
