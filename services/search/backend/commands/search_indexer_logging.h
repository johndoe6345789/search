#pragma once
/**
 * @file commands/search_indexer_logging.h
 * @brief Routes the search-indexer's own spdlog
 *        info messages to stdout regardless of the
 *        Drogon worker @c log_level. Drogon's file
 *        sink stays at WARN per the prod config —
 *        this only adds a parallel stdout sink for
 *        the daemon's own logger.
 */

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>

namespace commands
{

/**
 * @brief Install a colour stdout sink at info level
 *        as the spdlog default logger. Idempotent —
 *        calling twice replaces the previous sink.
 */
inline void initIndexerLogging()
{
    auto sink = std::make_shared<
        spdlog::sinks::stdout_color_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>(
        "search-indexer", sink);
    logger->set_level(spdlog::level::info);
    logger->set_pattern(
        "[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);
    spdlog::flush_on(spdlog::level::info);
}

} // namespace commands
