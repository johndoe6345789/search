#pragma once
/**
 * @file SearchIndexMappings.h
 * @brief Per-index Elasticsearch mappings for the
 *        6 authoritative indexes managed by the
 *        search-indexer daemon. Kept in their own
 *        translation unit so SearchIndexInit.cpp
 *        stays under the 100-LOC cap.
 *
 * Names are kept in lock-step with
 * services/search/constants.json — the indexer
 * itself reads constants.json at runtime; this
 * header is only consulted at boot to ensure each
 * ES index exists with the right mapping shape.
 */

#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>

namespace services
{

using json = nlohmann::json;

/// One (es-index-name, mappings-doc) pair.
using IndexMapping = std::pair<std::string, json>;

/**
 * @brief Authoritative mappings for the 6 indexes.
 *
 * `text` for searchable strings, `keyword` for
 * IDs/enums/slugs, `date` for timestamps. Other
 * scalars (booleans) are mapped explicitly so ES
 * does not infer surprising types.
 */
inline std::vector<IndexMapping> allIndexMappings()
{
    return {
      {"nextra-forum",
       {{"properties", {
         {"target_type", {{"type", "keyword"}}},
         {"target_id",   {{"type", "keyword"}}},
         {"author_id",   {{"type", "keyword"}}},
         {"title",       {{"type", "text"}}},
         {"body",        {{"type", "text"}}},
         {"created_at",  {{"type", "date"}}}
       }}}},
      {"nextra-wiki",
       {{"properties", {
         {"slug",       {{"type", "keyword"}}},
         {"title",      {{"type", "text"}}},
         {"body_md",    {{"type", "text"}}},
         {"updated_at", {{"type", "date"}}}
       }}}},
      {"nextra-blog",
       {{"properties", {
         {"slug",         {{"type", "keyword"}}},
         {"title",        {{"type", "text"}}},
         {"body_md",      {{"type", "text"}}},
         {"author_id",    {{"type", "keyword"}}},
         {"status",       {{"type", "keyword"}}},
         {"published_at", {{"type", "date"}}}
       }}}},
      {"nextra-products",
       {{"properties", {
         {"sku",         {{"type", "keyword"}}},
         {"name",        {{"type", "text"}}},
         {"description", {{"type", "text"}}},
         {"active",      {{"type", "keyword"}}}
       }}}},
      {"nextra-gallery",
       {{"properties", {
         {"slug",        {{"type", "keyword"}}},
         {"title",       {{"type", "text"}}},
         {"description", {{"type", "text"}}},
         {"owner_id",    {{"type", "keyword"}}},
         {"created_at",  {{"type", "date"}}}
       }}}},
      {"nextra-users",
       {{"properties", {
         {"username",     {{"type", "text"}}},
         {"email",        {{"type", "text"}}},
         {"display_name", {{"type", "text"}}},
         {"role",         {{"type", "keyword"}}}
       }}}}
    };
}

} // namespace services
