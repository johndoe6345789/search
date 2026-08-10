/**
 * @file IndexerAlerts.cpp
 * @brief Helper that fires a backfill-failure alert
 *        with consistent dedupe key + metadata.
 */

#include "search/backend/IndexerAlerts.h"

#include "alerts/backend/AlertEmitter.h"

#include <format>

namespace nextra::search
{

void emitBackfillAlert(const IndexDef& def, int code,
                       const std::string& msg)
{
    nextra::alerts::alertEmitter().emit(
        "search-indexer", "error",
        std::format("backfill failed for {}: {}",
                    def.name, msg),
        std::format("search-indexer.backfill.{}",
                    def.esIndex),
        nlohmann::json{
            {"code",    code},
            {"message", msg},
            {"table",   def.targetTable}});
}

} // namespace nextra::search
