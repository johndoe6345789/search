#pragma once
/**
 * @file commands/search_indexer.h
 * @brief CLI subcommand: run the search-indexer
 *        daemon (Phase 4.5).
 */

#include <string>

namespace commands
{

/**
 * @brief Run the search-indexer daemon.
 *
 * Loads @c constants/search-indexer.json,
 * constructs one @ref nextra::search::Indexer per
 * registered index, starts the Kafka consumer
 * (stub in dev builds), and drives the hourly
 * reindex timer.  Also serves the small Drogon
 * HTTP listener that exposes the admin controller
 * (`/api/search/*`) for the operator tool.
 *
 * @param config Path to a Drogon JSON config with
 *               DB credentials and listen ports.
 * @throws std::runtime_error on bad config.
 */
void cmdSearchIndexer(const std::string& config);

} // namespace commands
