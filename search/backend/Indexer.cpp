/**
 * @file services/search/Indexer.cpp
 * @brief Reindex control flow. Uses execSqlAsync
 *        end-to-end so every step (DB read, ES
 *        bulk POST, status update) runs on the
 *        Drogon event-loop thread without ever
 *        blocking it.
 */

#include "search/backend/Indexer.h"
#include "search/backend/IndexerAlerts.h"

#include <drogon/orm/Exception.h>

#include <format>

namespace nextra::search
{

Indexer::Indexer(drogon::orm::DbClientPtr db,
                 std::shared_ptr<ElasticClient> es,
                 IndexDef def,
                 std::int32_t batchSize)
    : db_(std::move(db)),
      es_(std::move(es)),
      def_(std::move(def)),
      batchSize_(batchSize)
{}

void Indexer::reindex(EsOk onOk, EsErr onErr)
{
    writeStatus("reindexing", 0);
    es_->createIndex(def_.esIndex,
        [this, onOk, onErr](json) {
            runBulk(onOk, onErr);
        },
        [this, onErr](int c, std::string m) {
            writeStatus("error", 0);
            emitBackfillAlert(def_, c, m);
            onErr(c, m);
        });
}

void Indexer::runBulk(EsOk onOk, EsErr onErr)
{
    auto sql = std::format(
        "SELECT * FROM {} LIMIT {}",
        def_.targetTable, batchSize_);
    db_->execSqlAsync(sql,
        [this, onOk, onErr](
            const drogon::orm::Result& rows) {
            auto body = buildNdjson(rows);
            auto n = static_cast<std::int64_t>(
                rows.size());
            if (body.empty()) {
                writeStatus("idle", 0);
                onOk(json{{"indexed", 0}});
                return;
            }
            es_->bulk(def_.esIndex, body,
                [this, onOk, n](json r) {
                    writeStatus("idle", n);
                    onOk(r);
                },
                [this, onErr](int c,
                              std::string m) {
                    writeStatus("error", 0);
                    emitBackfillAlert(def_, c, m);
                    onErr(c, m);
                });
        },
        [this, onErr](
            const drogon::orm::DrogonDbException& e) {
            writeStatus("error", 0);
            std::string m = e.base().what();
            emitBackfillAlert(def_, 500, m);
            onErr(500, m);
        });
}

} // namespace nextra::search
