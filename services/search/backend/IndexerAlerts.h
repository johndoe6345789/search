#pragma once
/**
 * @file IndexerAlerts.h
 * @brief Emit a search-indexer backfill alert.
 */

#include "search/backend/SearchTypes.h"

#include <string>

namespace nextra::search
{

/**
 * @brief Emit one alert describing a failed backfill.
 * @param def   Index definition (name, esIndex, table).
 * @param code  ES/HTTP status or 500 for DB errors.
 * @param msg   Human-readable failure message.
 */
void emitBackfillAlert(const IndexDef& def, int code,
                       const std::string& msg);

} // namespace nextra::search
