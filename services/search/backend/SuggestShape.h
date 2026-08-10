#pragma once
/**
 * @file SuggestShape.h
 * @brief Inline helpers that turn a raw ES hit
 *        into the shape returned by /api/search
 *        /suggest. Header-only so the .cpp stays
 *        under the 100-LOC cap.
 */

#include "search/backend/UrlBuilder.h"

#include <nlohmann/json.hpp>
#include <string>

namespace services::suggest_shape
{

using json = nlohmann::json;

/// Pick the best display string for a hit.
inline std::string pickTitle(const json& src)
{
    for (const char* k : {"title", "name",
                          "display_name",
                          "username", "slug",
                          "sku"}) {
        auto it = src.find(k);
        if (it != src.end() && it->is_string()) {
            return it->get<std::string>();
        }
    }
    return {};
}

/// Short snippet from common body fields.
inline std::string pickSnippet(const json& src)
{
    for (const char* k : {"description", "body",
                          "body_md"}) {
        auto it = src.find(k);
        if (it != src.end() && it->is_string()) {
            auto s = it->get<std::string>();
            if (s.size() > 160) s.resize(160);
            return s;
        }
    }
    return {};
}

/// Map one ES hit to the suggest response shape.
inline json shape(const json& hit)
{
    auto idx = hit.value("_index", "");
    auto id = hit.value("_id", "");
    json src = hit.value("_source", json::object());
    return {
        {"type",    UrlBuilder::typeOf(idx)},
        {"id",      id},
        {"title",   pickTitle(src)},
        {"snippet", pickSnippet(src)},
        {"url",     UrlBuilder::build(idx, id, src)}
    };
}

} // namespace services::suggest_shape
