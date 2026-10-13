#pragma once
/**
 * @file SearchIndexMappings.h
 * @brief Per-index Elasticsearch mappings for the
 *        authoritative indexes (Nextra's 6 plus
 *        LaunchPad's 4; SearchIndexInit only creates
 *        those listed in constants.json) managed by the
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
       }}}},
      // --------------------------------------------------
      // Knowledge base: guides, playbooks, benchmarks.
      // Admin-curated content per startup type.
      // --------------------------------------------------
      // Backed by wiki_pages + 002_kb_metadata cols.
      // Only rows WHERE kb_type IS NOT NULL are indexed.
      {"launchpad-kb",
       {{"properties", {
         {"kb_type",      {{"type", "keyword"}}},
         {"startup_type", {{"type", "keyword"}}},
         {"stage",        {{"type", "keyword"}}},
         {"title",        {{"type", "text"}}},
         {"body_md",      {{"type", "text"}}},
         {"tags",         {{"type", "keyword"}}},
         {"updated_at",   {{"type", "date"}}}
       }}}},
      // --------------------------------------------------
      // Planner steps: dynamic per startup type.
      // Lets founders search for a step by name.
      // --------------------------------------------------
      {"launchpad-planner",
       {{"properties", {
         {"step_id",       {{"type", "keyword"}}},
         {"startup_type",  {{"type", "keyword"}}},
         {"stage",         {{"type", "keyword"}}},
         {"title",         {{"type", "text"}}},
         {"description",   {{"type", "text"}}},
         {"tags",          {{"type", "keyword"}}}
       }}}},
      // --------------------------------------------------
      // Community forum posts + milestone comments.
      // Scoped by startup_type for type-specific boards.
      // --------------------------------------------------
      {"launchpad-community",
       {{"properties", {
         {"target_type",  {{"type", "keyword"}}},
         {"target_id",    {{"type", "keyword"}}},
         {"author_id",    {{"type", "keyword"}}},
         {"startup_type", {{"type", "keyword"}}},
         {"title",        {{"type", "text"}}},
         {"body",         {{"type", "text"}}},
         {"created_at",   {{"type", "date"}}}
       }}}},
      // --------------------------------------------------
      // Founder profiles: username, display name,
      // startup type and current stage — so founders
      // can find peers at the same stage.
      // --------------------------------------------------
      {"launchpad-founders",
       {{"properties", {
         {"username",     {{"type", "text"}}},
         {"display_name", {{"type", "text"}}},
         {"startup_type", {{"type", "keyword"}}},
         {"stage",        {{"type", "keyword"}}},
         {"bio",          {{"type", "text"}}}
       }}}}
    };
}

} // namespace services
