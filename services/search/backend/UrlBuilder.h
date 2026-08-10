#pragma once
/**
 * @file UrlBuilder.h
 * @brief Map an ES `_index` + hit source to a
 *        frontend-relative URL. Centralised here
 *        so callers (suggest, search) emit links
 *        in one consistent shape.
 */

#include <nlohmann/json.hpp>
#include <string>

namespace services
{

using json = nlohmann::json;

/**
 * @class UrlBuilder
 * @brief Tiny static helper. No state.
 */
class UrlBuilder
{
  public:
    /**
     * @brief Build a frontend URL for one hit.
     * @param esIndex The ES index name
     *                (e.g. "nextra-forum").
     * @param id      Document id as string.
     * @param src     The hit's `_source` JSON, used
     *                when the URL needs a slug or
     *                related field.
     * @return Path like "/forum/threads/<id>".
     *         Returns "#" if @p esIndex is unknown.
     */
    static std::string build(const std::string& esIndex,
                             const std::string& id,
                             const json& src);

    /**
     * @brief Map an ES index name to its logical
     *        "type" label used in the suggest API
     *        response shape.
     */
    static std::string typeOf(
        const std::string& esIndex);
};

} // namespace services
