#pragma once
/**
 * @file SearchService.h
 * @brief Multi-index Elasticsearch search service.
 */

#include "elasticsearch/backend/ElasticClient.h"
#include "users/backend/user_service_types.h"

#include <cstdint>
#include <string>

#include <nlohmann/json.hpp>

namespace services
{

using json = nlohmann::json;

/**
 * @class SearchService
 * @brief Provides cross-index search, document indexing,
 *        and document deletion for Elasticsearch.
 */
class SearchService
{
  public:
    SearchService();
    ~SearchService() = default;

    /**
     * @brief Search across all nextra ES indices.
     * @param query   Free-text search query.
     * @param page    1-based page number.
     * @param perPage Results per page (max 100).
     * @param onOk    Success callback with results JSON.
     * @param onErr   Error callback.
     */
    void searchAll(const std::string& query,
                   std::int32_t page,
                   std::int32_t perPage,
                   Callback onOk, ErrCallback onErr);

    /**
     * @brief Federated top-N autocomplete across
     *        every registered index.
     * @param query Free-text search term.
     * @param limit Max items, clamped to [1, 50].
     * @param onOk  Receives `{"items": [...]}`
     *              with each hit shaped as
     *              `{type,id,title,snippet,url}`.
     * @param onErr Error callback.
     */
    void suggest(const std::string& query,
                 std::int32_t limit,
                 Callback onOk, ErrCallback onErr);

    /**
     * @brief Index a user document into nextra-users.
     * @param userId   Document ID for the user.
     * @param userData JSON with user fields.
     * @param onOk     Success callback.
     * @param onErr    Error callback.
     */
    void indexUser(const std::string& userId,
                   const json& userData,
                   Callback onOk, ErrCallback onErr);

    /**
     * @brief Index a chat message into nextra-chat.
     * @param messageId Document ID for the message.
     * @param msgData   JSON with message fields.
     * @param onOk      Success callback.
     * @param onErr     Error callback.
     */
    void indexChatMessage(const std::string& messageId,
                          const json& msgData,
                          Callback onOk,
                          ErrCallback onErr);

    /**
     * @brief Remove a document from any index.
     * @param index Index name.
     * @param docId Document ID to remove.
     * @param onOk  Success callback.
     * @param onErr Error callback.
     */
    void removeDoc(const std::string& index,
                   const std::string& docId,
                   Callback onOk, ErrCallback onErr);

  private:
    ElasticClient es_;
};

} // namespace services
